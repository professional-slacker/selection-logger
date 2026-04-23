# Quick Start Guide

## Overview
Selection Logger is a cross-platform clipboard/selection logger with automatic monitoring for Linux and Windows.

## Quick Start (Linux)

### 1. Install Dependencies
```bash
# Ubuntu/Debian
sudo apt-get install libx11-dev xclip

# Fedora/RHEL
sudo dnf install libX11-devel xclip
```

### 2. Build and Run
```bash
# Clone repository
git clone https://github.com/yourusername/selection-logger.git
cd selection-logger

# Build for Linux
make clean && make PLATFORM=linux

# Run automatic monitoring
./selection-logger-auto --both
```

## Quick Start (Windows)

### 1. Build on Linux (Cross-compilation)
```bash
# Build Windows executable (statically linked)
make clean && make PLATFORM=windows STATIC=1

# Test with Wine
./test_wine_auto.sh
```

### 2. Run on Windows
```cmd
selection-logger-auto.exe --both
```

## Development Testing with Wine

### Test Windows Builds on Linux
```bash
# Build Windows version
make clean && make PLATFORM=windows STATIC=1

# Run Wine tests
./test_wine_auto.sh
```

### Manual Testing
```bash
# Test help
wine selection-logger-auto.exe --help

# Test different modes
wine selection-logger-auto.exe --primary --version
wine selection-logger-auto.exe --clipboard --version
wine selection-logger-auto.exe --both --interval 500
```

## Common Commands

### Build Commands
```bash
# Linux build
make PLATFORM=linux

# Windows build (static linking)
make PLATFORM=windows STATIC=1

# Release build
make clean && make release PLATFORM=windows

# Clean build
make clean
```

### Runtime Commands
```bash
# Monitor both selections (recommended)
./selection-logger-auto --both

# Monitor mouse selections only
./selection-logger-auto --primary

# Monitor Ctrl+C copies only
./selection-logger-auto --clipboard

# Faster polling (500ms)
./selection-logger-auto --both --interval 500

# Run in background
./selection-logger-auto --both &
```

## Log Files
- **Location**: `~/memories/Year/Month.txt`
- **Example**: `~/memories/2026/04.txt`
- **Format**: Timestamped text entries

## Troubleshooting

### Linux Issues
```bash
# Check X11 dependencies
ldd selection-logger-auto

# Check xclip installation
which xclip
xclip -version
```

### Windows/Wine Issues
```bash
# Check Wine installation
wine --version

# Check static linking
objdump -p selection-logger-auto.exe | grep "DLL Name:"

# Test without Wine (native Windows)
# Copy selection-logger-auto.exe to Windows machine
```

### Build Issues
```bash
# Clean and rebuild
make clean
make PLATFORM=linux  # or PLATFORM=windows STATIC=1

# Check compiler
g++ --version
x86_64-w64-mingw32-g++ --version
```

## Next Steps
1. Add to startup applications for automatic logging
2. Customize polling interval for performance
3. Review log files in `~/memories/` directory