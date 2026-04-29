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
cp selection-logger.exe "${WINDOWS_DIR}/"
cp dist/windows/LICENSE "${WINDOWS_DIR}/"
cp dist/windows/README.md "${WINDOWS_DIR}/"
cp dist/windows/install.bat "${WINDOWS_DIR}/"
cp dist/windows/install.ps1 "${WINDOWS_DIR}/"
cp dist/windows/uninstall.bat "${WINDOWS_DIR}/"

echo "Copying Linux release files..."
cp selection-logger "${LINUX_DIR}/"
cp dist/linux/LICENSE "${LINUX_DIR}/"
cp dist/linux/README.md "${LINUX_DIR}/"
cp dist/linux/install.sh "${LINUX_DIR}/"

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
