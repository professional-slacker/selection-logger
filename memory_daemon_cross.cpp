// Cross-platform memory daemon
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
// Linux: Set up X11 display and signal handlers
Display* g_display = nullptr;
Window g_root = 0;
KeyCode g_key = 0;

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

    g_root = DefaultRootWindow(g_display);
    g_key = XKeysymToKeycode(g_display, XStringToKeysym("m"));

    // Grab Ctrl + M with various modifier combinations
    unsigned int modifiers[] = {0, Mod2Mask, LockMask, Mod2Mask | LockMask};
    for (int i = 0; i < 4; i++) {
        XGrabKey(g_display, g_key, ControlMask | modifiers[i], g_root,
                 False, GrabModeAsync, GrabModeAsync);
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
    std::cout << "Selection Logger - Cross-platform clipboard logger" << std::endl;

    // Check for command line arguments
    if (argc > 1) {
        std::string command = argv[1];
        if (command == "--help" || command == "-h") {
            std::cout << "Usage:" << std::endl;
            std::cout << "  selection-logger [--help]" << std::endl;
            std::cout << "  selection-logger --service (Windows service mode)" << std::endl;
            return 0;
        }
#ifdef _WIN32
        else if (command == "--service") {
            // Windows service mode - use service_win32 implementation
            std::cout << "Running in Windows service mode..." << std::endl;
            // This would call the Windows service main function
            // For now, fall through to console mode
        }
#endif
    }

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

    std::cout << "Selection Logger started. Press Ctrl+M to save selection." << std::endl;
    std::cout << "Press Ctrl+C to exit." << std::endl;

#ifdef _WIN32
    // Windows: Poll for Ctrl+M in console
    std::cout << "Note: On Windows, clipboard is monitored continuously." << std::endl;

    std::string last_text;
    while (g_running) {
        // Check for Ctrl+M (Windows doesn't have X11 event system)
        // For now, just monitor clipboard continuously
        std::string text = clipboard->get_clipboard();

        if (!text.empty() && text != last_text) {
            last_text = text;

            // Skip if text is only whitespace
            if (text.find_first_not_of(" \t\n\r") == std::string::npos) {
                continue;
            }

            std::string path = get_log_file_path();
            std::ofstream ofs(path, std::ios::app);

            if (ofs) {
                auto now = std::chrono::system_clock::now();
                auto time = std::chrono::system_clock::to_time_t(now);
                std::tm tm;
#ifdef _WIN32
                localtime_s(&tm, &time);
#else
                std::tm* tmp = std::localtime(&time);
                tm = *tmp;
#endif

                ofs << "\n--- " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " ---\n";
                ofs << text << "\n";
                ofs.close();

                std::cout << "Saved: " << text.length() << " bytes to " << path << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
#else
    // Linux: X11 event loop
    XEvent ev;
    while (g_running) {
        // Check for events with timeout
        if (XPending(g_display) > 0) {
            XNextEvent(g_display, &ev);

            if (ev.type == KeyPress && ev.xkey.keycode == g_key) {
                std::string text = clipboard->get_primary();

                // Skip if selection is empty or only whitespace
                if (text.empty() || text.find_first_not_of(" \t\n\r") == std::string::npos) {
                    continue;
                }

                std::string path = get_log_file_path();
                std::ofstream ofs(path, std::ios::app);

                if (ofs) {
                    auto now = std::chrono::system_clock::now();
                    auto time = std::chrono::system_clock::to_time_t(now);
                    std::tm* tm = std::localtime(&time);

                    ofs << "\n--- " << std::put_time(tm, "%Y-%m-%d %H:%M:%S") << " ---\n";
                    ofs << text << "\n";
                    ofs.close();

                    std::cout << "Saved: " << text.length() << " bytes to " << path << std::endl;
                }
            }
        } else {
            // No events, sleep briefly
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
#endif

    // Cleanup
    delete clipboard;
    cleanup_platform();

    std::cout << "Selection Logger stopped." << std::endl;
    return 0;
}