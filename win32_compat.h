#ifndef WIN32_COMPAT_H
#define WIN32_COMPAT_H

#include <string>

// Win32 API compatible interface for clipboard access
// This provides a unified interface that works on both Windows and Linux

#ifdef _WIN32
    // Native Windows includes
    #include <windows.h>
#else
    // Linux compatibility layer
    typedef void* HANDLE;
    #define CF_TEXT 1

    // Mock Windows API functions for Linux
    bool OpenClipboard(void* hWndNewOwner);
    HANDLE GetClipboardData(unsigned int uFormat);
    void* GlobalLock(HANDLE hMem);
    bool GlobalUnlock(HANDLE hMem);
    bool CloseClipboard();
#endif

// Cross-platform clipboard text getter
std::string GetClipboardTextCompat();

#endif // WIN32_COMPAT_H