#include "download.h"
#include "train.h"

int main(int argc, char *argv[]) {

    if(!downloadProcess()) {
        return -1;
    }


    train();
}
