#include <thread>
#include <emscripten.h>

#include "chip8.hpp"
#include "gui.hpp"

Chip8 chip8;
Gui* gui = nullptr;

extern "C" {
    void load(char* path) {
        chip8.loadROM(path);
        gui = new Gui(1, path, chip8);
    }

    void stop() {
        emscripten_cancel_main_loop();
        gui->cleanup();
    }
}

void mainLoop() {
    // gui isn't ready yet
    if (!gui->initialize()) {   
        return;
    }

    // cycle through instructions and process user input
    chip8.cycle();
    gui->handleInput();

    // draw to screen and wait 1ms
    if (chip8.drawFlag) {
        gui->updateDisplay();
        chip8.drawFlag = false;
    }
}

int main() {
    emscripten_set_main_loop(mainLoop, 0, 0);
    return EXIT_SUCCESS;
}