#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

// FFmpeg headers - only include if available
#ifdef HAVE_FFMPEG
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#else
// Fallback definitions for demonstration when FFmpeg is not available
typedef struct AVFormatContext AVFormatContext;
typedef struct AVCodecContext AVCodecContext;
typedef struct AVCodec AVCodec;
typedef struct AVFrame AVFrame;
typedef struct AVPacket AVPacket;
typedef struct SwsContext SwsContext;
typedef unsigned char uint8_t;
#define AV_PIX_FMT_RGB24 0
#define AVMEDIA_TYPE_VIDEO 0
#define SWS_BILINEAR 0
#endif

// Constants
#define MAX_WIDTH 80
#define ASCII_CHARS "@#$%?*+;:,."
#define ASCII_CHARS_LEN 11

// Structure to hold video information
typedef struct {
    AVFormatContext *format_ctx;
    AVCodecContext *codec_ctx;
    AVCodec *codec;
    AVFrame *frame;
    AVFrame *frame_rgb;
    AVPacket *packet;
    struct SwsContext *sws_ctx;
    int video_stream;
    int width;
    int height;
    int scaled_width;
    int scaled_height;
    uint8_t *buffer;
} VideoContext;

// Function prototypes
int init_video(VideoContext *ctx, const char *filename);
void cleanup_video(VideoContext *ctx);
int decode_frame(VideoContext *ctx);
void convert_frame_to_ascii(VideoContext *ctx);
void print_colored_ascii(uint8_t *rgb_data, int width, int height);
char get_ascii_char(int brightness);
void clear_screen(void);
void demo_colored_ascii(void);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <video_file>\n", argv[0]);
        return 1;
    }

#ifdef HAVE_FFMPEG
    VideoContext ctx = {0};
    
    // Initialize FFmpeg
    av_register_all();
    avcodec_register_all();
    
    if (init_video(&ctx, argv[1]) < 0) {
        fprintf(stderr, "Failed to initialize video: %s\n", argv[1]);
        return 1;
    }

    printf("Video Info: %dx%d, scaled to %dx%d\n", 
           ctx.width, ctx.height, ctx.scaled_width, ctx.scaled_height);
    printf("Press Ctrl+C to exit\n\n");
    
    // Sleep for a moment to let user read the info
    usleep(2000000); // 2 seconds

    // Main decode and display loop
    while (decode_frame(&ctx) >= 0) {
        convert_frame_to_ascii(&ctx);
        usleep(33333); // ~30 FPS (1/30 second = 33333 microseconds)
    }

    cleanup_video(&ctx);
#else
    // Fallback demonstration when FFmpeg is not available
    printf("=== Video2ASCII C Implementation Demo ===\n");
    printf("Input file: %s\n", argv[1]);
    printf("\nThis is a demonstration of the video2ascii C implementation.\n");
    printf("FFmpeg libraries are not available in this environment.\n\n");
    
    printf("Features implemented:\n");
    printf("✓ Command line argument parsing\n");
    printf("✓ FFmpeg integration structure\n");
    printf("✓ Image scaling algorithms (max 80 chars width)\n");
    printf("✓ 24-bit RGB to ASCII conversion\n");
    printf("✓ ANSI color output support\n");
    printf("✓ Terminal display functions\n\n");
    
    // Demonstrate ASCII character mapping
    printf("ASCII brightness mapping: ");
    for (int i = 0; i < ASCII_CHARS_LEN; i++) {
        printf("%c", ASCII_CHARS[i]);
    }
    printf("\n");
    
    // Demonstrate color output
    printf("\nColor output demo (24-bit ANSI):\n");
    demo_colored_ascii();
    
    printf("\nTo use with actual video files, install FFmpeg libraries:\n");
    printf("sudo apt install libavformat-dev libavcodec-dev libavutil-dev libswscale-dev\n");
    printf("Then recompile with: make clean && make\n");
#endif
    
    return 0;
}

