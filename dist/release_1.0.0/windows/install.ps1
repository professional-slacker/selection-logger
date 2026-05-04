# Selection Logger PowerShell Installer
# Run as Administrator

param(
    [string]$InstallPath = "C:\Program Files\SelectionLogger"
)

# Check for admin rights
$currentPrincipal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Host "This script requires administrator privileges." -ForegroundColor Red
    Write-Host "Please run PowerShell as Administrator." -ForegroundColor Yellow
    pause
    exit 1
}

Write-Host "Selection Logger Installer" -ForegroundColor Green
Write-Host "=========================" -ForegroundColor Green

# Create installation directory
if (-not (Test-Path $InstallPath)) {
    New-Item -ItemType Directory -Path $InstallPath -Force | Out-Null
    Write-Host "Created installation directory: $InstallPath" -ForegroundColor Cyan
}

# Copy executable
Copy-Item "selection-logger.exe" -Destination $InstallPath -Force
Write-Host "Copied executable to $InstallPath" -ForegroundColor Cyan

# Install service
Write-Host "Installing Windows service..." -ForegroundColor Yellow
Start-Process -FilePath "$InstallPath\selection-logger.exe" -ArgumentList "install" -Wait -NoNewWindow

# Start service
Write-Host "Starting service..." -ForegroundColor Yellow
Start-Process -FilePath "$InstallPath\selection-logger.exe" -ArgumentList "start" -Wait -NoNewWindow

# Optional: Add to PATH
# $envPath = [Environment]::GetEnvironmentVariable("PATH", "Machine")
# if ($envPath -notlike "*$InstallPath*") {
#     [Environment]::SetEnvironmentVariable("PATH", "$envPath;$InstallPath", "Machine")
#     Write-Host "Added to system PATH" -ForegroundColor Cyan
# }

Write-Host ""
Write-Host "Installation complete!" -ForegroundColor Green
Write-Host ""
Write-Host "Service commands:" -ForegroundColor Yellow
Write-Host "  selection-logger.exe status  - Check service status"
Write-Host "  selection-logger.exe stop    - Stop the service"
Write-Host "  selection-logger.exe start   - Start the service"
Write-Host ""
Write-Host "Logs are saved to: Documents\SelectionLogs\" -ForegroundColor Cyan
Write-Host ""
pause