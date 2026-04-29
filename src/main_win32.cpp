#ifdef _WIN32

#include "service_win32.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// Service dispatch table
SERVICE_TABLE_ENTRYW ServiceTable[] = {
    { const_cast<LPWSTR>(SERVICE_NAME), ServiceMain },
    { NULL, NULL }
};

void PrintUsage() {
    std::wcout << L"Selection Logger - Cross-platform clipboard logger" << std::endl;
    std::wcout << L"Usage:" << std::endl;
    std::wcout << L"  selection-logger.exe install     - Install as Windows service" << std::endl;
    std::wcout << L"  selection-logger.exe uninstall   - Uninstall Windows service" << std::endl;
    std::wcout << L"  selection-logger.exe start       - Start the service" << std::endl;
    std::wcout << L"  selection-logger.exe stop        - Stop the service" << std::endl;
    std::wcout << L"  selection-logger.exe status      - Check service status" << std::endl;
    std::wcout << L"  selection-logger.exe run         - Run in console mode (manual)" << std::endl;
    std::wcout << L"  selection-logger.exe manual      - Manual mode: Ctrl+C to save, Ctrl+Q to exit" << std::endl;
    std::wcout << L"  selection-logger.exe auto        - Auto mode: continuous monitoring every second" << std::endl;
    std::wcout << L"  selection-logger.exe             - Run as service (called by SCM)" << std::endl;
}

int wmain(int argc, wchar_t* argv[]) {
    // Handle --help as first argument
    if (argc > 1) {
        std::wstring arg = argv[1];
        if (arg == L"--help" || arg == L"-h" || arg == L"/?") {
            PrintUsage();
            return 0;
        }
    }

    // Check command line arguments
    if (argc > 1) {
        std::wstring command = argv[1];

        if (command == L"install") {
            return InstallService() ? 0 : 1;
        }
        else if (command == L"uninstall") {
            return UninstallService() ? 0 : 1;
        }
        else if (command == L"start") {
            return StartService() ? 0 : 1;
        }
        else if (command == L"stop") {
            return StopService() ? 0 : 1;
        }
        else if (command == L"status") {
            return QueryServiceStatus() ? 0 : 1;
        }
        else if (command == L"run" || command == L"manual") {
            // Convert wchar_t** to char** for console main
            char** charArgs = new char*[argc];
            for (int i = 0; i < argc; i++) {
                int size = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, NULL, 0, NULL, NULL);
                charArgs[i] = new char[size];
                WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, charArgs[i], size, NULL, NULL);
            }

            int result = ConsoleMain(argc, charArgs);

            // Cleanup
            for (int i = 0; i < argc; i++) {
                delete[] charArgs[i];
            }
            delete[] charArgs;

            return result;
        }
        else if (command == L"auto") {
            // Auto mode: continuous monitoring
            std::wcout << L"Auto mode: monitoring clipboard every second..." << std::endl;
            std::wcout << L"Press Ctrl+C to exit." << std::endl;

            while (true) {
                LogClipboardToFile();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            return 0;
        }
        else if (command == L"help" || command == L"/?" || command == L"-h" || command == L"--help") {
            PrintUsage();
            return 0;
        }
        else {
            std::wcerr << L"Unknown command: " << command << std::endl;
            PrintUsage();
            return 1;
        }
    }

    // No arguments - run as service
    if (!StartServiceCtrlDispatcherW(ServiceTable)) {
        DWORD error = GetLastError();
        if (error == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
            // Not running as service, run in console mode
            std::wcout << L"Running in console mode (not installed as service)" << std::endl;
            std::wcout << L"Use 'selection-logger.exe install' to install as service" << std::endl;

            // Convert to char** for console main
            char** charArgs = new char*[argc + 1];
            for (int i = 0; i < argc; i++) {
                int size = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, NULL, 0, NULL, NULL);
                charArgs[i] = new char[size];
                WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, charArgs[i], size, NULL, NULL);
            }
            charArgs[argc] = nullptr;

            int result = ConsoleMain(argc, charArgs);

            // Cleanup
            for (int i = 0; i < argc; i++) {
                delete[] charArgs[i];
            }
            delete[] charArgs;

            return result;
        }
        else {
            std::wcerr << L"Failed to start service control dispatcher: " << error << std::endl;
            return 1;
        }
    }

    return 0;
}

