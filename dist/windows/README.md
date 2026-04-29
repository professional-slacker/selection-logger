# Selection Logger for Windows

A lightweight daemon that monitors clipboard text selections and automatically logs changes to dated files.

## Quick Start

1. **Download** `selection-logger.exe` from [Releases](https://github.com/professional-slacker/selection-logger/releases)
2. **Run as Administrator** (Command Prompt or PowerShell):
   ```cmd
   mkdir "C:\Program Files\SelectionLogger"
   copy selection-logger.exe "C:\Program Files\SelectionLogger\"
   cd "C:\Program Files\SelectionLogger"
   selection-logger.exe install
   selection-logger.exe start
   ```

The daemon will now run as a Windows service, logging clipboard selections to `Documents\SelectionLogs\`.

## Installation Methods

### Option 1: Batch File (Easiest)
- Right-click `install.bat` → **Run as administrator**

### Option 2: PowerShell
```powershell
# Run PowerShell as Administrator
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\install.ps1
```

### Option 3: Manual (Recommended)
1. Create the directory and copy the executable
2. Run as administrator:
   ```cmd
   selection-logger.exe install
   selection-logger.exe start
   ```

## Uninstall

### Batch File
- Right-click `uninstall.bat` → **Run as administrator**

### Manual
```cmd
cd "C:\Program Files\SelectionLogger"
selection-logger.exe stop
selection-logger.exe uninstall
cd \
rmdir /s "C:\Program Files\SelectionLogger"
```

## Commands

```cmd
selection-logger.exe install     Install the Windows service
selection-logger.exe uninstall   Uninstall the service
selection-logger.exe start       Start the service
selection-logger.exe stop        Stop the service
selection-logger.exe status      Check service status
selection-logger.exe run         Run in console mode (testing)
selection-logger.exe --help      Show help
```

## Log Files

- Location: `C:\Users\<username>\Documents\SelectionLogs\`
- Format: `YYYY-MM.txt` (one file per month)
- Each entry: timestamped clipboard text

## Security Notes

- **SmartScreen Warning**: This software is not code-signed. If Windows displays a warning, click **More info** → **Run anyway**, or right-click the file → **Properties** → check **Unblock**.
- **Admin Rights Required**: Service installation needs administrator privileges.
- **No Network Access**: The daemon does not connect to the internet.
- **No Data Collection**: All data stays on your local machine.

## Troubleshooting

### Service won't start:
```cmd
cd "C:\Program Files\SelectionLogger"
selection-logger.exe status
selection-logger.exe stop
selection-logger.exe start
```

### Verify file integrity:
```powershell
Get-FileHash -Algorithm SHA256 selection-logger.exe
```

## License

[MIT License](LICENSE)
