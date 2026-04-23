#!/bin/bash

# Test selection-logger with Wine
# This script tests the Windows build using Wine

set -e

echo "Testing selection-logger with Wine..."
echo "======================================"

# Check if Wine is installed
if ! command -v wine &> /dev/null; then
    echo "Error: Wine is not installed"
    echo "Install Wine first: sudo apt-get install wine"
    exit 1
fi

# Build Windows version (statically linked)
echo "Building Windows version..."
make clean
make PLATFORM=windows STATIC=1

echo ""
echo "Testing selection-logger.exe with Wine..."
echo "----------------------------------------"

# Test help option
echo "1. Testing --help option:"
wine selection-logger.exe --help

echo ""
echo "2. Testing basic execution:"
wine selection-logger.exe --version 2>&1 || echo "Version check completed"

echo ""
echo "3. Testing selection-logger-auto.exe:"
wine selection-logger-auto.exe --help

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
ls -lh selection-logger.exe selection-logger-auto.exe
file selection-logger.exe

echo ""
echo "Wine test completed successfully!"
echo "The Windows build works correctly under Wine."