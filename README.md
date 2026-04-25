```markdown
# selection-logger

A lightweight cross-platform daemon that monitors system text selections and automatically logs changes to dated files.

## ✨ Features
- **Cross-platform**: Windows and Linux (Wine test compatible)
- **Automatic Capture Only**: Saves text selections automatically when they change
- **Dual Selection Monitoring**: Monitors both PRIMARY (mouse selection) and CLIPBOARD (Ctrl+C) selections
- **Smart Organization**: Automatically categorizes logs into `~/memories/Year/Month.txt`
- **Low Overhead**: A minimal background process designed for cross-platform environments
- **Timestamped**: Every entry is recorded with a precise timestamp for later reference
- **Configurable**: Multiple monitoring modes and adjustable polling interval
- **Static Linking**: No external DLL dependencies (Windows)

## 📋 Prerequisites

### Linux/X11
You need the X11 development libraries and `xclip` installed on your system.
```bash
sudo apt update
sudo apt install libx11-dev xclip
```

### Windows
Coming soon...

### macOS
Coming soon...

## 🛠 Installation & Build

### Cross-platform Build System
This project uses a Makefile for cross-platform compilation supporting Linux, Windows, and Wine testing.

```bash
# Clone the repository
git clone https://github.com/yourusername/selection-logger.git
cd selection-logger

# Build for Linux (X11)
make clean && make PLATFORM=linux

# Build for Windows (statically linked, no DLL dependencies)
make clean && make PLATFORM=windows STATIC=1

# Build for release (statically linked Windows, optimized)
make clean && make release PLATFORM=windows
```

## 🚀 Usage

### Basic Usage
```bash
./selection-logger-auto
```
Starts in hybrid mode (monitors both PRIMARY and CLIPBOARD selections).

### Command Line Options
```bash
./selection-logger-auto [options]
```

**Options:**
- `-p, --primary`        Monitor PRIMARY selection only (mouse selection)
- `-c, --clipboard`      Monitor CLIPBOARD selection only (Ctrl+C)
- `-b, --both`           Monitor both PRIMARY and CLIPBOARD (default)
- `-i N, --interval N`   Polling interval in milliseconds (default: 1000)
- `-h, --help`           Show this help message

**Modes:**
- **PRIMARY**: Saves text when selected with mouse (no Ctrl+C needed)
- **CLIPBOARD**: Saves text when copied with Ctrl+C (works with Ctrl+A → Ctrl+C workflow)
- **BOTH**: Monitors both selections (recommended for comprehensive logging)

### Examples
```bash
# Monitor mouse selections only
./selection-logger-auto --primary

# Monitor Ctrl+C copies only (useful for Ctrl+A → Ctrl+C workflow)
./selection-logger-auto --clipboard

# Monitor both with faster polling (500ms)
./selection-logger-auto --both --interval 500

# Run in background
./selection-logger-auto --both &
```

## 📂 Directory Structure
```text
/home/USER/memories/
└── 2026/
    ├── 03.txt  <-- Log entries are appended here
    └── 04.txt
```

## 📝 Log Format
Entries are saved in the following format:
```text
--- Mon Mar 30 12:59:50 2026 ---
Your captured text selection appears here.

--- Mon Mar 30 13:05:22 2026 ---
file:///home/user/documents/report.pdf

--- Mon Mar 30 13:10:15 2026 ---
Another important text snippet.
```

**Note:** File copies (Ctrl+C on files) are also logged as file URIs.

## 🔧 Technical Details

### Cross-platform Architecture
- **Linux/X11**: Uses xclip command-line tool for clipboard access
- **Windows**: Uses Win32 API (OpenClipboard, GetClipboardData)
- **Wine Testing**: Windows executables can be tested on Linux using Wine
- **Static Linking**: Windows builds are statically linked (no external DLL dependencies)

### How It Works
1. Uses platform-specific tools to access system selections
2. Polls selections at configurable intervals (default: 1 second)
3. Detects changes by comparing with previous selection content
4. Skips empty or whitespace-only selections
5. Creates log directories automatically if they don't exist

### Selection Types
- **PRIMARY Selection**: Updated when text is selected with mouse (middle-click paste on X11)
- **CLIPBOARD Selection**: Updated with Ctrl+C (standard copy/paste)
- **Note**: Some applications may not update PRIMARY selection with Ctrl+A

## 💡 Pro Tips

1. **Startup Automation**: Add to your `.xinitrc` or desktop environment's "Startup Applications":
   ```bash
   /path/to/selection-logger-auto --both &
   ```

2. **Recommended Mode**: Use `--both` mode for comprehensive logging of all text selections.

3. **Performance**: Default 1000ms interval provides good balance between responsiveness and CPU usage.

4. **File Operations**: File copies are logged as file URIs (e.g., `file:///path/to/file.txt`).

## 🌐 Platform Support

### Linux (X11)
- **Dependencies**: `libx11-dev`, `xclip`
- **Install**: `sudo apt-get install libx11-dev xclip` (Ubuntu/Debian)
- **Build**: `make PLATFORM=linux`
- **Run**: `./selection-logger-auto [options]`

### Windows
- **Build**: `make PLATFORM=windows STATIC=1`
- **Output**: `selection-logger-auto.exe` (statically linked)
- **Run**: `selection-logger-auto.exe [options]`
- **Features**: No external DLL dependencies, Win32 API

### Wine Testing Environment
- **Purpose**: Test Windows builds on Linux during development
- **Test Script**: `./test_wine_auto.sh`
- **Requirements**: Wine installed (`sudo apt-get install wine`)
- **Benefits**: Cross-platform compatibility verification

### Development Workflow
1. **Code**: Write cross-platform C++ code
2. **Build**: `make PLATFORM=windows STATIC=1`
3. **Test**: `./test_wine_auto.sh` (Wine testing)
4. **Deploy**: Package for Windows and Linux

## 📄 Legacy Version (DEPRECATED)
The original manual mode version (`memory_daemon.cpp`) with Ctrl+M trigger is deprecated and will be removed in a future version. Use `selection-logger-auto` for automatic monitoring instead.

## License
MIT License
```
