# Development Guide

## Project Structure

```
selection-logger/
├── Makefile                    # Cross-platform build system
├── README.md                   # Main documentation
├── QUICK_START.md              # Quick start guide
├── DEVELOPMENT.md              # This file
├── create_release.sh           # Release packaging script
├── test_wine_auto.sh           # Wine testing script
├── test_wine.sh               # Legacy Wine test script
│
├── *.cpp, *.h                  # Source files
│   ├── memory_daemon.cpp      # DEPRECATED: Manual mode (Linux)
│   ├── memory_daemon_auto.cpp # Auto mode (Linux)
│   ├── memory_daemon_cross.cpp # Cross-platform auto mode
│   ├── memory_daemon_auto_cross.cpp # Cross-platform auto mode
│   ├── clipboard.cpp/h        # Platform-specific clipboard
│   ├── platform.cpp/h         # Platform detection
│   ├── win32_compat.cpp/h     # Win32 compatibility layer
│   ├── service_win32.cpp/h    # Windows service implementation
│   └── main_win32.cpp         # Windows main entry point
│
├── dist/                       # Distribution files
│   ├── windows/               # Windows distribution
│   └── windows_complete/      # Complete Windows package
│
└── install_*.sh/ps1/bat       # Installation scripts
```

## Build System

### Makefile Targets
```bash
# Default build (detects platform)
make

# Linux build
make PLATFORM=linux

# Windows build (dynamic linking)
make PLATFORM=windows

# Windows build (static linking, no DLLs)
make PLATFORM=windows STATIC=1

# Release build (static linking for Windows)
make release PLATFORM=windows

# Clean build artifacts
make clean
```

### Platform Detection
- **Linux**: Uses X11 clipboard via xclip
- **Windows**: Uses Win32 API (OpenClipboard, GetClipboardData)
- **Wine**: Windows executables tested on Linux

## Cross-platform Architecture

### Clipboard Abstraction Layer
```cpp
// clipboard.h
class Clipboard {
public:
    virtual std::string get_text(const std::string& selection_type) = 0;
    virtual bool set_text(const std::string& text, const std::string& selection_type) = 0;
};

// Linux implementation
class X11Clipboard : public Clipboard {
    // Uses xclip command-line tool
};

// Windows implementation  
class WindowsClipboard : public Clipboard {
    // Uses Win32 API
};
```

### Platform Detection
```cpp
// platform.h
std::string get_platform();
bool is_windows();
bool is_linux();
```

## Development Workflow

### 1. Code Development
```bash
# Edit source files
vim memory_daemon_cross.cpp
```

### 2. Build and Test (Linux)
```bash
make clean && make PLATFORM=linux
./selection-logger-auto --both --interval 1000
```

### 3. Build and Test (Windows via Wine)
```bash
make clean && make PLATFORM=windows STATIC=1
./test_wine_auto.sh
```

### 4. Native Testing
- **Linux**: Run directly
- **Windows**: Copy EXE to Windows machine

## Testing Strategy

### Unit Tests
```bash
# Test clipboard functionality
./test_clipboard_formats

# Test selection monitoring
./test_selection

# Test file operations
./test_file_copy
```

### Integration Tests
```bash
# Wine testing (Windows compatibility)
./test_wine_auto.sh

# Cross-platform consistency
# 1. Build both platforms
# 2. Run same test scenarios
# 3. Compare log outputs
```

### Manual Testing
1. Copy text with Ctrl+C
2. Select text with mouse
3. Check log file: `~/memories/2026/04.txt`
4. Verify timestamps and content

## Release Process

### 1. Version Bumping
```bash
# Update version in create_release.sh
VERSION=${1:-1.0.0}
```

### 2. Build Release Packages
```bash
./create_release.sh 1.1.0
```

### 3. Test Release Packages
```bash
# Test Windows package
unzip dist/release_1.1.0/selection-logger-windows-1.1.0.zip
cd windows
wine selection-logger-auto.exe --help

# Test Linux package
tar -xzf dist/release_1.1.0/selection-logger-linux-1.1.0.tar.gz
cd linux
./selection-logger-auto --help
```

### 4. Create GitHub Release
1. Tag commit: `git tag v1.1.0`
2. Push tag: `git push origin v1.1.0`
3. Upload release packages
4. Update release notes

## Code Guidelines

### C++ Standards
- C++11 minimum
- Platform-specific code in separate files
- Use abstraction layers for platform differences

### File Organization
- Platform-specific code: `*_win32.*`, `*_x11.*`
- Cross-platform code: `*_cross.*`
- Headers: `.h` extension
- Sources: `.cpp` extension

### Error Handling
```cpp
try {
    // Platform-specific operations
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
```

## Debugging

### Linux Debugging
```bash
# Build with debug symbols
make clean && make PLATFORM=linux CXXFLAGS="-g -O0"

# Run with gdb
gdb --args ./selection-logger-auto --both

# Check system dependencies
ldd ./selection-logger-auto
```

### Windows Debugging (via Wine)
```bash
# Build with debug symbols
make clean && make PLATFORM=windows STATIC=1 CXXFLAGS="-g -O0"

# Run with Wine debug output
WINEDEBUG=+seh wine selection-logger-auto.exe --both

# Check DLL dependencies (should be none with static linking)
objdump -p selection-logger-auto.exe | grep "DLL Name:"
```

### Log Analysis
```bash
# Monitor log file in real-time
tail -f ~/memories/2026/04.txt

# Check log directory structure
ls -la ~/memories/
ls -la ~/memories/2026/
```

## Contributing

### 1. Fork Repository
```bash
git clone https://github.com/yourusername/selection-logger.git
cd selection-logger
```

### 2. Create Feature Branch
```bash
git checkout -b feature/auto-mode-enhancements
```

### 3. Make Changes
```bash
# Implement feature
# Add tests
# Update documentation
```

### 4. Test Changes
```bash
# Test on Linux
make clean && make PLATFORM=linux
./selection-logger-auto --both

# Test on Windows (via Wine)
make clean && make PLATFORM=windows STATIC=1
./test_wine_auto.sh
```

### 5. Submit Pull Request
1. Commit changes
2. Push to fork
3. Create PR on GitHub

## Future Enhancements

### Planned Features
1. macOS support (NSPasteboard)
2. GUI configuration interface
3. Cloud sync integration
4. Advanced filtering rules
5. Searchable log database

### Technical Debt
1. Remove deprecated manual mode
2. Refactor platform detection
3. Improve error recovery
4. Add more unit tests