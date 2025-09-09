#include "UI.h"

#undef slots

#include "download.h"
#include "train.h"
#include <iostream>
#include <string>

void printHelp() {
    std::cout << "Usage: program [option]\n";
    std::cout << "Options:\n";
    std::cout << "  download   Run the download process\n";
    std::cout << "  train      Run the training process\n";
    std::cout << "  ui         Run the visual application\n";
    std::cout << "  all        Run download, train, and UI sequentially\n";
    std::cout << "  help       Show this help message\n";
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Error: No option provided.\n";
        printHelp();
        return -1;
    }

    if (argc > 3) {
        std::cerr << "Error: too many options provided.\n";
        printHelp();
        return -1;
    }

    std::string option = argv[1];

    if (option == "help") {
        printHelp();
        return 0;
    }

    if (option == "download" || option == "all") {
        if (!downloadProcess()) {
            return -1;
        }
    }

    if (option == "train" || option == "all") {
        if (!train()) {
            return -1;
        }
    }

    if (option == "ui" || option == "all") {
        if(!hasModelFiles("./data")){
            std::cerr << "Missing some Model files." << std::endl;
            std::cerr << "First train them." << std::endl;
            return -1;
        }
        if (runVisualApp(argc, argv)) {
            return -1;
        }
    }

    return 0;
}
