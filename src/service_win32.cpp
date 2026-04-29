#ifdef _WIN32

#include "service_win32.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

// Global service status
ServiceStatus g_serviceStatus;

// Service entry point
void WINAPI ServiceMain(DWORD argc, LPWSTR* argv) {
    // Register the service control handler
    g_serviceStatus.statusHandle = RegisterServiceCtrlHandlerW(SERVICE_NAME, ServiceCtrlHandler);
    if (!g_serviceStatus.statusHandle) {
        return;
    }

    // Initialize service status
    g_serviceStatus.status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_serviceStatus.status.dwCurrentState = SERVICE_START_PENDING;
    g_serviceStatus.status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    g_serviceStatus.status.dwWin32ExitCode = NO_ERROR;
    g_serviceStatus.status.dwServiceSpecificExitCode = 0;
    g_serviceStatus.status.dwCheckPoint = 0;
    g_serviceStatus.status.dwWaitHint = 3000; // 3 seconds

    // Create stop event
    g_serviceStatus.stopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_serviceStatus.stopEvent) {
        g_serviceStatus.status.dwCurrentState = SERVICE_STOPPED;
        g_serviceStatus.status.dwWin32ExitCode = GetLastError();
        SetServiceStatus(g_serviceStatus.statusHandle, &g_serviceStatus.status);
        return;
    }

    // Report running status
    g_serviceStatus.status.dwCurrentState = SERVICE_RUNNING;
    g_serviceStatus.status.dwCheckPoint = 0;
    g_serviceStatus.status.dwWaitHint = 0;
    SetServiceStatus(g_serviceStatus.statusHandle, &g_serviceStatus.status);

    g_serviceStatus.isRunning = true;

    // Main service loop
    while (g_serviceStatus.isRunning) {
        // Check for stop event
        if (WaitForSingleObject(g_serviceStatus.stopEvent, 1000) == WAIT_OBJECT_0) {
            break;
        }

        // Log clipboard content
        LogClipboardToFile();
    }

    // Cleanup
    CloseHandle(g_serviceStatus.stopEvent);

    // Report stopped status
    g_serviceStatus.status.dwCurrentState = SERVICE_STOPPED;
    g_serviceStatus.status.dwCheckPoint = 0;
    g_serviceStatus.status.dwWaitHint = 0;
    SetServiceStatus(g_serviceStatus.statusHandle, &g_serviceStatus.status);
}

// Service control handler
void WINAPI ServiceCtrlHandler(DWORD ctrlCode) {
    switch (ctrlCode) {
        case SERVICE_CONTROL_STOP:
        case SERVICE_CONTROL_SHUTDOWN:
            g_serviceStatus.status.dwCurrentState = SERVICE_STOP_PENDING;
            g_serviceStatus.status.dwCheckPoint = 1;
            g_serviceStatus.status.dwWaitHint = 3000;
            SetServiceStatus(g_serviceStatus.statusHandle, &g_serviceStatus.status);

            g_serviceStatus.isRunning = false;
            SetEvent(g_serviceStatus.stopEvent);
            break;

        case SERVICE_CONTROL_INTERROGATE:
            // Fall through to send current status
            break;

        default:
            break;
    }

    // Send current status
    SetServiceStatus(g_serviceStatus.statusHandle, &g_serviceStatus.status);
}

// Install the service
bool InstallService() {
    SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!scm) {
        std::wcerr << L"Failed to open Service Control Manager: " << GetLastError() << std::endl;
        return false;
    }

    // Get current executable path
    wchar_t path[MAX_PATH];
    if (!GetModuleFileNameW(NULL, path, MAX_PATH)) {
        std::wcerr << L"Failed to get module filename: " << GetLastError() << std::endl;
        CloseServiceHandle(scm);
        return false;
    }

    // Create the service
    SC_HANDLE service = CreateServiceW(
        scm,
        SERVICE_NAME,
        SERVICE_DISPLAY_NAME,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,  // Start automatically
        SERVICE_ERROR_NORMAL,
        path,
        NULL, NULL, NULL, NULL, NULL
    );

    if (!service) {
        DWORD error = GetLastError();
        if (error == ERROR_SERVICE_EXISTS) {
            std::wcout << L"Service already exists." << std::endl;
            CloseServiceHandle(scm);
            return true;
        }
        std::wcerr << L"Failed to create service: " << error << std::endl;
        CloseServiceHandle(scm);
        return false;
    }

    // Set service description (commented out for MinGW compatibility)
    // SERVICE_DESCRIPTION serviceDesc = { L"Logs system clipboard selections to file" };
    // ChangeServiceConfig2W(service, SERVICE_CONFIG_DESCRIPTION, &serviceDesc);

    std::wcout << L"Service installed successfully." << std::endl;

    CloseServiceHandle(service);
    CloseServiceHandle(scm);
    return true;
}

