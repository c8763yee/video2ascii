# Video2ASCII - C Remake

A high-performance C implementation of video2ascii that converts video files into colored ASCII art displayed in the terminal. This version supports 24-bit color output and uses FFmpeg for robust video decoding.

## Features

- **Video Decoding**: Uses FFmpeg libraries for wide format support
- **Smart Scaling**: Automatically scales video to max 80 characters width while maintaining aspect ratio
- **24-bit Color Support**: Full RGB color output using ANSI escape codes (`\033[38;2;R;G;Bm`)
- **ASCII Mapping**: Converts pixel brightness to ASCII characters for artistic effect
- **Real-time Playback**: Approximately 30 FPS terminal output
- **Wide Format Support**: Supports all video formats that FFmpeg can decode

## Requirements

### System Dependencies

- **GCC Compiler**: C99 compatible compiler
- **FFmpeg Development Libraries**:
  - `libavformat-dev` - Format handling
  - `libavcodec-dev` - Video decoding
  - `libavutil-dev` - Utility functions
  - `libswscale-dev` - Image scaling
- **pkg-config** (recommended for automatic library detection)

### Installation on Ubuntu/Debian

```bash
sudo apt update
sudo apt install gcc ffmpeg libavformat-dev libavcodec-dev libavutil-dev libswscale-dev pkg-config
```

### Installation on CentOS/RHEL/Fedora

```bash
# Fedora
sudo dnf install gcc ffmpeg-devel pkgconfig

# CentOS/RHEL (with EPEL repository)
sudo yum install gcc ffmpeg-devel pkgconfig
```

### Installation on macOS

```bash
# Using Homebrew
brew install gcc ffmpeg pkg-config
```

## Building

1. **Clone and navigate to the C-remake directory**:
   ```bash
   cd C-remake
   ```

2. **Check dependencies** (optional):
   ```bash
   make check-deps
   ```

3. **Build the executable**:
   ```bash
   make
   ```

4. **Install system-wide** (optional):
   ```bash
   make install
   ```

## Usage

### Basic Usage

```bash
./video2ascii <video_file>
```

### Examples

```bash
# Play a local video file
./video2ascii /path/to/your/video.mp4

# Play different formats
./video2ascii movie.avi
./video2ascii animation.mkv
./video2ascii clip.mov
```

### Supported Formats

Any video format supported by FFmpeg, including but not limited to:
- MP4 (H.264, H.265)
- AVI
- MKV
- MOV
- WebM
- FLV
- WMV
- And many more...

## How It Works

1. **Video Decoding**: FFmpeg libraries decode video frames from the input file
2. **Frame Scaling**: Each frame is resized to a maximum width of 80 characters while preserving aspect ratio
3. **Color Processing**: RGB values are preserved for 24-bit color output
4. **Brightness Calculation**: Luminance is calculated using the formula: `0.299*R + 0.587*G + 0.114*B`
5. **ASCII Mapping**: Brightness values are mapped to ASCII characters: `@#$%?*+;:,.`
6. **Color Output**: Each character is colored using ANSI escape codes with original RGB values
7. **Terminal Display**: Frames are displayed at ~30 FPS with screen clearing between frames

## Technical Details

### Color Support

The program outputs 24-bit color using ANSI escape sequences:
```
\033[38;2;R;G;Bm<character>\033[0m
```
Where R, G, B are the original pixel color values (0-255).

### ASCII Character Mapping

Characters are ordered from dense to sparse:
- `@` - Darkest pixels (highest brightness)
- `#` - Very dark
- `$` - Dark
- `%` - Medium-dark
- `?` - Medium
- `*` - Medium-light
- `+` - Light
- `;` - Lighter
- `:` - Very light
- `,` - Lightest
- `.` - Brightest pixels (lowest brightness)

### Performance Considerations

- Uses efficient FFmpeg APIs for video decoding
- SwScale library for optimized image scaling
- Direct RGB24 format processing
- Minimal memory allocation during playback
- Approximately 30 FPS playback speed

## Troubleshooting

### Common Issues

1. **"Could not open file"**
   - Check if the video file exists and is readable
   - Verify the file format is supported by your FFmpeg installation

2. **Compilation errors about missing libraries**
   - Install FFmpeg development packages
   - Check that pkg-config can find the libraries: `pkg-config --list-all | grep av`

3. **Colors not displaying properly**
   - Ensure your terminal supports 24-bit color (most modern terminals do)
   - Try a different terminal if colors appear incorrect

4. **Playback too fast/slow**
   - The program uses a fixed 30 FPS rate
   - Terminal performance may affect actual display speed

### Terminal Compatibility

The program uses standard ANSI escape codes and should work with:
- Modern terminal emulators (GNOME Terminal, Konsole, Terminal.app, etc.)
- SSH sessions with color support
- Most terminal multiplexers (tmux, screen)

## Building Without pkg-config

If pkg-config is not available, the Makefile will attempt to link libraries directly:

```bash
gcc -Wall -Wextra -std=c99 -O2 -o video2ascii main.c -lavformat -lavcodec -lavutil -lswscale -lm
```

## Makefile Targets

- `make` or `make all` - Build the executable
- `make install` - Install to /usr/local/bin
- `make clean` - Remove build files
- `make check-deps` - Check for required dependencies
- `make help` - Show help information

## License

This is a complete rewrite in C for educational and practical purposes. Please ensure you have appropriate rights to use any video files you process.

## Comparison with Python Version

This C implementation offers several advantages over the original Python version:
- **Performance**: Significantly faster video processing and display
- **Memory Efficiency**: Lower memory usage with direct C memory management
- **Color Support**: Full 24-bit color vs. grayscale ASCII
- **Format Support**: Broader video format support through FFmpeg
- **Standalone**: No Python runtime dependencies required