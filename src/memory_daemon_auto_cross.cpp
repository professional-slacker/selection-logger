// Cross-platform auto-monitoring daemon
// Supports both Windows (Win32 API) and Linux (X11)

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <atomic>

// Platform-specific includes
#ifdef _WIN32
    #include <windows.h>
    #include <shlobj.h>
    #include "service_win32.h"
#else
    #include <X11/Xlib.h>
    #include <X11/Xatom.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

#include "platform.h"
#include "clipboard.h"

// Global flag for graceful shutdown
std::atomic<bool> g_running{true};

// Signal handler for graceful shutdown
void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        g_running = false;
    }
}

// Create directory if it doesn't exist
bool create_directory(const std::string& path) {
#ifdef _WIN32
    return CreateDirectoryA(path.c_str(), NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
#else
    return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
#endif
}

// Get log file path based on current Year/Month
std::string get_log_file_path() {
    std::string base_dir;

#ifdef _WIN32
    // Windows: use Documents folder with SHGetFolderPathW (more compatible with MinGW)
    wchar_t documents[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, 0, documents))) {
        char documents_utf8[MAX_PATH];
        WideCharToMultiByte(CP_UTF8, 0, documents, -1, documents_utf8, MAX_PATH, NULL, NULL);
        base_dir = std::string(documents_utf8) + "\\SelectionLogs";
    } else {
        // Fallback to current directory
        base_dir = "SelectionLogs";
    }
#else
    // Linux: use home directory
    const char* home = getenv("HOME");
    if (!home) home = ".";
    base_dir = std::string(home) + "/memories";
#endif

    // Create directory
    create_directory(base_dir);

    // Get current date
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    std::tm* tmp = std::localtime(&time);
    tm = *tmp;
#endif

    // Build file path
    std::stringstream ss;
    ss << base_dir;
#ifdef _WIN32
    ss << "\\";
#else
    ss << "/";
#endif
    ss << (tm.tm_year + 1900) << "-"
       << std::setw(2) << std::setfill('0') << (tm.tm_mon + 1) << ".txt";

    return ss.str();
}

// Platform-specific initialization
#ifdef _WIN32
// Windows: Set up console control handler
BOOL WINAPI console_handler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        g_running = false;
        return TRUE;
    }
    return FALSE;
}

bool init_platform() {
    return SetConsoleCtrlHandler(console_handler, TRUE);
}

void cleanup_platform() {
    // Nothing special for Windows
}
#else
// Linux: Set up X11 display
Display* g_display = nullptr;

bool init_platform() {
    // Set up signal handlers
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // Open X11 display
    g_display = XOpenDisplay(NULL);
    if (!g_display) {
        std::cerr << "Error: Unable to open X display" << std::endl;
        return false;
    }

    return true;
}

void cleanup_platform() {
    if (g_display) {
        XCloseDisplay(g_display);
        g_display = nullptr;
    }
}
#endif

// Main function
int main(int argc, char* argv[]) {
    std::string mode = "primary"; // Default mode

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--clipboard" || arg == "-c") {
            mode = "clipboard";
        } else if (arg == "--primary" || arg == "-p") {
            mode = "primary";
        } else if (arg == "--both" || arg == "-b") {
            mode = "both";
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Selection Logger v1.0" << std::endl;
            std::cout << "Usage:" << std::endl;
            std::cout << "  selection-logger [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --primary, -p    Monitor primary selection (default)" << std::endl;
            std::cout << "  --clipboard, -c  Monitor clipboard selection" << std::endl;
            std::cout << "  --both, -b       Monitor both selections" << std::endl;
            std::cout << "  --version, -v    Show version information" << std::endl;
            std::cout << "  --help, -h       Show this help" << std::endl;
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            std::cout << "Selection Logger v1.0" << std::endl;
            std::cout << "Cross-platform clipboard monitoring daemon" << std::endl;
            std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
#ifdef PLATFORM_LINUX
            std::cout << "Platform: Linux (X11)" << std::endl;
#endif
#ifdef _WIN32
            std::cout << "Platform: Windows (Win32)" << std::endl;
#endif
            return 0;
        }
    }

    std::cout << "Selection Logger Auto-Monitoring started in " << mode << " mode." << std::endl;

    // Initialize platform
    if (!init_platform()) {
        return 1;
    }

    // Create clipboard instance
    Clipboard* clipboard = Clipboard::create();
    if (!clipboard) {
        std::cerr << "Error: Failed to create clipboard instance" << std::endl;
        cleanup_platform();
        return 1;
    }

    std::cout << "Press Ctrl+C to exit." << std::endl;

    std::string last_primary;
    std::string last_clipboard;

    while (g_running) {
        if (mode == "primary" || mode == "both") {
            std::string current_primary = clipboard->get_primary();

            if (!current_primary.empty() && current_primary != last_primary) {
                last_primary = current_primary;

                // Skip if text is only whitespace
                if (current_primary.find_first_not_of(" \t\n\r") == std::string::npos) {
                    continue;
                }

                std::string path = get_log_file_path();
                std::ofstream ofs(path, std::ios::app);

                if (ofs) {
                    auto now = std::chrono::system_clock::now();
                    auto time = std::chrono::system_clock::to_time_t(now);
#ifdef _WIN32
                    std::tm tm;
                    localtime_s(&tm, &time);
                    ofs << "\n--- PRIMARY " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " ---\n";
#else
                    std::tm* tm = std::localtime(&time);
                    ofs << "\n--- PRIMARY " << std::put_time(tm, "%Y-%m-%d %H:%M:%S") << " ---\n";
#endif
                    ofs << current_primary << "\n";
                    ofs.close();

                    std::cout << "Saved PRIMARY: " << current_primary.length() << " bytes" << std::endl;
                }
            }
        }

        if (mode == "clipboard" || mode == "both") {
            std::string current_clipboard = clipboard->get_clipboard();

            if (!current_clipboard.empty() && current_clipboard != last_clipboard) {
                last_clipboard = current_clipboard;

                // Skip if text is only whitespace
                if (current_clipboard.find_first_not_of(" \t\n\r") == std::string::npos) {
                    continue;
                }

                std::string path = get_log_file_path();
                std::ofstream ofs(path, std::ios::app);

                if (ofs) {
                    auto now = std::chrono::system_clock::now();
                    auto time = std::chrono::system_clock::to_time_t(now);
#ifdef _WIN32
                    std::tm tm;
                    localtime_s(&tm, &time);
                    ofs << "\n--- CLIPBOARD " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " ---\n";
#else
                    std::tm* tm = std::localtime(&time);
                    ofs << "\n--- CLIPBOARD " << std::put_time(tm, "%Y-%m-%d %H:%M:%S") << " ---\n";
#endif
                    ofs << current_clipboard << "\n";
                    ofs.close();

                    std::cout << "Saved CLIPBOARD: " << current_clipboard.length() << " bytes" << std::endl;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Cleanup
    delete clipboard;
    cleanup_platform();

    std::cout << "Selection Logger Auto-Monitoring stopped." << std::endl;
    return 0;
}