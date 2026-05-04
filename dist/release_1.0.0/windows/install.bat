@echo off
echo Selection Logger Installer
echo =========================

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

echo Installing to %INSTALL_DIR%

REM Create directory
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

REM Copy executable
copy selection-logger.exe "%INSTALL_DIR%\"

REM Install service
echo Installing Windows service...
"%INSTALL_DIR%\selection-logger.exe" install

REM Start service
echo Starting service...
"%INSTALL_DIR%\selection-logger.exe" start

REM Add to PATH (optional)
REM setx PATH "%PATH%;%INSTALL_DIR%" /M

echo.
echo Installation complete!
echo.
echo Service commands:
echo   selection-logger.exe status  - Check service status
echo   selection-logger.exe stop    - Stop the service
echo   selection-logger.exe start   - Start the service
echo.
echo Logs are saved to: Documents\SelectionLogs\
pause