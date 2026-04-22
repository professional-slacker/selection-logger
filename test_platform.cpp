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

    // Try to grab a key
    KeyCode key = XKeysymToKeycode(display, XStringToKeysym("m"));
    std::cout << "Keycode for 'm': " << (int)key << std::endl;

    std::cout << "Attempting to grab Ctrl+M..." << std::endl;
    int result = XGrabKey(display, key, ControlMask, root, False, GrabModeAsync, GrabModeAsync);
    std::cout << "XGrabKey result: " << result << std::endl;

    if (result != 0) {
        std::cerr << "XGrabKey failed" << std::endl;
    } else {
        std::cout << "XGrabKey succeeded" << std::endl;
    }

    XCloseDisplay(display);
    std::cout << "Test completed" << std::endl;
    return 0;
}