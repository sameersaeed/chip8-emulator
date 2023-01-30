#include "Chip8.h"

#include <iostream>
#include <fstream>
#include <random>
#include <time.h>

enum opcodes                // for instructions to be executed in CPU cy
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
    _Fx29    =   0x3029,    //  Fx29 : LD F, Vx
    _Fx33    =   0x0033,    //  Fx33 : LD B, Vx
    _Fx55    =   0x0055,    //  Fx55 : LD [I], Vx
    _Fx65    =   0x0065,    //  Fx65 : LD Vx, [I]
};

Chip8::Chip8()
{
    pc      = 0x200;        // ROM instructions are stored starting at this point
    opcode  = 0;
    index   = 0;
    sp      = 0; 
}

Chip8::~Chip8(){}


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

void Chip8::reset()
{
    // resetting data 
    for (int i = 0; i < 4096; ++i)  // clearing memory
        memory[i] = 0;

    for (int i = 0; i < 2048; ++i)
        display[i] = 0;             // emptying all pxs

    for (int i = 0; i < 80; ++i)
        memory[i] = fontset[i];     // loading fonts into memory

    for (int i = 0; i < 16; ++i)
    {
        stack[i]    = 0;    // emptying stack
        key[i]      = 0;    // keypad inputs
        V[i]        = 0;    // registers V0-VF
    }

    // resetting timers
    soundTimer = 0;
    delayTimer = 0;

    srand(time(NULL));     // rng
}

bool Chip8::loadROM(const char* ROM)
{
    reset();   // initializing data

    // reading file as binary, move to EOF
    std::ifstream f(ROM, std::ios::ate);

    if (f.is_open())
    {
        std::streampos fileSize = f.tellg();    // getting file's size
        char* buf = new char[fileSize];         // storing file data in buffer

        // reading file from beginning to end
        f.seekg(0, std::ios::beg);
        f.read(buf, fileSize);
        f.close();

        for(long i = 0; i < fileSize; ++i)
            memory[0x200 + i] = buf[i];         // loading buffer data to memory

        delete[] buf;                           // free buffer memory
        return true;
    }
    else
    {
        std::cerr << "Error: Could not load file: " << ROM << "\n";
        return false;
    }
}

