#pragma once
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

    inline void print(const std::string &text, Color color = Color::RESET, bool bold = false) {
        std::cout << "\033["
                  << (bold ? "1;" : "")
                  << static_cast<int>(color) << "m"
                  << text << "\033[0m";
    }

    inline void println(const std::string &text, Color color = Color::RESET, bool bold = false) {
        print(text + "\n", color, bold);
    }
}
