#include "Chip8.h"

Chip8::Chip8() : pc(0x200), opcode(0), index(0), sp(0){}

Chip8::~Chip8(){}

void Chip8::reset()
{
    memory.resize(4096);       // initializing vector sizes
    display.resize(64 * 64);
    stack.resize(16);
    key.resize(16);
    V.resize(16);

    std::fill(memory.begin(), memory.end(), 0);     // clearing memory
    std::fill(display.begin(), display.end(), 0);   // toggling all pixels off
    std::fill(stack.begin(), stack.end(), 0);       // emptying stack
    std::fill(key.begin(), key.end(), 0);           // resetting keypad inputs
    std::fill(V.begin(), V.end(), 0);               // clearing registers V0-VF

    // resetting timers
    soundTimer = 0;
    delayTimer = 0;

    for (int i = 0; i < 80; ++i)
        memory[i] = fontset[i];     // loading fonts into memory

    srand(time(NULL));     // rng
}

bool Chip8::loadROM(const char* ROM)
{
    reset();   // initializing data

    std::ifstream f(ROM, std::ios::ate);        // seeking EOF

    if (f.is_open())
    {
        std::streampos fileSize = f.tellg();    // getting file's size
        char* buf = new char[fileSize];         // file buffer

        // reading file from beginning to end
        f.seekg(0, std::ios::beg);
        f.read(buf, fileSize);                  // storing data within buffer
        f.close();

        for(long i = 0; i < fileSize; ++i)
            memory[0x200 + i] = buf[i];         // loading buffer data to memory

        delete[] buf;                           // freeing buffer memory
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
    opcode  = memory[pc] << 8 | memory[pc + 1];

    mask    = oc & 0x000F;
    byte    = oc & 0x00FF;
    addr    = oc & 0x0FFF;
    x       = (oc & 0x0F00) >> 8;
    y       = (oc & 0x00F0) >> 4; 
    
    // decode
    switch(opcode & 0xF000)
    {
        case oc_00E_:                               // 00E?
            switch (mask) 
            {
                case oc_00E0:                       // 00E0 : clears display
                    for (int i = 0; i < 2048; ++i) 
                        display[i] = 0;            
                    drawFlag = true;
                    pc += 2;
                    break;
                case oc_00EE:                       // 00EE : returns from subroutine
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;
                default:                            // invalid opcode
                    printf("Unknown opcode [0x00E?]: %.4X\n", opcode);
                    return;
            } 
            break;
        case oc_1nnn:                               // 1nnn : jumps to addr
            pc = addr;
            break;
        case oc_2nnn:                               // 2nnn : calls subroutine at addr
            stack[sp] = pc;
            ++sp;
            pc = addr;
            break;
        case oc_3xkk:                               // 3xkk : skips next instruction if Vx = kk
            pc += (V[x] == byte) ? 4 : 2;
            break;
        case oc_4xkk:                               // 4xkk : skips next instruction if Vx != kk
            pc += (V[x] != byte) ? 4 : 2;
            break;
        case oc_5xy0:                               // 5xy0 : skips next instruction of Vx = Vy
            pc += (V[x] == V[y]) ? 4 : 2;
            break;
        case oc_6xkk:                               // 6xkk : puts value of kk into register Vx 
            V[x] = byte;
            pc += 2;
            break;
        case oc_7xkk:                               // 7xkk : adds kk to Vx, stores result in Vx
            V[x] += byte;
            pc += 2;
            break;
        case oc_8xy_:                               // 8xy?
            switch (mask) 
            {
                case oc_8xy0:                       // 8xy0 : stores value of Vy in Vx
                    V[x] = V[y];
                    pc += 2;
                    break;
                case oc_8xy1:                       // 8xy1 : performs OR on Vx and Vy, stores result in Vx
                    V[x] |= V[y];
                    pc += 2;
                    break;
                case oc_8xy2:                       // 8xy2 : performs AND on Vx and Vy, stores result in Vx 
                    V[x] &= V[y];
                    pc += 2;
                    break;
                case oc_8xy3:                       // 8xy3 : performs XOR on Vx and Vy, stores result in Vx
                    V[x] ^= V[y];
                    pc += 2;
                    break;
                case oc_8xy4:                       // 8xy4 : adds Vx and Vy. if result > 8 bits, VF=1, else VF=0.
                    V[x] += V[y];                   //        only lowest 8 bits of result are stored in Vx
                    V[0xF] = (V[y] > 0x00F) ?
                                1 : 0;
                    pc += 2;
                    break;
                case oc_8xy5:                       // 8xy5 : Vx -= Vy. results are stored in Vx. 
                    V[x] = V[x] - V[y];             //        then if Vx > Vy, VF=1, else VF=0.
                    V[0xF] = (V[x] > V[y]) ?       
                             1 : 0;
                    pc += 2;
                    break;
                case oc_8xy6:                       // 8xy6 : if LSB of Vx=1, VF=1, else VF=0. then shift Vx right by 1 (div by 2)
                    V[0xF] = (V[x] & 0x01);
                    if(V[x] >> 1)
                       V[x] >>= 1;
                    pc += 2;
                    break;
                case oc_8xy7:                       // 8xy7 : Vx =- Vy. results are stored in Vx.
                    V[x] = V[y] - V[x];             //        then if Vx > Vy, VF=1, else VF=0.
                    V[0xF] = (V[x] > V[y]) ?       
                             0 : 1;
                    pc += 2;
                    break;
                case oc_8xyE:                       // 8xyE : if MSB of Vx=1, VF=1, else VF=0. then shift Vx left by 1 (mul by 2)
                    V[x] <<= 1;
                    V[0xF] = (V[x] >> 7);
                    pc += 2;
                    break;
                default:                            // invalid opcode
                    printf("\nUnknown opcode [0x8xy?]: %.4X\n", opcode);
            }
            break;
        case oc_9xy0:                               // 9xy0 : skip next instruction if Vx != Vy
             pc += (V[x] != V[y]) ? 4 : 2;
            break;
        case oc_Annn:                               // Annn : set I = nnn
            index = addr;
            pc += 2;
            break;
        case oc_Bnnn:                               // Bnnn : jump to location nnn + V0
            pc = (addr) + V[0];
            break;
        case oc_Cxkk:                               // Cxkk : set Vx = random byte & kk
            V[x] = (rand() % (0xFF + 1)) & byte;
            pc += 2;
            break;
        case oc_Dxyn:                               // Dxyn : display n-byte sprite starting from 
        {                                           //        memory location I at (Vx, Vy), set
            uint16_t xPos    = V[x] % 64;           //        VF = collision
            uint16_t yPos    = V[y] % 32;
            uint16_t px;

            V[0xF] = 0;
            for (int i = 0; i < mask; ++i)          // sprite row
            {
                px = memory[index + i];             // pixel in sprite row 
                for(int j = 0; j < 8; ++j)          // MSB --> LSB, i.e left to right
                {
                    if((px & (0x80 >> j)) != 0)     
                    {
                        if(display[(xPos + j + ((yPos + i) * 64))] == 1) 
                            V[0xF] = 1;
                        display[xPos + j + ((yPos + i) * 64)] ^= 1;
                    }
                }
            }
            drawFlag = true;
            pc += 2;
        }
            break;

        case oc_Ex__:                               // Ex??
            switch (byte) 
            {
                case oc_Ex9E:                       // Ex9E : skip next instruction if key with value of Vx is pressed
                    pc += (key[V[x]] != 0) ? 4 : 2;
                    break;
                case oc_ExA1:                       // ExA1 : skip next instruction if key with value of Vx isnt pressed
                    if (key[V[x]] == 0)
                        pc +=  4;
                    else
                        pc += 2;
                    break;
                default:                            // invalid opcode
                    printf("\nUnknown opcode [0xEx??]: %.4X\n", opcode);
            }
            break;
        case oc_Fx__:                               // Fx??
            switch (byte)
            {
                case oc_Fx07:                       // Fx07 : set Vx to the value of the delay timer
                    V[x] = delayTimer;
                    pc += 2;
                    break;
                case oc_Fx0A:                       // Fx0A : wait for a key press, store the value into Vx
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
                case oc_Fx15:                       // Fx15 : set delay timer to the value of Vx
                    delayTimer = V[x];
                    pc += 2;
                    break;
                case oc_Fx18:                       // Fx18 : set sound timer to the value of Vx
                    soundTimer = V[x];
                    pc += 2;
                    break;
                case oc_Fx1E:                       // Fx1E : add Vx to I
                    if(index + V[x] > 0xFFF)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    index += V[x];
                    pc += 2;
                    break;
                case oc_Fx29:                       // Fx29 : set I to location of sprite for digit Vx
                    index = V[x] * 0x5;
                    pc += 2;
                    break;
                case oc_Fx33:                       // Fx33  : store BCD representation of Vx in I, I+1 and I+2
                    memory[index]     = V[x] / 100;
                    memory[index + 1] = (V[x] / 10) % 10;
                    memory[index + 2] = V[x] % 10;
                    pc += 2;
                    break;
                case oc_Fx55:                       // Fx55 : store registers V0-Vx in memory starting at location I
                    for (int i = 0; i <= x; ++i)
                        memory[index + i] = V[i];
                    index += x + 1;
                    pc += 2;
                    break;
                case oc_Fx65:                       // Fx65 : read registers V0-Vx from memory starting at location I
                    for (int i = 0; i <= x; ++i)
                        V[i] = memory[index + i];
                    index += x + 1;
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
