#include "Chip8.h"

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
                case _00E0:                        // 00E0 : clears display
                    for (int i = 0; i < 2048; ++i) 
                        display[i] = 0;            
                    drawFlag = true;
                    pc += 2;
                    break;
                case _00EE:                        // 00EE : returns from subroutine
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;
                default:                           // invalid opcode
                    printf("Unknown opcode [0x00E?]: %.4X\n", opcode);
                    return;
            } 
            break;
        case _1nnn:                                // 1nnn : jumps to addr
            pc = addr;
            break;
        case _2nnn:                                // 2nnn : calls subroutine at addr
            stack[sp] = pc;
            ++sp;
            pc = addr;
            break;
        case _3xkk:                                // 3xkk : skips next instruction if Vx = kk
            if (V[x] == byte)
                pc += 4;
            else
                pc += 2;
            break;
        case _4xkk:                                // 4xkk : skips next instruction if Vx != kk
            if (V[x] != byte)
                pc += 4;
            else
                pc += 2;
            break;
        case _5xy0:                                // 5xy0 : skips next instruction of Vx = Vy
            if (V[x] == V[y])
                pc += 4;
            else
                pc += 2;
            break;
        case _6xkk:                                // 6xkk : puts value of kk into register Vx 
            V[x] = byte;
            pc += 2;
            break;
        case _7xkk:                                // 7xkk : adds kk to Vx, stores result in Vx
            V[x] += byte;
            pc += 2;
            break;
        case _8xy_:                                // 8xy?
            switch (mask) 
            {
                case _8xy0:                        // 8xy0 : stores value of Vy in Vx
                    V[x] = V[y];
                    pc += 2;
                    break;
                case _8xy1:                        // 8xy1 : performs OR on Vx and Vy, stores result in Vx
                    V[x] |= V[y];
                    pc += 2;
                    break;
                case _8xy2:                        // 8xy2 : performs AND on Vx and Vy, stores result in Vx 
                    V[x] &= V[y];
                    pc += 2;
                    break;
                case _8xy3:                        // 8xy3 : performs XOR on Vx and Vy, stores result in Vx
                    V[x] ^= V[y];
                    pc += 2;
                    break;
                case _8xy4:                        // 8xy4 : adds Vx and Vy. if result > 8 bits, VF=1, else VF=0.
                    V[x] += V[y];                  //        only lowest 8 bits of result are stored in Vx
                    if(V[y] > (0xFF - V[x]))
                        V[0xF] = 1; //carry
                    else
                        V[0xF] = 0;
                    pc += 2;
                    break;
                case _8xy5:                        // 8xy5 : if Vx > Vy, VF=1, else VF=0. then Vx -= Vy and results
                    if(V[y] > V[x])                //        are stored in Vx
                        V[0xF] = 0; // borrow
                    else
                        V[0xF] = 1;
                    V[x] -= V[y];
                    pc += 2;
                    break;
                case _8xy6:                        // 8xy6 : if LSB of Vx=1, VF=1, else VF=0. then shift Vx right by 1 (div by 2)
                    V[0xF] = V[x] & 0x1;
                    V[x] >>= 1;
                    pc += 2;
                    break;
                case _8xy7:                        // 8xy7 : if Vy > Vx, VF=1, else VF=0. then Vx = Vy - Vx and results
                    if(V[x] > V[y])	               //        are stored in Vx
                        V[0xF] = 0; // borrow
                    else
                        V[0xF] = 1;
                    V[x] = V[y] - V[x];
                    pc += 2;
                    break;
                case _8xyE:                        // 8xyE : if MSB of Vx=1, VF=1, else VF=0. then shift Vx left by 1 (mul by 2)
                    V[0xF] = V[x] >> 7;
                    V[x] <<= 1;
                    pc += 2;
                    break;
                default:                           // invalid opcode
                    printf("\nUnknown opcode [0x8xy?]: %.4X\n", opcode);
            }
            break;
        case _9xy0:                                // 9xy0 : skip next instruction if Vx != Vy
            if (V[x] != V[y])
                pc += 4;
            else
                pc += 2;
            break;
        case _Annn:                                // Annn : set I = nnn
            index = addr;
            pc += 2;
            break;
        case _Bnnn:                                // Bnnn : jump to location nnn + V0
            pc = (addr) + V[0];
            break;
        case _Cxkk:                                // Cxkk : set Vx = random byte & kk
            V[x] = (rand() % (0xFF + 1)) & byte;
            pc += 2;
            break;
        case _Dxyn:                                // Dxyn : display n-byte sprite starting from 
        {                                          //        memory location I at (Vx, Vy), set
            uint16_t drawX    = V[x];              //        VF = collision
            uint16_t drawY    = V[y];
            uint16_t height   = mask;              // nibble mask
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
                case _Ex9E:                        // Ex9E : skip next instruction if key with value of Vx is pressed
                    if (key[V[x]] != 0)
                        pc +=  4;
                    else
                        pc += 2;
                    break;
                case _ExA1:                        // ExA1 : skip next instruction if key with value of Vx isnt pressed
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
                case _Fx07:                        // Fx07 : set Vx to the value of the delay timer
                    V[x] = delayTimer;
                    pc += 2;
                    break;
                case _Fx0A:                        // Fx0A : wait for a key press, store the value into Vx
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
                case _Fx15:                        // Fx15 : set delay timer to the value of Vx
                    delayTimer = V[x];
                    pc += 2;
                    break;
                case _Fx18:                        // Fx18 : set sound timer to the value of Vx
                    soundTimer = V[x];
                    pc += 2;
                    break;
                case _Fx1E:                        // Fx1E : add Vx to I
                    if(index + V[x] > 0xFFF)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    index += V[x];
                    pc += 2;
                    break;
                case _Fx29:                        // Fx29 : set I to location of sprite for digit Vx
                    index = V[x] * 0x5;
                    pc += 2;
                    break;
                case _Fx33:                        // Fx33  store BCD representation of Vx in I, I+1 and I+2
                    memory[index]     = V[x] / 100;
                    memory[index + 1] = (V[x] / 10) % 10;
                    memory[index + 2] = V[x] % 10;
                    pc += 2;
                    break;
                case _Fx55:                        // Fx55 : store registers V0-Vx in memory starting at location I
                    for (int i = 0; i <= (x); ++i)
                        memory[index + i] = V[i];
                    index += (x) + 1;
                    pc += 2;
                    break;
                case _Fx65:                        // Fx65 : read registers V0-Vx from memory starting at location I
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