// Stub implementations for service management functions
bool StartService() {
    SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) {
        std::wcerr << L"Failed to open Service Control Manager: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE service = OpenServiceW(scm, SERVICE_NAME, SERVICE_START);
    if (!service) {
        std::wcerr << L"Failed to open service: " << GetLastError() << std::endl;
        CloseServiceHandle(scm);
        return false;
    }

    if (!::StartServiceW(service, 0, NULL)) {
        DWORD error = GetLastError();
        if (error != ERROR_SERVICE_ALREADY_RUNNING) {
            std::wcerr << L"Failed to start service: " << error << std::endl;
            CloseServiceHandle(service);
            CloseServiceHandle(scm);
            return false;
        }
        std::wcout << L"Service is already running." << std::endl;
    }
    else {
        std::wcout << L"Service started successfully." << std::endl;
    }

    CloseServiceHandle(service);
    CloseServiceHandle(scm);
    return true;
}

bool StopService() {
    SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) {
        std::wcerr << L"Failed to open Service Control Manager: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE service = OpenServiceW(scm, SERVICE_NAME, SERVICE_STOP);
    if (!service) {
        std::wcerr << L"Failed to open service: " << GetLastError() << std::endl;
        CloseServiceHandle(scm);
        return false;
    }

    SERVICE_STATUS status;
    if (!ControlService(service, SERVICE_CONTROL_STOP, &status)) {
        std::wcerr << L"Failed to stop service: " << GetLastError() << std::endl;
        CloseServiceHandle(service);
        CloseServiceHandle(scm);
        return false;
    }

    std::wcout << L"Service stopped successfully." << std::endl;

    CloseServiceHandle(service);
    CloseServiceHandle(scm);
    return true;
}

bool QueryServiceStatus() {
    SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) {
        std::wcerr << L"Failed to open Service Control Manager: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE service = OpenServiceW(scm, SERVICE_NAME, SERVICE_QUERY_STATUS);
    if (!service) {
        std::wcerr << L"Failed to open service: " << GetLastError() << std::endl;
        CloseServiceHandle(scm);
        return false;
    }

    SERVICE_STATUS_PROCESS status;
    DWORD bytesNeeded;
    if (!QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,
        reinterpret_cast<LPBYTE>(&status), sizeof(status), &bytesNeeded)) {
        std::wcerr << L"Failed to query service status: " << GetLastError() << std::endl;
        CloseServiceHandle(service);
        CloseServiceHandle(scm);
        return false;
    }

    std::wcout << L"Service status: ";
    switch (status.dwCurrentState) {
        case SERVICE_STOPPED:
            std::wcout << L"Stopped";
            break;
        case SERVICE_START_PENDING:
            std::wcout << L"Start pending";
            break;
        case SERVICE_STOP_PENDING:
            std::wcout << L"Stop pending";
            break;
        case SERVICE_RUNNING:
            std::wcout << L"Running";
            break;
        case SERVICE_CONTINUE_PENDING:
            std::wcout << L"Continue pending";
            break;
        case SERVICE_PAUSE_PENDING:
            std::wcout << L"Pause pending";
            break;
        case SERVICE_PAUSED:
            std::wcout << L"Paused";
            break;
        default:
            std::wcout << L"Unknown (" << status.dwCurrentState << L")";
            break;
    }
    std::wcout << std::endl;

    CloseServiceHandle(service);
    CloseServiceHandle(scm);
    return true;
}

#endif // _WIN32