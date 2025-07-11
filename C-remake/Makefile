# Makefile for video2ascii C remake

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LIBS = -lavformat -lavcodec -lavutil -lswscale -lm

# Target executable
TARGET = video2ascii
SOURCE = main.c

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(SOURCE)
	@echo "Compiling $(TARGET)..."
	@if pkg-config --exists libavformat libavcodec libavutil libswscale; then \
		echo "Using pkg-config for FFmpeg libraries..."; \
		$(CC) $(CFLAGS) -DHAVE_FFMPEG `pkg-config --cflags libavformat libavcodec libavutil libswscale` \
		-o $(TARGET) $(SOURCE) `pkg-config --libs libavformat libavcodec libavutil libswscale` -lm; \
	elif ldconfig -p | grep -q libavformat; then \
		echo "FFmpeg libraries found, using standard paths..."; \
		$(CC) $(CFLAGS) -DHAVE_FFMPEG -o $(TARGET) $(SOURCE) $(LIBS); \
	else \
		echo "FFmpeg libraries not found, building demonstration version..."; \
		$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) -lm; \
		echo "Note: This version will demonstrate features but cannot process actual video files."; \
	fi
	@echo "Build complete!"

# Install target (optional)
install: $(TARGET)
	@echo "Installing $(TARGET) to /usr/local/bin..."
	sudo cp $(TARGET) /usr/local/bin/
	@echo "Installation complete!"

# Clean target
clean:
	@echo "Cleaning build files..."
	rm -f $(TARGET)
	@echo "Clean complete!"

# Check dependencies
check-deps:
	@echo "Checking for required dependencies..."
	@if command -v pkg-config >/dev/null 2>&1; then \
		echo "✓ pkg-config found"; \
		if pkg-config --exists libavformat; then \
			echo "✓ libavformat found"; \
		else \
			echo "✗ libavformat not found"; \
		fi; \
		if pkg-config --exists libavcodec; then \
			echo "✓ libavcodec found"; \
		else \
			echo "✗ libavcodec not found"; \
		fi; \
		if pkg-config --exists libavutil; then \
			echo "✓ libavutil found"; \
		else \
			echo "✗ libavutil not found"; \
		fi; \
		if pkg-config --exists libswscale; then \
			echo "✓ libswscale found"; \
		else \
			echo "✗ libswscale not found"; \
		fi; \
	else \
		echo "✗ pkg-config not found"; \
	fi
	@if command -v gcc >/dev/null 2>&1; then \
		echo "✓ gcc found"; \
	else \
		echo "✗ gcc not found"; \
	fi

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build the video2ascii executable (default)"
	@echo "  install   - Install the executable to /usr/local/bin"
	@echo "  clean     - Remove build files"
	@echo "  check-deps - Check for required dependencies"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Dependencies required:"
	@echo "  - gcc compiler"
	@echo "  - FFmpeg development libraries:"
	@echo "    - libavformat-dev"
	@echo "    - libavcodec-dev" 
	@echo "    - libavutil-dev"
	@echo "    - libswscale-dev"
	@echo "  - pkg-config (recommended)"

# Phony targets
.PHONY: all install clean check-deps help