// Uninstall the service
bool UninstallService() {
    SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) {
        std::wcerr << L"Failed to open Service Control Manager: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE service = OpenServiceW(scm, SERVICE_NAME, DELETE);
    if (!service) {
        std::wcerr << L"Failed to open service: " << GetLastError() << std::endl;
        CloseServiceHandle(scm);
        return false;
    }

    // Stop the service first
    SERVICE_STATUS status;
    ControlService(service, SERVICE_CONTROL_STOP, &status);
    Sleep(1000); // Wait for service to stop

    // Delete the service
    if (!DeleteService(service)) {
        DWORD error = GetLastError();
        if (error != ERROR_SERVICE_MARKED_FOR_DELETE) {
            std::wcerr << L"Failed to delete service: " << error << std::endl;
            CloseServiceHandle(service);
            CloseServiceHandle(scm);
            return false;
        }
    }

    std::wcout << L"Service uninstalled successfully." << std::endl;

    CloseServiceHandle(service);
    CloseServiceHandle(scm);
    return true;
}

// Get clipboard text using Win32 API
std::string GetClipboardTextWin32() {
    if (!OpenClipboard(nullptr)) {
        return "";
    }

    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr) {
        CloseClipboard();
        return "";
    }

    char* text = static_cast<char*>(GlobalLock(hData));
    if (text == nullptr) {
        CloseClipboard();
        return "";
    }

    std::string result(text);

    GlobalUnlock(hData);
    CloseClipboard();

    return result;
}

// Get log file path with current date
std::wstring GetLogFilePath() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &time);

    std::wstringstream ss;
    ss << L"selection_log_"
       << (tm.tm_year + 1900) << L"-"
       << std::setw(2) << std::setfill(L'0') << (tm.tm_mon + 1) << L"-"
       << std::setw(2) << std::setfill(L'0') << tm.tm_mday
       << L".txt";

    return ss.str();
}

// Log clipboard content to file
void LogClipboardToFile() {
    std::string text = GetClipboardTextWin32();
    if (text.empty()) {
        return;
    }

    std::wstring logFileW = GetLogFilePath();
    // Convert wstring to string for MinGW compatibility
    std::string logFile(logFileW.begin(), logFileW.end());
    std::ofstream file(logFile.c_str(), std::ios::app);
    if (!file.is_open()) {
        return;
    }

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &time);

    file << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " - " << text << std::endl;
    file.close();
}

// Console mode main function (for testing)
int ConsoleMain(int argc, char* argv[]) {
    std::cout << "Selection Logger running in console mode..." << std::endl;
    std::cout << "Press Ctrl+C to save clipboard, Ctrl+Q to exit." << std::endl;

    // Set console mode to handle Ctrl+C
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & ~ENABLE_LINE_INPUT);

    while (true) {
        // Check for Ctrl+C (0x03) or Ctrl+Q (0x11)
        INPUT_RECORD ir;
        DWORD count;

        if (PeekConsoleInput(hStdin, &ir, 1, &count) && count > 0) {
            ReadConsoleInput(hStdin, &ir, 1, &count);

            if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
                WORD keyCode = ir.Event.KeyEvent.wVirtualKeyCode;
                bool ctrlPressed = (ir.Event.KeyEvent.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;

                if (ctrlPressed && keyCode == 'C') {
                    // Ctrl+C: Save clipboard
                    std::cout << "Saving clipboard..." << std::endl;
                    LogClipboardToFile();
                }
                else if (ctrlPressed && keyCode == 'Q') {
                    // Ctrl+Q: Exit
                    std::cout << "Exiting..." << std::endl;
                    break;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}

#endif // _WIN32