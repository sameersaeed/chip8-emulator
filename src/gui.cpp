#include "gui.hpp"
#include <iostream>
#include <thread>

Gui::Gui(int scale, const std::string& romPath)
    : scale(scale), romPath(romPath), chip8(chip8), 
      window(nullptr), renderer(nullptr), texture(nullptr), buffer(2048) {
    chip8 = new Chip8();
}

Gui::~Gui() {
    cleanup();
}

void Gui::handleError(const char* message) {
    std::cerr << "[ERROR]\t(gui):\t" << message << "\n";
    exit(-1);
}

bool Gui::initialize() {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            std::string error = "Couldn't initialize SDL: " + std::string(SDL_GetError());
            handleError(error.c_str());
        }

        std::string filename = romPath.substr(romPath.find_last_of("/\\") + 1);

        window = SDL_CreateWindow(
            filename.c_str(),           
            SDL_WINDOWPOS_UNDEFINED, 
            SDL_WINDOWPOS_UNDEFINED,
            256 * scale,                
            128 * scale,                
            SDL_WINDOW_SHOWN
       );

    if (window == NULL) {
        std::string error = "Window could not be created: " + std::string(SDL_GetError());
        handleError(error.c_str());
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        64, 
        32
   );

    if (!chip8->loadROM(romPath.c_str())) {
        handleError("Couldn't load ROM");
    }

    return true;
}

void Gui::run() {
    while (true) {
        // cycle through instructions and process user input
        chip8->cycle();        
        handleInput();

        // draw to screen and wait 1ms
        if (chip8->drawFlag) { 
            updateDisplay();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Gui::handleInput() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            exit(0);
        }

        if (e.type == SDL_KEYDOWN) {                
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                exit(0);
            }   

            for (int i = 0; i < 16; ++i) {
                if (e.key.keysym.sym == keypad[i]) {
                    chip8->key.at(i) = 1; // set state to ON
                }
            }
        }
        if (e.type == SDL_KEYUP) {                  
            for (int i = 0; i < 16; ++i) {
                if (e.key.keysym.sym == keypad[i]) {
                    chip8->key.at(i) = 0; // set state to OFF
                }
            }
        }
    }
}

void Gui::updateDisplay() {
    for (int i = 0; i < 2048; ++i) 
        buffer[i] = (0x00FFFFFF * chip8->display[i]) | 0xFF000000;

    SDL_UpdateTexture(texture, 
                      NULL, 
                      static_cast<void*>(buffer.data()), 
                      64 * sizeof(uint32_t)
                    );
    SDL_RenderClear(renderer);
    SDL_RenderCopy(
        renderer, 
        texture, 
        NULL, 
        NULL
    );
    SDL_RenderPresent(renderer);
}

void Gui::cleanup() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}
