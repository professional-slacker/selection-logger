# selection-logger

A lightweight cross-platform daemon that monitors system text selections and automatically logs changes to dated files.

> **Windows**: [Download from Releases](https://github.com/professional-slacker/selection-logger/releases) — no build tools required.
> **Linux**: Build from source (see below) or `create_release.sh` to package.

## Features
- **Cross-platform**: Windows and Linux (Wine test compatible)
- **Automatic Capture Only**: Saves text selections automatically when they change
- **Dual Selection Monitoring**: Monitors both PRIMARY (mouse selection) and CLIPBOARD (Ctrl+C) selections
- **Smart Organization**: Automatically categorizes logs into `~/memories/YYYY-MM.txt` (Linux) or `Documents\SelectionLogs\YYYY-MM.txt` (Windows)
- **Low Overhead**: A minimal background process designed for cross-platform environments
- **Timestamped**: Every entry is recorded with a precise timestamp for later reference
- **Configurable**: Multiple monitoring modes and adjustable polling interval
- **Static Linking**: No external DLL dependencies (Windows)

## Prerequisites

### Linux/X11
You need the X11 development libraries and `xclip` installed on your system.
```bash
sudo apt update
sudo apt install libx11-dev xclip
```

### Windows
Windows 7 or later (using Win32 API, no additional dependencies required).
Build with MinGW-w64 (cross-compilation from Linux) or MSVC on Windows.
```bash
# Cross-compile from Linux with MinGW-w64
sudo apt install g++-mingw-w64-x86-64
make PLATFORM=windows STATIC=1
```

### macOS
Not yet supported.

## Download

### Windows
Pre-built binaries are available from the [Releases page](https://github.com/professional-slacker/selection-logger/releases).
Download `selection-logger-windows-x86_64.zip` and extract to any directory.

### Linux
Pre-built binaries are available from the [Releases page](https://github.com/professional-slacker/selection-logger/releases).
Download `selection-logger-linux-1.0.0.tar.gz` and extract:
```bash
tar -xzf selection-logger-linux-1.0.0.tar.gz
cd linux/
sudo cp selection-logger /usr/local/bin/
```
Or build from source (X11 libraries required): `make PLATFORM=linux`

## Build from Source

### Cross-platform Build System
This project uses a Makefile for cross-platform compilation supporting Linux, Windows, and Wine testing.

```bash
# Clone the repository
git clone https://github.com/professional-slacker/selection-logger.git
cd selection-logger

# Build for Linux (X11)
make clean && make PLATFORM=linux

# Build for Windows (statically linked, no DLL dependencies)
make clean && make PLATFORM=windows STATIC=1

# Build for release (statically linked Windows, optimized)
make clean && make release PLATFORM=windows
```

## Usage

### Basic Usage
```bash
./selection-logger
```
Starts in default (PRIMARY) mode, monitoring mouse selections.

### Command Line Options
```bash
./selection-logger [options]
```

**Options:**
- `-p, --primary`        Monitor PRIMARY selection only (mouse selection, default)
- `-c, --clipboard`      Monitor CLIPBOARD selection only (Ctrl+C)
- `-b, --both`           Monitor both PRIMARY and CLIPBOARD
- `-v, --version`        Show version information
- `-h, --help`           Show this help message

**Modes:**
- **PRIMARY**: Saves text when selected with mouse (no Ctrl+C needed, default)
- **CLIPBOARD**: Saves text when copied with Ctrl+C (works with Ctrl+A → Ctrl+C workflow)
- **BOTH**: Monitors both selections (recommended for comprehensive logging)

### Examples
```bash
# Monitor mouse selections only (default)
./selection-logger

# Monitor Ctrl+C copies only
./selection-logger --clipboard

# Monitor both selections
./selection-logger --both

# Run in background
./selection-logger --both &
```

## Directory Structure
### Linux
```text
~/memories/
    2026-04.txt  <-- Log entries are appended here
```

### Windows
```text
%USERPROFILE%\Documents\SelectionLogs\
    2026-04.txt  <-- Log entries are appended here
```

## Log Format
Entries are saved in the following format:
```text
--- PRIMARY 2026-04-28 14:30:00 ---
Your captured text selection appears here.

--- CLIPBOARD 2026-04-28 14:35:22 ---
Another important text snippet.

--- PRIMARY 2026-04-28 14:40:15 ---
file:///home/user/documents/report.pdf
```

**Note:** File copies (Ctrl+C on files) are also logged as file URIs.

## Technical Details

### Cross-platform Architecture
- **Linux/X11**: Uses xclip command-line tool for clipboard access
- **Windows**: Uses Win32 API (OpenClipboard, GetClipboardData)
- **Wine Testing**: Windows executables can be tested on Linux using Wine
- **Static Linking**: Windows builds are statically linked (no external DLL dependencies)

### How It Works
1. Uses platform-specific tools to access system selections
2. Polls selections at configurable intervals (default: 500ms)
3. Detects changes by comparing with previous selection content
4. Skips empty or whitespace-only selections
5. Creates log directories automatically if they don't exist

### Selection Types
- **PRIMARY Selection**: Updated when text is selected with mouse (middle-click paste on X11)
- **CLIPBOARD Selection**: Updated with Ctrl+C (standard copy/paste)

## Pro Tips

1. **Startup Automation**: Add to your `.xinitrc` or desktop environment's "Startup Applications":
   ```bash
   /path/to/selection-logger --both &
   ```

2. **Recommended Mode**: Use `--both` mode for comprehensive logging of all text selections.

3. **Performance**: Default 500ms interval provides good balance between responsiveness and CPU usage.

4. **File Operations**: File copies are logged as file URIs (e.g., `file:///path/to/file.txt`).

## Platform Support

### Linux (X11)
- **Dependencies**: `libx11-dev`, `xclip`
- **Install**: `sudo apt-get install libx11-dev xclip` (Ubuntu/Debian)
- **Build**: `make PLATFORM=linux`
- **Run**: `./selection-logger [options]`
- **Dist Package**: `dist/linux/` — includes install script and systemd user service

### Windows
- **Build**: `make PLATFORM=windows STATIC=1`
- **Output**: `selection-logger.exe` (statically linked)
- **Run**: `selection-logger.exe [options]`
- **Features**: No external DLL dependencies, Win32 API
- **Dist Package**: `dist/windows/` — includes install/uninstall scripts

### Wine Testing Environment
- **Purpose**: Test Windows builds on Linux during development
- **Test Script**: `test/test_wine.sh`, `test/test_wine_auto.sh`
- **Requirements**: Wine installed (`sudo apt-get install wine`)
- **Benefits**: Cross-platform compatibility verification

### Development Workflow
1. **Code**: Write cross-platform C++ code in `src/`
2. **Build**: `make PLATFORM=linux` or `make PLATFORM=windows STATIC=1`
3. **Test**: `cd test && ./test_wine_auto.sh` (Wine testing)
4. **Test**: `make test` (Linux native tests)
5. **Package**: `./create_release.sh` (creates distribution archives)
6. **Deploy**: Upload to GitHub Releases

## Project Structure
```text
selection-logger/
├── src/               # Source code
├── test/              # Test scripts and binaries
├── dist/
│   ├── linux/         # Linux distribution package
│   └── windows/       # Windows distribution package
├── Makefile           # Build system
├── create_release.sh  # Release archive generator
└── README.md          # This file
```

## License
MIT License
