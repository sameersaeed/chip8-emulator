#ifndef GUI_HPP
#define GUI_HPP

#include <iostream>
#include <string>
#include <vector>
#include "SDL2/SDL.h"

#include "chip8.hpp"

class Gui {
public:
    Gui(int scale, const std::string& path, Chip8& chip8);
    ~Gui();

    void cleanup();
    void handleInput();
    void updateDisplay();

    bool initialize();
    std::string romPath;

private:
    void handleError(const char* message);

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_texture;

    int m_scale;
    std::vector<uint32_t> m_buffer;

    uint8_t m_keypad[16] = {
        SDLK_x, SDLK_1, SDLK_2, SDLK_3, 
        SDLK_q, SDLK_w, SDLK_e, SDLK_a, 
        SDLK_s, SDLK_d, SDLK_z, SDLK_c, 
        SDLK_4, SDLK_r, SDLK_f, SDLK_v
    };

    Chip8& m_chip8;
};

#endif