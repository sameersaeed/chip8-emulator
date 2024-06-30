#include "gui.hpp"

Gui::Gui(int scale, const std::string& path, Chip8& chip8)
    : romPath(path), m_window(nullptr), m_renderer(nullptr), m_texture(nullptr), 
        m_scale(scale), m_buffer(2048), m_chip8(chip8) {}

Gui::~Gui() {
    cleanup();
}

void Gui::handleError(const char* message) {
    std::cerr << "[ERROR]\t(gui):\t " << message << "\n";
    exit(-1);
}

bool Gui::initialize() {
    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::string error = "Couldn't initialize SDL: " + std::string(SDL_GetError());
        handleError(error.c_str());
    }

    std::string filename = romPath.substr(romPath.find_last_of("/\\") + 1);

    m_window = SDL_CreateWindow(
        filename.c_str(),           
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED,
        256 * m_scale,                
        128 * m_scale,                
        SDL_WINDOW_SHOWN
    );

    if (m_window == NULL) {
        std::string error = "Window couldn't be created: " + std::string(SDL_GetError());
        handleError(error.c_str());
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    m_texture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        64, 
        32
   );

    return true;
}

void Gui::handleInput() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            exit(0);
        }

        if (e.type == SDL_KEYDOWN) {            // key press             
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                exit(0);
            }   

            for (int i = 0; i < 16; ++i) {
                if (e.key.keysym.sym == m_keypad[i]) {
                    m_chip8.key.at(i) = 1;     // set state to ON
                }
            }
        }

        if (e.type == SDL_KEYUP) {              // key release
            for (int i = 0; i < 16; ++i) {
                if (e.key.keysym.sym == m_keypad[i]) {
                    m_chip8.key.at(i) = 0;     // set state to OFF
                }
            }
        }
    }
}

void Gui::updateDisplay() {
    for (int i = 0; i < 2048; ++i) 
        m_buffer[i] = (0x00FFFFFF * m_chip8.display[i]) | 0xFF000000;

    SDL_UpdateTexture(
        m_texture, 
        NULL, 
        static_cast<void*>(m_buffer.data()), 
        64 * sizeof(uint32_t)
    );

    SDL_RenderClear(m_renderer);

    SDL_RenderCopy(
        m_renderer, 
        m_texture, 
        NULL, 
        NULL
    );

    SDL_RenderPresent(m_renderer);
}

void Gui::cleanup() {
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }

    SDL_Quit();
}
