@echo off
echo Selection Logger Uninstaller
echo ===========================

REM Check for administrator privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo This script requires administrator privileges.
    echo Please run as administrator.
    pause
    exit /b 1
)

REM Set installation directory
set INSTALL_DIR=C:\Program Files\SelectionLogger

echo Uninstalling from %INSTALL_DIR%

REM Stop service
echo Stopping service...
"%INSTALL_DIR%\selection-logger.exe" stop

REM Uninstall service
echo Uninstalling service...
"%INSTALL_DIR%\selection-logger.exe" uninstall

REM Remove directory
if exist "%INSTALL_DIR%" (
    rmdir /s /q "%INSTALL_DIR%"
    echo Removed installation directory.
)

echo.
echo Uninstallation complete!
pause