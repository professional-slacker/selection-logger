```markdown
# selection-logger

A lightweight cross-platform daemon that automatically captures system text selections (clipboard) and saves them to dated log files.

## ✨ Features
- **Automatic Capture**: Saves text selections automatically when they change (no manual trigger needed)
- **Dual Selection Monitoring**: Monitors both PRIMARY (mouse selection) and CLIPBOARD (Ctrl+C) selections
- **Smart Organization**: Automatically categorizes logs into `~/memories/Year/Month.txt`
- **Low Overhead**: A minimal background process designed for cross-platform environments
- **Timestamped**: Every entry is recorded with a precise timestamp for later reference
- **Configurable**: Multiple monitoring modes and adjustable polling interval

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
Clone this repository and compile the source code using `g++`.

```bash
g++ -o selection-logger-auto memory_daemon_auto.cpp -lX11
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

### How It Works
1. Uses platform-specific tools to access system selections (xclip on Linux/X11)
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

## 📄 Legacy Version
The original manual mode version (`memory_daemon.cpp`) with Ctrl+M trigger is still available but deprecated in favor of the automatic version.

## License
MIT License
```
