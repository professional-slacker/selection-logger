#include "clipboard.h"
#include <iostream>
#include <cstdio>
#include <memory>

#if PLATFORM_LINUX
class LinuxClipboard : public Clipboard {
public:
    std::string get_text(const std::string& selection_type) override {
        // Check if running under Wine
        if (is_running_under_wine()) {
            // Under Wine, try to use Win32 API through Wine
            return get_text_win32();
        } else {
            // Native Linux, use xclip
            return get_text_xclip(selection_type);
        }
    }

private:
    std::string get_text_xclip(const std::string& selection_type) {
        char buffer[4096];
        std::string result = "";

        // Use xclip for Linux/X11
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

        // Remove trailing newline if present
        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }

        return result;
    }

    std::string get_text_win32() {
        // This would require linking with Wine libraries or using dlopen
        // For simplicity, we'll use a system call to wine + a small Windows executable
        // or use xclip as fallback for now
        return get_text_xclip("clipboard");
    }
};
#endif

#if PLATFORM_WINDOWS
class WindowsClipboard : public Clipboard {
public:
    std::string get_text(const std::string& selection_type) override {
        // Windows only has one clipboard (no primary selection like X11)
        // Wine will emulate this API
        if (!OpenClipboard(nullptr)) {
            return "";
        }

        HANDLE hData = GetClipboardData(CF_TEXT);
        if (hData == nullptr) {
            CloseClipboard();
            return "";
        }

        char* pszText = static_cast<char*>(GlobalLock(hData));
        if (pszText == nullptr) {
            CloseClipboard();
            return "";
        }

        std::string text(pszText);

        GlobalUnlock(hData);
        CloseClipboard();

        return text;
    }
};
#endif

#if PLATFORM_MACOS
class MacClipboard : public Clipboard {
public:
    std::string get_text(const std::string& selection_type) override {
        // macOS implementation would go here
        // For now, return empty string
        return "";
    }
};
#endif

// Factory method implementation
Clipboard* Clipboard::create() {
#if PLATFORM_WINDOWS
    return new WindowsClipboard();
#elif PLATFORM_LINUX
    // Check if running under Wine
    if (is_running_under_wine()) {
        // In Wine environment, we might want to use Windows API
        // or stick with xclip depending on the setup
        return new LinuxClipboard(); // For now, use xclip even under Wine
    }
    return new LinuxClipboard();
#elif PLATFORM_MACOS
    return new MacClipboard();
#else
    // Unknown platform
    return nullptr;
#endif
}