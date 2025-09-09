#include <filesystem>

#include "UI.h"
#undef slots
#include <filesystem>

#include "download.h"
#undef slots

#include "train.h"
#undef slots





int main(int argc, char *argv[]) {
    if(!downloadProcess()){
        return -1;
    }
    if(!train()){
        return -1;
    }

    return runVisualApp(argc, argv);
}
