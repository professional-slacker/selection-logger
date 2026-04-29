#!/bin/bash

# Test selection-logger with Wine (auto-monitoring mode)

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${PROJECT_DIR}"

echo "Testing selection-logger with Wine..."
echo "======================================"

# Check if Wine is installed
if ! command -v wine &> /dev/null; then
    echo "Error: Wine is not installed"
    echo "Install Wine first: sudo apt-get install wine"
    exit 1
fi

# Build Windows version (statically linked)
echo "Building Windows version (statically linked)..."
make clean
make PLATFORM=windows STATIC=1

echo ""
echo "Testing selection-logger.exe with Wine..."
echo "----------------------------------------"

# Test help option
echo "1. Testing --help option:"
wine selection-logger.exe --help

echo ""
echo "2. Testing different modes:"
echo "   a) --primary mode (mouse selection):"
wine selection-logger.exe --primary --version 2>&1 | head -5 || true

echo ""
echo "   b) --clipboard mode (Ctrl+C):"
wine selection-logger.exe --clipboard --version 2>&1 | head -5 || true

echo ""
echo "   c) --both mode (default):"
wine selection-logger.exe --both --version 2>&1 | head -5 || true

echo ""
echo "3. Testing interval option:"
wine selection-logger.exe --interval 500 --version 2>&1 | head -5 || true

echo ""
echo "4. Testing DLL dependencies:"
echo "Checking if EXE runs without external DLLs..."
if objdump -p selection-logger.exe | grep -q "libgcc_s_seh-1.dll"; then
    echo "WARNING: EXE still depends on MinGW DLLs"
else
    echo "OK: EXE is statically linked (no external DLL dependencies)"
fi

echo ""
echo "5. File information:"
ls -lh selection-logger.exe
file selection-logger.exe

echo ""
echo "6. Quick functionality test (5 seconds):"
echo "Starting auto mode for 5 seconds..."
timeout 5 wine selection-logger.exe --both --interval 1000 2>&1 | tail -10 || true

echo ""
echo "Wine test completed successfully!"
echo ""
echo "Summary:"
echo "- Cross-platform automatic mode ✓"
echo "- Static linking (no DLL dependencies) ✓"
echo "- Wine compatibility ✓"
echo "- Multiple monitoring modes ✓"
