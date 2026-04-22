#ifndef PLATFORM_H
#define PLATFORM_H

// Platform detection macros
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS 1
    #define PLATFORM_LINUX 0
    #define PLATFORM_MACOS 0
#elif defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_WINDOWS 0
    #define PLATFORM_LINUX 0
    #define PLATFORM_MACOS 1
#elif defined(__linux__)
    #define PLATFORM_WINDOWS 0
    #define PLATFORM_LINUX 1
    #define PLATFORM_MACOS 0
#else
    #define PLATFORM_WINDOWS 0
    #define PLATFORM_LINUX 0
    #define PLATFORM_MACOS 0
#endif

// Wine detection (Linux環境でWineが動作しているか検出)
bool is_running_under_wine();

// Platform-specific includes
#if PLATFORM_WINDOWS
    #include <windows.h>
    #include <tchar.h>
#elif PLATFORM_LINUX
    #include <X11/Xlib.h>
    #include <X11/Xatom.h>
    #include <unistd.h>
#elif PLATFORM_MACOS
    // macOS specific includes
    #include <Carbon/Carbon.h>
#endif

#endif // PLATFORM_H