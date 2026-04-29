#include <iostream>
#include <cstdlib>
#include <string>

int main() {
    std::cout << "Testing system selections after Ctrl+A (X11 on Linux)..." << std::endl;
    std::cout << "1. Open a text editor or terminal" << std::endl;
    std::cout << "2. Type some text" << std::endl;
    std::cout << "3. Press Ctrl+A to select all" << std::endl;
    std::cout << "4. Then check the selections:" << std::endl;
    std::cout << std::endl;

    // Check PRIMARY selection
    std::cout << "PRIMARY selection (mouse selection):" << std::endl;
    system("xclip -o -selection primary 2>&1 | head -5");
    std::cout << std::endl;

    // Check CLIPBOARD selection
    std::cout << "CLIPBOARD selection (Ctrl+C):" << std::endl;
    system("xclip -o -selection clipboard 2>&1 | head -5");
    std::cout << std::endl;

    // Check both
    std::cout << "To manually test:" << std::endl;
    std::cout << "  xclip -o -selection primary" << std::endl;
    std::cout << "  xclip -o -selection clipboard" << std::endl;
    std::cout << std::endl;
    std::cout << "Note: Some applications may not update PRIMARY with Ctrl+A" << std::endl;

    return 0;
}