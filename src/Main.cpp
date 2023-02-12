/**
 * References:
 * http://devernay.free.fr/hacks/chip8/C8TECH10.HTM                                 (opcodes)
 * https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/   (0xDxyn implementation)
 * https://en.wikipedia.org/wiki/CHIP-8                                             (general info)
*/

#include "Chip8.h"

#include <iostream>
#include <thread>
#include <memory>
#include "SDL2/SDL.h"

uint8_t keypad[16] = 
{
    SDLK_x,
    SDLK_1, 
    SDLK_2, 
    SDLK_3, 
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v
};

int main(int argc, char* argv[]) 
{
    if (argc != 3) 
    {
        std::cout << "usage: " << argv[0] << "[display-scale] <path-to-ROM>\n";
        return -1;
    }

    if (atoi(argv[1]) <= 0)
    {
        std::cout << "[ERROR]\t\tdisplay scale must be at least 1\n"; 
        return -2;
    }

    std::vector<uint32_t> buffer;                   // storing pixel data
    buffer.resize(2048);
    
    std::unique_ptr<Chip8> chip8 { new Chip8() };   // creating chip8 instance

    SDL_Window* window = NULL;
    int scale = atoi(argv[1]);                      // user can set window size

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) 
    {
        std::cout << "[ERROR]\t\tCould not initialize SDL: " << SDL_GetError() << "\n";
        return -3;
    }

    std::string s(argv[2]);
    std::string filename = s.substr(s.find_last_of("/\\") + 1);

    window = SDL_CreateWindow(
                              filename.c_str(),     // ROM name
                              SDL_WINDOWPOS_UNDEFINED, 
                              SDL_WINDOWPOS_UNDEFINED,
                              256 * scale,          // width
                              128 * scale,          // height
                              SDL_WINDOW_SHOWN
                             );

    if (window == NULL)
    {
        std::cout << "[ERROR]\t\tWindow could not be created: " << SDL_GetError() << "\n";
        return -4;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             64, 
                                             32
                                            );

    if (!chip8->loadROM(argv[2]))
    {
        std::cerr << "[ERROR]\t\t Couldn't load ROM\n";
        return -5;
    }

    while (1)                                           // emulator loaded succesfully
    {
        chip8->cycle();                                 // cycle thru instructions
        
        SDL_Event e;
        while (SDL_PollEvent(&e)) 
        {
            if (e.type == SDL_QUIT) return 0;           // exit w/o key press

            if (e.type == SDL_KEYDOWN)                  // key pressed
            {
                if (e.key.keysym.sym == SDLK_ESCAPE) return 0;

                for (int i = 0; i < 16; ++i) 
                {
                    if (e.key.keysym.sym == keypad[i])  // get key
                        chip8->key.at(i) = 1;           // set state to ON
                }
            }
            if (e.type == SDL_KEYUP)                    // key released
            {
                for (int i = 0; i < 16; ++i) 
                {
                    if (e.key.keysym.sym == keypad[i])  // get key
                        chip8->key.at(i) = 0;           // set state to OFF
                }
            }
        }

        if (chip8->drawFlag)                            // drawing to screen
        {
            // updating screen and textures
            for (int i = 0; i < 2048; ++i) 
                buffer[i] = (0x00FFFFFF * chip8->display[i]) | 0xFF000000;

            SDL_UpdateTexture(texture, 
                              NULL, 
                              static_cast<void*>(buffer.data()), 
                              64 * sizeof(uint32_t)
                             );
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, 
                           texture, 
                           NULL, 
                           NULL
                          );
            SDL_RenderPresent(renderer);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}