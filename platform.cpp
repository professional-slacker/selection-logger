#include "platform.h"
#include <cstdlib>
#include <cstring>

bool is_running_under_wine() {
#if PLATFORM_LINUX
    // Check for Wine environment variables
    const char* wine_version = std::getenv("WINE_VERSION");
    const char* wine_prefix = std::getenv("WINEPREFIX");
    const char* wine_arch = std::getenv("WINEARCH");

    // Also check for Wine in process name or other indicators
    if (wine_version != nullptr || wine_prefix != nullptr || wine_arch != nullptr) {
        return true;
    }

    // Check /proc/self/exe symlink or other Wine-specific indicators
    // This is a simple check - could be expanded for more robust detection
    FILE* pipe = popen("which wine 2>/dev/null", "r");
    if (pipe) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            pclose(pipe);
            return true; // Wine is installed
        }
        pclose(pipe);
    }
#endif
    return false;
}