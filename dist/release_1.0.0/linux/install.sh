#!/bin/bash
#
# Install selection-logger for Linux
# Installs binaries to /usr/local/bin/ and sets up systemd user service
#

set -e

BIN_DIR="/usr/local/bin"
SERVICE_NAME="selection-logger"
SERVICE_DIR="${HOME}/.config/systemd/user"

echo "Selection Logger Installer"
echo "=========================="

# Check for root
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root (sudo)"
    exit 1
fi

# Install binary
echo "Installing binary to ${BIN_DIR}..."
cp selection-logger "${BIN_DIR}/"
chmod 755 "${BIN_DIR}/selection-logger"

echo "Binaries installed successfully."

# Setup systemd user service
echo "Setting up systemd service..."
mkdir -p "${SERVICE_DIR}"

cat > "${SERVICE_DIR}/${SERVICE_NAME}.service" << 'SERVICE_EOF'
[Unit]
Description=Selection Logger - clipboard monitoring daemon
After=graphical-session.target

[Service]
Type=simple
ExecStart=/usr/local/bin/selection-logger
Restart=on-failure
RestartSec=5

[Install]
WantedBy=default.target
SERVICE_EOF

chmod 644 "${SERVICE_DIR}/${SERVICE_NAME}.service"

echo "Service file created at ${SERVICE_DIR}/${SERVICE_NAME}.service"

# Reload systemd and enable
echo "Enabling and starting service..."
sudo -u "${SUDO_USER}" systemctl --user daemon-reload
sudo -u "${SUDO_USER}" systemctl --user enable "${SERVICE_NAME}"
sudo -u "${SUDO_USER}" systemctl --user start "${SERVICE_NAME}"

echo ""
echo "Installation complete!"
echo "Service '${SERVICE_NAME}' is now running."
echo ""
echo "Useful commands:"
echo "  systemctl --user status ${SERVICE_NAME}"
echo "  journalctl --user -u ${SERVICE_NAME} -f"
echo ""
echo "Log files are saved to: ~/selection_logs/"
