#include <iostream>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <vector>

#ifndef CHIP8_H
#define CHIP8_H

class Chip8
{
    
enum opcodes                // CPU instructions
{
    _00E_    =   0x0000,    // 00E?
    _00E0    =   0x0000,    //  00E0 : CLS
    _00EE    =   0x000E,    //  00EE : RET
    _1nnn    =   0x1000,    // 1nnn : JMP addr
    _2nnn    =   0x2000,    // 2nnn : CALL addr
    _3xkk    =   0x3000,    // 3xkk : SE Vx, byte
    _4xkk    =   0x4000,    // 4xkk : SNE Vx, byte
    _5xy0    =   0x5000,    // 5xy0 : SE Vx, Vy
    _6xkk    =   0x6000,    // 6xkk : LD Vx, byte
    _7xkk    =   0x7000,    // 7xkk : ADD Vx, byte
    _8xy_    =   0x8000,    // 8xy?
    _8xy0    =   0x0000,    //  8xy0 : LD Vx, Vy
    _8xy1    =   0x0001,    //  8xy1 : OR Vx, Vy
    _8xy2    =   0x0002,    //  8xy2 : AND Vx, Vy 
    _8xy3    =   0x0003,    //  8xy3 : XOR Vx, Vy
    _8xy4    =   0x0004,    //  8xy4 : ADD Vx, Vy
    _8xy5    =   0x0005,    //  8xy5 : SUB Vx, Vy
    _8xy6    =   0x0006,    //  8xy6 : SHR Vx {, Vy}
    _8xy7    =   0x0007,    //  8xy7 : SUBN Vx, Vy
    _8xyE    =   0x000E,    //  8xyE : SHL Vx, Vy
    _9xy0    =   0x9000,    // 9xy0 : SNE Vx, Vy
    _Annn    =   0xA000,    // Annn : LD I, addr
    _Bnnn    =   0xB000,    // Bnnn : JP V0, addr
    _Cxkk    =   0xC000,    // Cxkk : RND Vx, byte
    _Dxyn    =   0xD000,    // Dxyn : DRW Vx, Vy, nibble
    _Ex__    =   0xE000,    // Ex??
    _Ex9E    =   0x009E,    //  Ex9E : SKP Vx
    _ExA1    =   0x00A1,    //  ExA1 : SKNP Vx
    _Fx__    =   0xF000,    // Fx??
    _Fx07    =   0x0007,    //  Fx07 : LD Vx, DT
    _Fx0A    =   0x000A,    //  Fx0A : LD Vx, K
    _Fx15    =   0x0015,    //  Fx15 : LD DT, Vx
    _Fx18    =   0x0018,    //  Fx18 : LD ST, Vx
    _Fx1E    =   0x001E,    //  Fx1E : ADD I, Vx
    _Fx29    =   0x0029,    //  Fx29 : LD F, Vx
    _Fx33    =   0x0033,    //  Fx33 : LD B, Vx
    _Fx55    =   0x0055,    //  Fx55 : LD [I], Vx
    _Fx65    =   0x0065,    //  Fx65 : LD Vx, [I]
};

uint8_t fontset[80] =               // font hex representations
{
    0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
    0x20, 0x60, 0x20, 0x20, 0x70,   // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
    0xF0, 0x80, 0xF0, 0x80, 0x80    // F
};

public:
    Chip8();
    ~Chip8();

    void cycle();
    std::vector<uint8_t> display;
    std::vector<uint8_t> key;

    bool loadROM(const char* ROM);
    bool drawFlag;

    uint16_t mask;                                                              // nibble - opcode & 0x000F         
    uint16_t byte;                                                              // kk     - opcode & 0x00FF         
    uint16_t addr;                                                              // addr   - opcode & 0x0FFF         
    uint16_t x;                                                                 // (opcode & 0x0F00) >> 8  
    uint16_t y;                                                                 // (opcode & 0x00F0) >> 4  

private:
    void reset();

    std::vector<uint8_t> V;
    std::vector<uint16_t> stack;
    std::vector<uint8_t> memory;

    uint16_t opcode;
    uint16_t pc;
    uint16_t index;
    uint16_t sp;

    uint8_t delayTimer;
    uint8_t soundTimer;
};

#endif