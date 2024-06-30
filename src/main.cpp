#include <iostream>
#include <memory>
#include <thread>

#include "chip8.hpp"
#include "gui.hpp"

void handleError(const char* message) {
    std::cerr << "[ERROR]\t(main):\t " << message << "\n";
    exit(-1);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        handleError("Invalid arguments were provided\nUsage: <display-scale> <path-to-ROM>");
    }

    // args
    int scale = atoi(argv[1]);
    std::string romPath = argv[2];

    if (scale <= 0) {
        handleError("Display scale must be at least 1");
    }

    Chip8 chip8;

    if (!chip8.loadROM(romPath.c_str())) {
        handleError("Couldn't load ROM");
    }

    Gui gui(scale, romPath, chip8);

    if (!gui.initialize()) {
        handleError("Couldn't initialize GUI");
    }

    while (true) {
        // cycle through instructions and process user input
        chip8.cycle();
        gui.handleInput();

        // draw to screen
        if (chip8.drawFlag) {
            gui.updateDisplay();
            chip8.drawFlag = false;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(800));
    }
}