#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#ifndef CHIP8_H
#define CHIP8_H

class Chip8
{
public:
    Chip8();
    ~Chip8();
    
    void printError(std::string range, uint16_t opcode);
    void cycle();
    bool loadROM(const char* ROM);
    
    std::vector<uint8_t> display;
    std::vector<uint8_t> key;

    uint16_t mask;              // nibble - opcode & 0x000F         
    uint16_t byte;              // kk     - opcode & 0x00FF         
    uint16_t addr;              // addr   - opcode & 0x0FFF         
    uint16_t x;                 // (opcode & 0x0F00) >> 8  
    uint16_t y;                 // (opcode & 0x00F0) >> 4  

    bool drawFlag;

private:
    void reset();           

    // Instructions
    void CLS();                 // 00E0 - CLS
    void RET();                 // 00EE - RET
    void JP_addr();             // 1nnn - JP
    void CALL();                // 2nnn - CALL
    void SE_Vx_byte();          // 3xkk - SE Vx Vy
    void SNE_Vx_byte();         // 4xkk - SNE Vx byte
    void SE_VxVy();             // 5xy0 - SE Vx Vy
    void LD_Vx_byte();          // 6xkk - LD Vx byte
    void ADD_Vx_byte();         // 7xkk - ADD Vx byte
    void LD_VxVy();             // 8xy0 - LD Vx Vy
    void OR();                  // 8xy1 - OR Vx Vy
    void AND();                 // 8xy2 - AND Vx Vy
    void XOR();                 // 8xy3 - XOR Vx Vy
    void ADD_VxVy();            // 8xy4 - ADD Vx Vy
    void SUB();                 // 8xy5 - SUB Vx Vy
    void SHR();                 // 8xy6 - SHR Vx { Vy}
    void SUBN();                // 8xy7 - SUBN Vx Vy
    void SHL();                 // 8xyE - SHL Vx { Vy}
    void SNE_VxVy();            // 9xy0 - SNE Vx Vy
    void LD_I_addr();           // Annn - LD I addr   
    void JP_addrV0();           // Bnnn - JP V0 addr
    void RND();                 // Cxkk - RND Vx byte
    void DRW();                 // Dxyn - DRW Vx Vy nibble
    void SKP();                 // Ex9E - SKP Vx
    void SKNP();                // ExA1 - SKNP Vx 
    void LD_Vx_t();             // Fx07 - LD Vx DT
    void LD_Vx_k();             // Fx0A - LD Vx k
    void LD_t_Vx(uint8_t& t);   // Fx15 & Fx18 - LD DT/ST Vx
    void ADD_I_Vx();            // Fx1E - ADD I Vx
    void LD_F_Vx();             // Fx29 - LD F Vx
    void LD_BCD();              // Fx33 - LD B Vx (Vx BCD)
    void LD_wVF();              // Fx55 - LD [I] Vx (write)
    void LD_rVF();              // Fx65 - LD Vx. [I] (read)

    std::vector<uint8_t> V;
    std::vector<uint16_t> stack;
    std::vector<uint8_t> memory;

    uint16_t opcode;
    uint16_t pc;
    uint16_t index;
    uint16_t sp;
    
    uint8_t delayTimer;
    uint8_t soundTimer;

    enum opcodes                  // Instruction opcodes
    {
        oc_00E_    =   0x0000,    // 00E?
        oc_00E0    =   0x0000,    // 00E0 : CLS
        oc_00EE    =   0x000E,    // 00EE : RET
        oc_1nnn    =   0x1000,    // 1nnn : JMP addr
        oc_2nnn    =   0x2000,    // 2nnn : CALL addr
        oc_3xkk    =   0x3000,    // 3xkk : SE Vx, byte
        oc_4xkk    =   0x4000,    // 4xkk : SNE Vx, byte
        oc_5xy0    =   0x5000,    // 5xy0 : SE Vx, Vy
        oc_6xkk    =   0x6000,    // 6xkk : LD Vx, byte
        oc_7xkk    =   0x7000,    // 7xkk : ADD Vx, byte
        oc_8xy_    =   0x8000,    // 8xy?
        oc_8xy0    =   0x0000,    // 8xy0 : LD Vx, Vy
        oc_8xy1    =   0x0001,    // 8xy1 : OR Vx, Vy
        oc_8xy2    =   0x0002,    // 8xy2 : AND Vx, Vy 
        oc_8xy3    =   0x0003,    // 8xy3 : XOR Vx, Vy
        oc_8xy4    =   0x0004,    // 8xy4 : ADD Vx, Vy
        oc_8xy5    =   0x0005,    // 8xy5 : SUB Vx, Vy
        oc_8xy6    =   0x0006,    // 8xy6 : SHR Vx {, Vy}
        oc_8xy7    =   0x0007,    // 8xy7 : SUBN Vx, Vy
        oc_8xyE    =   0x000E,    // 8xyE : SHL Vx, Vy
        oc_9xy0    =   0x9000,    // 9xy0 : SNE Vx, Vy
        oc_Annn    =   0xA000,    // Annn : LD I, addr
        oc_Bnnn    =   0xB000,    // Bnnn : JP V0, addr
        oc_Cxkk    =   0xC000,    // Cxkk : RND Vx, byte
        oc_Dxyn    =   0xD000,    // Dxyn : DRW Vx, Vy, nibble
        oc_Ex__    =   0xE000,    // Ex??
        oc_Ex9E    =   0x009E,    // Ex9E : SKP Vx
        oc_ExA1    =   0x00A1,    // ExA1 : SKNP Vx
        oc_Fx__    =   0xF000,    // Fx??
        oc_Fx07    =   0x0007,    // Fx07 : LD Vx, DT
        oc_Fx0A    =   0x000A,    // Fx0A : LD Vx, K
        oc_Fx15    =   0x0015,    // Fx15 : LD DT, Vx
        oc_Fx18    =   0x0018,    // Fx18 : LD ST, Vx
        oc_Fx1E    =   0x001E,    // Fx1E : ADD I, Vx
        oc_Fx29    =   0x0029,    // Fx29 : LD F, Vx
        oc_Fx33    =   0x0033,    // Fx33 : LD B, Vx
        oc_Fx55    =   0x0055,    // Fx55 : LD [I], Vx
        oc_Fx65    =   0x0065,    // Fx65 : LD Vx, [I]
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
};

#endif