#!/bin/bash

# Create release packages for Windows and Linux
# Usage: ./create_release.sh [version]

set -e

VERSION=${1:-1.0.0}
RELEASE_DIR="dist/release_${VERSION}"
WINDOWS_DIR="${RELEASE_DIR}/windows"
LINUX_DIR="${RELEASE_DIR}/linux"

echo "Creating release packages for version ${VERSION}"

# Clean and create directories
rm -rf "${RELEASE_DIR}"
mkdir -p "${WINDOWS_DIR}" "${LINUX_DIR}"

echo "Building Windows release (statically linked)..."
make clean
make PLATFORM=windows STATIC=1

echo "Building Linux release..."
make clean
make PLATFORM=linux

echo "Copying Windows release files..."
cp selection-logger.exe selection-logger-auto.exe "${WINDOWS_DIR}/"
cp LICENSE.txt "${WINDOWS_DIR}/"
cp README.md "${WINDOWS_DIR}/"

# Create Windows README
cat > "${WINDOWS_DIR}/README_WINDOWS.txt" << 'EOF'
Selection Logger for Windows
============================

Version: ${VERSION}

Files included:
- selection-logger.exe      - Main selection logger
- selection-logger-auto.exe - Automatic monitoring version
- LICENSE.txt              - License information
- README.md                - General documentation

Usage:
1. Run selection-logger.exe to log clipboard/selection manually
2. Run selection-logger-auto.exe for automatic monitoring

Features:
- Cross-platform clipboard logging
- Automatic monitoring option
- Monthly log file organization

Requirements:
- Windows 7 or later
- No additional DLLs required (statically linked)

Notes:
- Logs are saved to: %USERPROFILE%/selection_logs/
- Each month creates a new log file (YYYY/MM.txt)
EOF

echo "Copying Linux release files..."
cp selection-logger selection-logger-auto "${LINUX_DIR}/"
cp LICENSE.txt "${LINUX_DIR}/"
cp README.md "${LINUX_DIR}/"

# Create Linux README
cat > "${LINUX_DIR}/README_LINUX.txt" << 'EOF'
Selection Logger for Linux
==========================

Version: ${VERSION}

Files included:
- selection-logger      - Main selection logger
- selection-logger-auto - Automatic monitoring version
- LICENSE.txt          - License information
- README.md            - General documentation

Dependencies:
- X11 development libraries
- xclip command-line tool
- pthread library

Install dependencies on Ubuntu/Debian:
  sudo apt-get install libx11-dev xclip

Install dependencies on Fedora/RHEL:
  sudo dnf install libX11-devel xclip

Usage:
1. Run ./selection-logger to log clipboard/selection manually
2. Run ./selection-logger-auto for automatic monitoring

Features:
- Cross-platform clipboard logging
- Automatic monitoring option
- Monthly log file organization

Notes:
- Logs are saved to: ~/selection_logs/
- Each month creates a new log file (YYYY/MM.txt)
- Requires X11 session for clipboard access
EOF

echo "Creating archives..."
cd "${RELEASE_DIR}"

# Windows archive
zip -r "selection-logger-windows-${VERSION}.zip" windows/

# Linux archive
tar -czf "selection-logger-linux-${VERSION}.tar.gz" linux/

echo "Release packages created in ${RELEASE_DIR}:"
echo "  - selection-logger-windows-${VERSION}.zip"
echo "  - selection-logger-linux-${VERSION}.tar.gz"

# Create checksums
sha256sum "selection-logger-windows-${VERSION}.zip" "selection-logger-linux-${VERSION}.tar.gz" > SHA256SUMS.txt

echo ""
echo "Checksums:"
cat SHA256SUMS.txt