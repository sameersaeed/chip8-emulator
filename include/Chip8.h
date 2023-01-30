#include <cstdint>
#include <array>

#ifndef CHIP8_H
#define CHIP8_H

class Chip8
{
public:
    Chip8();                        
    ~Chip8();

    void cycle();                   // emulating CPU cycle
    bool loadROM(const char* ROM);  // load ROM data

    uint8_t     display[64 * 32];   // storing pixel data
    uint8_t     key[16];            // keypad contains 16 different keys
    bool        drawFlag;           // for drawing pixel, 0 - blank, 1 - fill

    uint16_t mask;  // opcode & 0x000F,         
    uint16_t byte;  // opcode & 0x00FF,         
    uint16_t addr;  // opcode & 0x0FFF,         
    uint16_t x;     // (opcode & 0x0F00) >> 8,  
    uint16_t y;     // (opcode & 0x00F0) >> 4,  

private:
    void reset();                   // for setting up initial values

    uint8_t     memory[4096];       // 4KB RAM
    uint16_t    opcode;             // for tracking current opcode
    uint16_t    pc;                 // program counter
    uint16_t    index;              // index register
    uint8_t     V[16];              // V registers, V0-VF

    uint16_t    stack[16];          // stack of size 16
    uint16_t    sp;                 // stack pointer

    uint8_t     delayTimer;         // timers, decrement @ 60Hz
    uint8_t     soundTimer;         //         until they reach 0
};

#endif