void Chip8::cycle() // cpu cycles: fetch --> decode --> execute opcode
{ 
    // fetch
    opcode = memory[pc] << 8 | memory[pc + 1];

    mask    = opcode & 0x000F;
    byte    = opcode & 0x00FF;
    addr    = opcode & 0x0FFF;
    x       = (opcode & 0x0F00) >> 8;
    y       = (opcode & 0x00F0) >> 4; 
    
    // decode
    switch(opcode & 0xF000)
    {
        case _00E_:                                // 00E?
            switch (mask) 
            {
                case _00E0:                        // 00E0 : CLS
                    for (int i = 0; i < 2048; ++i) 
                        display[i] = 0;            // clears display
                    drawFlag = true;
                    pc += 2;
                    break;
                case _00EE:                        // 00EE : RET
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;
                default:                           // invalid opcode
                    printf("Unknown opcode [0x00E?]: %.4X\n", opcode);
                    return;
            } 
            break;
        case _1nnn:                                // 1nnn : JMP addr
            pc = addr;
            break;
        case _2nnn:                                // 2nnn : CALL addr
            stack[sp] = pc;
            ++sp;
            pc = addr;
            break;
        case _3xkk:                                // 3xkk : SE Vx, byte
            if (V[x] == byte)
                pc += 4;
            else
                pc += 2;
            break;
        case _4xkk:                                // 4xkk : SNE Vx, byte
            if (V[x] != byte)
                pc += 4;
            else
                pc += 2;
            break;
        case _5xy0:                                // 5xy0 : SE Vx, Vy
            if (V[x] == V[y])
                pc += 4;
            else
                pc += 2;
            break;
        case _6xkk:                                // 6xkk : LD Vx, byte
            V[x] = byte;
            pc += 2;
            break;
        case _7xkk:                                // 7xkk : ADD Vx, byte
            V[x] += byte;
            pc += 2;
            break;
        case _8xy_:                                // 8xy?
            switch (mask) 
            {
                case _8xy0:                        // 8xy0 : LD Vx, Vy
                    V[x] = V[y];
                    pc += 2;
                    break;
                case _8xy1:                        // 8xy1 : OR Vx, Vy
                    V[x] |= V[y];
                    pc += 2;
                    break;
                case _8xy2:                        // 8xy2 : AND Vx, Vy 
                    V[x] &= V[y];
                    pc += 2;
                    break;
                case _8xy3:                        // 8xy3 : XOR Vx, Vy
                    V[x] ^= V[y];
                    pc += 2;
                    break;
                case _8xy4:                        // 8xy4 : ADD Vx, Vy
                    V[x] += V[y];
                    if(V[y] > (0xFF - V[x]))
                        V[0xF] = 1; //carry
                    else
                        V[0xF] = 0;
                    pc += 2;
                    break;
                case _8xy5:                        // 8xy5 : SUB Vx, Vy
                    if(V[y] > V[x])
                        V[0xF] = 0; // there is a borrow
                    else
                        V[0xF] = 1;
                    V[x] -= V[y];
                    pc += 2;
                    break;
                case _8xy6:                        // 8xy6 : SHR Vx {, Vy}
                    V[0xF] = V[x] & 0x1;
                    V[x] >>= 1;
                    pc += 2;
                    break;
                case _8xy7:                        // 8xy7 : SUBN Vx, Vy
                    if(V[x] > V[y])	// VY-VX
                        V[0xF] = 0; // there is a borrow
                    else
                        V[0xF] = 1;
                    V[x] = V[y] - V[x];
                    pc += 2;
                    break;
                case _8xyE:                        // 8xyE : SHL Vx, Vy
                    V[0xF] = V[x] >> 7;
                    V[x] <<= 1;
                    pc += 2;
                    break;
                default:                           // invalid opcode
                    printf("\nUnknown opcode [0x8xy?]: %.4X\n", opcode);
            }
            break;
        case _9xy0:                                // 9xy0 : SNE Vx, Vy
            if (V[x] != V[y])
                pc += 4;
            else
                pc += 2;
            break;
        case _Annn:                                // Annn : LD I(ndex), addr
            index = addr;
            pc += 2;
            break;
        case _Bnnn:                                // Bnnn : JP V0, addr
            pc = (addr) + V[0];
            break;
        case _Cxkk:                                // Cxkk : RND Vx, byte
            V[x] = (rand() % (0xFF + 1)) & byte;
            pc += 2;
            break;
        case _Dxyn:                                // Dxyn : DRW Vx, Vy, nibble
        {
            uint16_t drawX    = V[x];
            uint16_t drawY    = V[y];
            uint16_t height   = mask;               // nibble mask
            uint16_t px;

            V[0xF] = 0;
            for (int i = 0; i < height; i++)
            {
                px = memory[index + i];
                for(int j = 0; j < 8; j++)
                {
                    if((px & (0x80 >> j)) != 0)
                    {
                        if(display[(drawX + j + ((drawY + i) * 64))] == 1)
                            V[0xF] = 1;
                        display[drawX + j + ((drawY + i) * 64)] ^= 1;
                    }
                }
            }
            drawFlag = true;
            pc += 2;
            break;
        }
        case _Ex__:                                // Ex??
            switch (byte) 
            {
                case _Ex9E:                        // Ex9E : SKP Vx
                    if (key[V[x]] != 0)
                        pc +=  4;
                    else
                        pc += 2;
                    break;
                case _ExA1:                        // ExA1 : SKNP Vx
                    if (key[V[x]] == 0)
                        pc +=  4;
                    else
                        pc += 2;
                    break;
                default:                           // invalid opcode
                    printf("\nUnknown opcode [0xEx??]: %.4X\n", opcode);
            }
            break;
        case _Fx__:                                // Fx??
            switch (byte)
            {
                case _Fx07:                        // Fx07 : LD Vx, DT
                    V[x] = delayTimer;
                    pc += 2;
                    break;
                case _Fx0A:                        // Fx0A : LD Vx, K(ey)
                {
                    bool key_pressed = false;
                    for(int i = 0; i < 16; ++i)
                    {
                        if(key[i] != 0)
                        {
                            V[x] = i;
                            key_pressed = true;
                        }
                    }
                    if(!key_pressed)
                        return;
                    pc += 2;
                }
                    break;
                case _Fx15:                        // Fx15 : LD D(elay)T(imer), Vx
                    delayTimer = V[x];
                    pc += 2;
                    break;
                case _Fx18:                        // Fx18 : LD S(ound)T(imer), Vx
                    soundTimer = V[x];
                    pc += 2;
                    break;
                case _Fx1E:                        // Fx1E : ADD I(ndex), Vx
                    if(index + V[x] > 0xFFF)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    index += V[x];
                    pc += 2;
                    break;
                case _Fx29:                        // Fx29 : LD F, Vx
                    index = V[x] * 0x5;
                    pc += 2;
                    break;
                case _Fx33:                        // Fx33 : LD B, Vx
                    memory[index]     = V[x] / 100;
                    memory[index + 1] = (V[x] / 10) % 10;
                    memory[index + 2] = V[x] % 10;
                    pc += 2;
                    break;
                case _Fx55:                        // Fx55 : LD [I(ndex)], Vx
                    for (int i = 0; i <= (x); ++i)
                        memory[index + i] = V[i];
                    index += (x) + 1;
                    pc += 2;
                    break;
                case _Fx65:                        // Fx65 : LD Vx, [I(ndex)]
                    for (int i = 0; i <= (x); ++i)
                        V[i] = memory[index + i];
                    index += (x) + 1;
                    pc += 2;
                    break;
                default:                            // invalid opcode
                    printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
            }
            break;
        default:                                    // invalid opcode
            printf("Invalid opcode: %.4X\n", opcode);
    }
    // updating timers
    if (delayTimer > 0)
        --delayTimer;
    if (soundTimer > 0)
        --soundTimer;
}