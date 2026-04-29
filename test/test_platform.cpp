#include <X11/Xlib.h>
#include <iostream>

int main() {
    std::cout << "Testing platform connection (X11 on Linux)..." << std::endl;

    const char* display_name = getenv("DISPLAY");
    std::cout << "DISPLAY: " << (display_name ? display_name : "(null)") << std::endl;

    Display* display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "FAILED: Cannot open X display" << std::endl;
        return 1;
    }

    std::cout << "SUCCESS: X display opened" << std::endl;

    Window root = DefaultRootWindow(display);
    std::cout << "Root window: " << root << std::endl;

    XCloseDisplay(display);
    std::cout << "Test completed" << std::endl;
    return 0;
}
