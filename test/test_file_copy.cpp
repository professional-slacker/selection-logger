#include <iostream>
#include <cstdlib>
#include <string>

int main() {
    std::cout << "ファイルコピー時のクリップボード内容テスト" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;

    std::cout << "1. まず、ファイルマネージャーで test_file.txt を選択してください" << std::endl;
    std::cout << "2. Ctrl+C でコピーしてください" << std::endl;
    std::cout << "3. このプログラムを実行してクリップボード内容を確認します" << std::endl;
    std::cout << std::endl;

    std::cout << "現在のクリップボード内容:" << std::endl;
    std::cout << "------------------------" << std::endl;

    // CLIPBOARD selection
    std::cout << "CLIPBOARD (Ctrl+C):" << std::endl;
    system("xclip -o -selection clipboard 2>&1");
    std::cout << std::endl;

    // PRIMARY selection
    std::cout << "PRIMARY (マウス選択):" << std::endl;
    system("xclip -o -selection primary 2>&1");
    std::cout << std::endl;

    // 生のデータを16進数で表示
    std::cout << "生データ (hexdump):" << std::endl;
    system("xclip -o -selection clipboard 2>/dev/null | hexdump -C | head -20");

    return 0;
}