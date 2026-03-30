#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <unistd.h>

namespace fs = std::filesystem;

// ログ保存先ディレクトリ（必要に応じて変更してください）
const std::string BASE_DIR = std::string(getenv("HOME")) + "/memories";

// 現在の年月を取得してディレクトリとファイルパスを生成
std::string get_log_file_path() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm* lt = std::localtime(&now);
    char year[5], month[3];
    std::strftime(year, sizeof(year), "%Y", lt);
    std::strftime(month, sizeof(month), "%m", lt);

    fs::path dir = fs::path(BASE_DIR) / year;
    fs::create_directories(dir);
    return (dir / (std::string(month) + ".txt")).string();
}
std::string get_x11_selection() {
    char buffer[128];
    std::string result = "";
    
    // stderrも拾うように 2>&1 を追加
    FILE* pipe = popen("xclip -o -selection primary 2>&1", "r"); 
    if (!pipe) return "ERROR: popen failed";

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);

    // ターミナルに直接出す（ここが空なら、xclip自体が何も得られていない）
    printf("Debug: Raw xclip output: [%s]\n", result.c_str());
    
    return result;
}

std::string get_x11_selection2() {
    char buffer[128];
    std::string result = "";

    // 1. まずは「マウスで選択(primary)」を試す
    // 2. それが空なら「クリップボード(clipboard)」を試す
    // コマンドを連結して実行します
    const char* cmd = "xclip -o -selection primary 2>/dev/null || xclip -o -selection clipboard 2>/dev/null";
    
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        perror("popen failed");
        return "";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    
    int status = pclose(pipe);

    // デバッグ用：取得した文字数を表示
    if (!result.empty()) {
        printf("Debug: Captured %zu characters.\n", result.length());
    } else {
        printf("Debug: xclip returned no text. (Status: %d)\n", status);
    }

    return result;
}



int main() {
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Error: Unable to open X display\n");
        return 1;
    }

    Window root = DefaultRootWindow(display);
    
    // 1. 先にキーコードを取得
    KeyCode key = XKeysymToKeycode(display, XStringToKeysym("m"));
    printf("Debug: X11 initialized. Keycode for 'm' is %d\n", key);

    // 2. NumLock/CapsLock 対策を含めて Grab
    // ControlMask だけではなく、他のロック状態との組み合わせも登録するのが定石
    unsigned int modifiers[] = {0, Mod2Mask, LockMask, Mod2Mask | LockMask};
    for (int i = 0; i < 4; i++) {
        XGrabKey(display, key, ControlMask | modifiers[i], root, 
                 False, GrabModeAsync, GrabModeAsync);
    }

    XEvent ev;
    printf("Debug: Waiting for event (Ctrl + m)...\n");

    while (true) {
        XNextEvent(display, &ev);

        if (ev.type == KeyPress) {
            // デバッグ表示：何らかのキー入力を検知した
            printf("Debug: KeyPress detected! Code: %u, State: %u\n", 
                   ev.xkey.keycode, ev.xkey.state);

            // 押されたキーが 'm' かどうかチェック
            if (ev.xkey.keycode == key) {
                printf("Debug: Ctrl + m confirmed! Attempting to get selection...\n");
                
                // 引数なしの get_x11_selection を呼ぶ場合
                std::string text = get_x11_selection(); 
                
                if (!text.empty()) {
                    std::string path = get_log_file_path();
                    std::ofstream ofs(path, std::ios::app);
                    printf("Debug: path output: [%s]\n", path.c_str());

                    if (ofs) {
                        auto now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                        std::string time_str = std::ctime(&now_c);
                        if (!time_str.empty()) time_str.pop_back(); // ctimeの改行を除く

                        // 1. 書き込み
                        ofs << "\n--- " << time_str << " ---\n";
                        ofs << text << "\n";
                        
                        // 2. 強制的にディスクへ書き出す
                        ofs.flush(); 
                        
                        // 3. ファイルを閉じる（これで確実に保存される）
                        ofs.close(); 

                        std::cout << "Successfully saved (" << text.length() << " bytes) to: " << path << std::endl;
                    } else {
                        // ここが実行されるなら、ファイルを開けていない
                        std::cerr << "!!! Error: Cannot open file: " << path << std::endl;
                    }
                } else {
                    // xclip が何も返さなかった場合
                    printf("Debug: Ctrl+m pressed, but selection (xclip) returned nothing.\n");
                    printf("Hint: Try highlighting some text or Copying (Ctrl+C) before pressing Ctrl+m.\n");
                }
            }           
        }
    }

    XCloseDisplay(display);
    return 0;
}
