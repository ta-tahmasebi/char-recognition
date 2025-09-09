#include "color.h"

namespace colorPrint {
    void print(const std::string &text, Color color, bool bold) {
        std::cout << "\033["
                  << (bold ? "1;" : "")
                  << static_cast<int>(color) << "m"
                  << text << "\033[0m";
    }

    void println(const std::string &text, Color color, bool bold) {
        print(text + "\n", color, bold);
    }
}