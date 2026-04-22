#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <unistd.h>

namespace fs = std::filesystem;

// Base directory for logs
const std::string BASE_DIR = std::string(getenv("HOME")) + "/memories";

// Get log file path based on current Year/Month
std::string get_log_file_path() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm* lt = std::localtime(&now);
    char year[5], month[3];
    std::strftime(year, sizeof(year), "%Y", lt);
    std::strftime(month, sizeof(month), "%m", lt);

    fs::path dir = fs::path(BASE_DIR) / year;
    try {
        fs::create_directories(dir);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: Cannot create directory: " << e.what() << std::endl;
    }
    return (dir / (std::string(month) + ".txt")).string();
}

// Fetch text from system primary selection (X11 via xclip on Linux)
std::string get_system_selection() {
    char buffer[128];
    std::string result = "";
    
    // Using xclip to fetch primary selection
    FILE* pipe = popen("xclip -o -selection primary 2>/dev/null", "r"); 
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

int main() {
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "Error: Unable to open X display" << std::endl;
        return 1;
    }

    Window root = DefaultRootWindow(display);
    KeyCode key = XKeysymToKeycode(display, XStringToKeysym("m"));

    // Grab Ctrl + M with various modifier combinations (NumLock/CapsLock)
    unsigned int modifiers[] = {0, Mod2Mask, LockMask, Mod2Mask | LockMask};
    for (int i = 0; i < 4; i++) {
        XGrabKey(display, key, ControlMask | modifiers[i], root, 
                 False, GrabModeAsync, GrabModeAsync);
    }

    std::cout << "selection-logger started. Press Ctrl+M to save selection." << std::endl;

    XEvent ev;
    while (true) {
        XNextEvent(display, &ev);

        if (ev.type == KeyPress && ev.xkey.keycode == key) {
            std::string text = get_system_selection(); 
            
            // Skip if selection is empty or only whitespace
            if (text.empty() || text.find_first_not_of(" \t\n\r") == std::string::npos) {
                continue; 
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
    }

    XCloseDisplay(display);
    return 0;
}
