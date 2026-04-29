# Selection Logger for Linux

A lightweight daemon that monitors X11 text selections (clipboard and primary selection) and automatically logs changes to dated files.

## Quick Start

```bash
# Install dependencies (Debian/Ubuntu)
sudo apt install libx11-dev xclip

# Run manually (foreground, for testing)
chmod +x selection-logger-auto
./selection-logger-auto

# Or install as a systemd user service
sudo ./install.sh
```

## Files

| File | Description |
|---|---|
| `selection-logger` | Manual mode (DEPRECATED) |
| `selection-logger-auto` | Automatic monitoring mode (recommended) |
| `install.sh` | Systemd service installer |
| `LICENSE` | License information |

## Usage

### Automatic Monitoring (Recommended)

```bash
# Run in foreground (for testing)
./selection-logger-auto

# Install as systemd service
sudo ./install.sh

# Check service status
systemctl --user status selection-logger

# View logs
journalctl --user -u selection-logger -f
```

### Manual Mode (Legacy)

```bash
./selection-logger
```

Reads the current selection once and logs it, then waits for Enter key press to read again.

## Log Files

- Location: `~/selection_logs/` or `$XDG_DATA_HOME/selection_logs/`
- Format: `YYYY-MM.txt` (one file per month)
- Each entry: timestamped clipboard text

## Dependencies

- **libX11** — X11 clipboard access
- **pthread** — Threading
- **xclip** (optional, for some clipboard operations)

Install on Debian/Ubuntu:
```bash
sudo apt install libx11-dev xclip
```

Install on Fedora/RHEL:
```bash
sudo dnf install libX11-devel xclip
```

## Uninstall

```bash
# If installed as service
systemctl --user stop selection-logger
systemctl --user disable selection-logger
rm ~/.config/systemd/user/selection-logger.service

# Remove binaries
sudo rm /usr/local/bin/selection-logger
sudo rm /usr/local/bin/selection-logger-auto
```

## Security Notes

- **No Network Access**: The daemon does not connect to the internet.
- **No Data Collection**: All data stays on your local machine.

## License

[MIT License](LICENSE)
