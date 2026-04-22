#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <unistd.h>
#include <cstring>
#include <csignal>

namespace fs = std::filesystem;

// X error handler
int x_error_handler(Display* display, XErrorEvent* event) {
    char error_text[256];
    XGetErrorText(display, event->error_code, error_text, sizeof(error_text));
    std::cerr << "X Error: " << error_text << std::endl;
    std::cerr << "  Major opcode: " << (int)event->request_code << std::endl;
    return 0; // Return 0 to indicate we handled the error
}

// Base directory for logs
std::string get_base_dir() {
    const char* home = getenv("HOME");
    if (!home) {
        // HOME環境変数が設定されていない場合は/tmpを使用
        return "/tmp/memories";
    }
    return std::string(home) + "/memories";
}

// Get log file path based on current Year/Month
std::string get_log_file_path() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm* lt = std::localtime(&now);
    char year[5], month[3];
    std::strftime(year, sizeof(year), "%Y", lt);
    std::strftime(month, sizeof(month), "%m", lt);

    fs::path dir = fs::path(get_base_dir()) / year;
    try {
        fs::create_directories(dir);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: Cannot create directory: " << e.what() << std::endl;
    }
    return (dir / (std::string(month) + ".txt")).string();
}

// Fetch text from system selection (X11 via xclip on Linux)
std::string get_system_selection(const std::string& selection_type) {
    char buffer[128];
    std::string result = "";

    std::string command = "xclip -o -selection " + selection_type + " 2>/dev/null";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: popen() failed. Is xclip installed?" << std::endl;
        return "";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }

    int status = pclose(pipe);
    if (status != 0) return ""; // Returns empty if xclip fails or selection is empty

    return result;
}

// Save text to log file
void save_to_log(const std::string& text) {
    // Skip if selection is empty or only whitespace
    if (text.empty() || text.find_first_not_of(" \t\n\r") == std::string::npos) {
        return;
    }

    std::string path = get_log_file_path();
    std::ofstream ofs(path, std::ios::app);

    if (ofs) {
        auto now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string time_str = std::ctime(&now_c);
        if (!time_str.empty()) time_str.pop_back(); // Remove trailing newline

        ofs << "\n--- " << time_str << " ---\n";
        ofs << text << "\n";
        ofs.close();

        std::cout << "Saved: " << text.length() << " bytes to " << path << std::endl;
    } else {
        std::cerr << "Error: Could not write to file: " << path << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== ENTERING main() ===" << std::endl;

    // Parse command line arguments
    std::string mode = "hybrid"; // Default: hybrid mode (both PRIMARY and CLIPBOARD)
    int poll_interval_ms = 1000; // Default: 1 second

    std::cout << "Parsing command line arguments..." << std::endl;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--primary") == 0) {
            mode = "primary";
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--clipboard") == 0) {
            mode = "clipboard";
        } else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--both") == 0) {
            mode = "hybrid";
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  -p, --primary        Monitor PRIMARY selection (mouse selection)\n"
                      << "  -c, --clipboard      Monitor CLIPBOARD selection (Ctrl+C)\n"
                      << "  -b, --both           Monitor both PRIMARY and CLIPBOARD (default)\n"
                      << "  -i N, --interval N   Polling interval in milliseconds (default: 1000)\n"
                      << "  -h, --help           Show this help message\n"
                      << "\n"
                      << "Modes:\n"
                      << "  PRIMARY:    Saves text when selected with mouse (no Ctrl+C needed)\n"
                      << "  CLIPBOARD:  Saves text when copied with Ctrl+C (works with Ctrl+A → Ctrl+C)\n"
                      << "  BOTH:       Monitors both selections (recommended)\n";
            return 0;
        } else if ((strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interval") == 0) && i + 1 < argc) {
            poll_interval_ms = atoi(argv[i + 1]);
            if (poll_interval_ms < 100) {
                std::cerr << "Warning: Interval too short, using minimum 100ms" << std::endl;
                poll_interval_ms = 100;
            }
            i++; // Skip next argument
        }
    }

    std::cout << "Arguments parsed. mode=" << mode
              << ", poll_interval_ms=" << poll_interval_ms << std::endl;

    std::cout << "selection-logger started in " << mode << " mode." << std::endl;
    std::cout << "Polling interval: " << poll_interval_ms << " ms" << std::endl;

    if (mode == "primary") {
        std::cout << "Monitoring PRIMARY selection (mouse selection)" << std::endl;
    } else if (mode == "clipboard") {
        std::cout << "Monitoring CLIPBOARD selection (Ctrl+C)" << std::endl;
    } else {
        std::cout << "Monitoring both PRIMARY and CLIPBOARD selections" << std::endl;
    }

    // Track last selections to detect changes
    std::string last_primary = "";
    std::string last_clipboard = "";

    while (true) {
        if (mode == "primary" || mode == "hybrid") {
            std::string current_primary = get_system_selection("primary");
            if (current_primary != last_primary &&
                !current_primary.empty() &&
                current_primary.find_first_not_of(" \t\n\r") != std::string::npos) {

                std::cout << "PRIMARY selection changed" << std::endl;
                save_to_log(current_primary);
                last_primary = current_primary;
            }
        }

        if (mode == "clipboard" || mode == "hybrid") {
            std::string current_clipboard = get_system_selection("clipboard");
            if (current_clipboard != last_clipboard &&
                !current_clipboard.empty() &&
                current_clipboard.find_first_not_of(" \t\n\r") != std::string::npos) {

                std::cout << "CLIPBOARD selection changed" << std::endl;
                save_to_log(current_clipboard);
                last_clipboard = current_clipboard;
            }
        }

        usleep(poll_interval_ms * 1000); // Convert ms to microseconds
    }

    return 0;
}