#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <string>
#include "platform.h"

class Clipboard {
public:
    virtual ~Clipboard() = default;

    // Get text from clipboard/selection
    virtual std::string get_text(const std::string& selection_type = "clipboard") = 0;

    // Platform-specific factory method
    static Clipboard* create();

    // Convenience methods
    std::string get_primary() { return get_text("primary"); }
    std::string get_clipboard() { return get_text("clipboard"); }

protected:
    Clipboard() = default;
};

#endif // CLIPBOARD_H