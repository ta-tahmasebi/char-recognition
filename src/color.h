#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <string>

namespace colorPrint {
    enum class Color {
        RESET = 0,
        RED = 31,
        GREEN = 32,
        YELLOW = 33,
        BLUE = 34,
        MAGENTA = 35,
        CYAN = 36,
        WHITE = 37,
        BRIGHT_RED = 91,
        BRIGHT_GREEN = 92,
        BRIGHT_YELLOW = 93,
        BRIGHT_BLUE = 94,
        BRIGHT_MAGENTA = 95,
        BRIGHT_CYAN = 96,
        BRIGHT_WHITE = 97
    };

    void print(const std::string &text, Color color = Color::RESET, bool bold = false);
    void println(const std::string &text, Color color = Color::RESET, bool bold = false);
}

#endif