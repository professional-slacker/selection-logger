#include "win32_compat.h"
#include <iostream>
#include <cstdio>
#include <cstring>

#ifndef _WIN32
// Linux implementation of Win32 clipboard API using xclip

bool OpenClipboard(void* hWndNewOwner) {
    // Always return true for Linux compatibility
    return true;
}

HANDLE GetClipboardData(unsigned int uFormat) {
    if (uFormat != CF_TEXT) {
        return nullptr;
    }

    // Use xclip to get clipboard text
    FILE* pipe = popen("xclip -o -selection clipboard 2>/dev/null", "r");
    if (!pipe) {
        return nullptr;
    }

    // Read all text
    std::string text;
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        text += buffer;
    }
    pclose(pipe);

    // Remove trailing newline
    if (!text.empty() && text.back() == '\n') {
        text.pop_back();
    }

    // Allocate "global" memory (just heap memory in Linux)
    char* data = new char[text.size() + 1];
    std::strcpy(data, text.c_str());

    return reinterpret_cast<HANDLE>(data);
}

void* GlobalLock(HANDLE hMem) {
    return reinterpret_cast<void*>(hMem);
}

bool GlobalUnlock(HANDLE hMem) {
    // Nothing to do in Linux
    return true;
}

bool CloseClipboard() {
    // Nothing to do in Linux
    return true;
}

#endif

// Cross-platform implementation
std::string GetClipboardTextCompat() {
#ifdef _WIN32
    // Native Windows implementation
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
#else
    // Linux implementation using our compatibility layer
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

    // Clean up allocated memory (Linux specific)
    delete[] reinterpret_cast<char*>(hData);

    return text;
#endif
}