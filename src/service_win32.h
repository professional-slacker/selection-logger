#ifndef SERVICE_WIN32_H
#define SERVICE_WIN32_H

#ifdef _WIN32

#include <windows.h>
#include <string>

// Service configuration
#define SERVICE_NAME           L"SelectionLogger"
#define SERVICE_DISPLAY_NAME   L"Selection Logger"
#define SERVICE_DESCRIPTION    L"Logs system clipboard selections to file"

// Service status structure
struct ServiceStatus {
    SERVICE_STATUS status;
    SERVICE_STATUS_HANDLE statusHandle;
    HANDLE stopEvent;
    bool isRunning;
};

// Service control functions
void WINAPI ServiceMain(DWORD argc, LPWSTR* argv);
void WINAPI ServiceCtrlHandler(DWORD ctrlCode);

// Service management functions
bool InstallService();
bool UninstallService();
bool StartService();
bool StopService();
bool QueryServiceStatus();

// Service worker functions
void ServiceWorkerThread();
void LogClipboardToFile();

// Console mode main function (for testing)
int ConsoleMain(int argc, char* argv[]);

// Utility functions
std::wstring GetLogFilePath();
std::string GetClipboardTextWin32();

#endif // _WIN32

#endif // SERVICE_WIN32_H