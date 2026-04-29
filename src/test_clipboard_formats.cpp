#include <iostream>
#include <cstdlib>
#include <string>

int main() {
    std::cout << "クリップボードの利用可能な形式を確認" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;

    // xclipで利用可能なターゲット（形式）をリスト
    std::cout << "xclipで利用可能なターゲット:" << std::endl;
    std::cout << "---------------------------" << std::endl;
    system("xclip -selection clipboard -t TARGETS -o 2>&1");

    std::cout << std::endl;
    std::cout << "各種形式で取得してみる:" << std::endl;
    std::cout << "----------------------" << std::endl;

    // 一般的な形式
    const char* targets[] = {
        "UTF8_STRING",
        "STRING",
        "TEXT",
        "text/plain",
        "text/plain;charset=utf-8",
        "text/uri-list",
        "x-special/gnome-copied-files",
        "application/x-kde-cutselection",
        NULL
    };

    for (int i = 0; targets[i] != NULL; i++) {
        std::cout << "\n[" << targets[i] << "]:" << std::endl;
        std::string cmd = "xclip -selection clipboard -t '" + std::string(targets[i]) + "' -o 2>&1 | head -c 200";
        system(cmd.c_str());
        std::cout << std::endl;
    }

    return 0;
}