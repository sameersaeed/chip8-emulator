#ifndef GUI_HPP
#define GUI_HPP

#include <vector>
#include <string>
#include "SDL2/SDL.h"

#include "chip8.hpp"

class Gui {
public:
    Gui(int scale, const std::string& romPath);
    ~Gui();

    bool initialize();
    void run();

private:
    void handleInput();
    void handleError(const char* message);
    void updateDisplay();
    void cleanup();

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    int scale;
    std::string romPath;
    std::vector<uint32_t> buffer;

    uint8_t keypad[16] = {
        SDLK_x, SDLK_1, SDLK_2, SDLK_3, 
        SDLK_q, SDLK_w, SDLK_e, SDLK_a, 
        SDLK_s, SDLK_d, SDLK_z, SDLK_c, 
        SDLK_4, SDLK_r, SDLK_f, SDLK_v
    };

    Chip8* chip8;
};

#endif