#ifdef HAVE_FFMPEG
int init_video(VideoContext *ctx, const char *filename) {
    // Open input file
    if (avformat_open_input(&ctx->format_ctx, filename, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        return -1;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(ctx->format_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        return -1;
    }

    // Find video stream
    ctx->video_stream = -1;
    for (int i = 0; i < ctx->format_ctx->nb_streams; i++) {
        if (ctx->format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            ctx->video_stream = i;
            break;
        }
    }

    if (ctx->video_stream == -1) {
        fprintf(stderr, "Could not find video stream\n");
        return -1;
    }

    // Get codec parameters
    AVCodecParameters *codecpar = ctx->format_ctx->streams[ctx->video_stream]->codecpar;
    
    // Find decoder
    ctx->codec = avcodec_find_decoder(codecpar->codec_id);
    if (!ctx->codec) {
        fprintf(stderr, "Unsupported codec\n");
        return -1;
    }

    // Allocate codec context
    ctx->codec_ctx = avcodec_alloc_context3(ctx->codec);
    if (!ctx->codec_ctx) {
        fprintf(stderr, "Could not allocate codec context\n");
        return -1;
    }

    // Copy codec parameters to context
    if (avcodec_parameters_to_context(ctx->codec_ctx, codecpar) < 0) {
        fprintf(stderr, "Could not copy codec parameters\n");
        return -1;
    }

    // Open codec
    if (avcodec_open2(ctx->codec_ctx, ctx->codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return -1;
    }

    // Store original dimensions
    ctx->width = ctx->codec_ctx->width;
    ctx->height = ctx->codec_ctx->height;

    // Calculate scaled dimensions (max width 80, maintain aspect ratio)
    double aspect_ratio = (double)ctx->height / (double)ctx->width;
    ctx->scaled_width = (ctx->width > MAX_WIDTH) ? MAX_WIDTH : ctx->width;
    ctx->scaled_height = (int)(ctx->scaled_width * aspect_ratio);

    // Allocate frames
    ctx->frame = av_frame_alloc();
    ctx->frame_rgb = av_frame_alloc();
    if (!ctx->frame || !ctx->frame_rgb) {
        fprintf(stderr, "Could not allocate frames\n");
        return -1;
    }

    // Calculate buffer size and allocate buffer
    int buffer_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, ctx->scaled_width, ctx->scaled_height, 1);
    ctx->buffer = (uint8_t*)av_malloc(buffer_size);
    if (!ctx->buffer) {
        fprintf(stderr, "Could not allocate buffer\n");
        return -1;
    }

    // Associate buffer with frame_rgb
    av_image_fill_arrays(ctx->frame_rgb->data, ctx->frame_rgb->linesize, 
                         ctx->buffer, AV_PIX_FMT_RGB24, 
                         ctx->scaled_width, ctx->scaled_height, 1);

    // Initialize scaling context
    ctx->sws_ctx = sws_getContext(ctx->width, ctx->height, ctx->codec_ctx->pix_fmt,
                                  ctx->scaled_width, ctx->scaled_height, AV_PIX_FMT_RGB24,
                                  SWS_BILINEAR, NULL, NULL, NULL);
    if (!ctx->sws_ctx) {
        fprintf(stderr, "Could not initialize scaling context\n");
        return -1;
    }

    // Allocate packet
    ctx->packet = av_packet_alloc();
    if (!ctx->packet) {
        fprintf(stderr, "Could not allocate packet\n");
        return -1;
    }

    return 0;
}

int decode_frame(VideoContext *ctx) {
    while (av_read_frame(ctx->format_ctx, ctx->packet) >= 0) {
        if (ctx->packet->stream_index == ctx->video_stream) {
            // Send packet to decoder
            int ret = avcodec_send_packet(ctx->codec_ctx, ctx->packet);
            if (ret < 0) {
                av_packet_unref(ctx->packet);
                continue;
            }

            // Receive frame from decoder
            ret = avcodec_receive_frame(ctx->codec_ctx, ctx->frame);
            if (ret == 0) {
                av_packet_unref(ctx->packet);
                return 0; // Successfully decoded a frame
            }
        }
        av_packet_unref(ctx->packet);
    }
    return -1; // No more frames
}

void convert_frame_to_ascii(VideoContext *ctx) {
    // Scale frame to RGB24 format
    sws_scale(ctx->sws_ctx, (const uint8_t* const*)ctx->frame->data, 
              ctx->frame->linesize, 0, ctx->height,
              ctx->frame_rgb->data, ctx->frame_rgb->linesize);

    // Clear screen and print colored ASCII
    clear_screen();
    print_colored_ascii(ctx->frame_rgb->data[0], ctx->scaled_width, ctx->scaled_height);
}

void print_colored_ascii(uint8_t *rgb_data, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pixel_index = (y * width + x) * 3; // RGB24 format
            uint8_t r = rgb_data[pixel_index];
            uint8_t g = rgb_data[pixel_index + 1];
            uint8_t b = rgb_data[pixel_index + 2];

            // Calculate brightness (luminance)
            int brightness = (int)(0.299 * r + 0.587 * g + 0.114 * b);
            
            // Get ASCII character based on brightness
            char ascii_char = get_ascii_char(brightness);
            
            // Print with 24-bit color using ANSI escape codes
            printf("\033[38;2;%d;%d;%dm%c", r, g, b, ascii_char);
        }
        printf("\033[0m\n"); // Reset color and newline
    }
    fflush(stdout);
}

void clear_screen(void) {
    // ANSI escape sequence to clear screen and move cursor to top-left
    printf("\033[2J\033[H");
}

void cleanup_video(VideoContext *ctx) {
    if (ctx->packet) av_packet_free(&ctx->packet);
    if (ctx->frame) av_frame_free(&ctx->frame);
    if (ctx->frame_rgb) av_frame_free(&ctx->frame_rgb);
    if (ctx->buffer) av_free(ctx->buffer);
    if (ctx->sws_ctx) sws_freeContext(ctx->sws_ctx);
    if (ctx->codec_ctx) avcodec_free_context(&ctx->codec_ctx);
    if (ctx->format_ctx) avformat_close_input(&ctx->format_ctx);
}
#endif

char get_ascii_char(int brightness) {
    // Map brightness (0-255) to ASCII character index
    int index = (brightness * (ASCII_CHARS_LEN - 1)) / 255;
    if (index >= ASCII_CHARS_LEN) index = ASCII_CHARS_LEN - 1;
    if (index < 0) index = 0;
    
    // Reverse order for better visual effect (darker = more dense character)
    return ASCII_CHARS[ASCII_CHARS_LEN - 1 - index];
}

// Demo function to show colored ASCII output when FFmpeg is not available
void demo_colored_ascii(void) {
    printf("Simulating colored ASCII conversion:\n");
    
    // Create a simple gradient pattern
    int width = 60;
    int height = 10;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Create RGB gradient
            uint8_t r = (x * 255) / width;
            uint8_t g = (y * 255) / height;
            uint8_t b = ((x + y) * 255) / (width + height);
            
            // Calculate brightness
            int brightness = (int)(0.299 * r + 0.587 * g + 0.114 * b);
            char ascii_char = get_ascii_char(brightness);
            
            // Print with 24-bit color
            printf("\033[38;2;%d;%d;%dm%c", r, g, b, ascii_char);
        }
        printf("\033[0m\n"); // Reset color and newline
    }
    printf("\n");
}