#include <iostream>
#include <memory>

#include "gui.hpp"

void handleError(const char* message) {
    std::cerr << "[ERROR]\t(main):\t" << message << "\n";
    exit(-1);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        handleError("invalid arguments were provided\nusage: <display-scale> <path-to-ROM>");
    }

    // args
    int scale = atoi(argv[1]);
    std::string romPath = argv[2];

    if (scale <= 0) {
        handleError("display scale must be at least 1");
    }

    Gui gui(scale, romPath);

    if (!gui.initialize()) {
        handleError("couldn't initialize GUI");
    }

    gui.run();
    return 0;
}