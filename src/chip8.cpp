#include "chip8.hpp"

Chip8::Chip8() : m_index(0), m_opcode(0), m_pc(0x200), m_sp(0) {}

Chip8::~Chip8() {}

void Chip8::reset() {
    // initializing vector sizes
    m_memory.resize(4096);                            
    display.resize(64 * 64);
    m_stack.resize(16);
    key.resize(16);
    m_V.resize(16);

    // resetting timers
    m_soundTimer = 0;
    m_delayTimer = 0;

    // loading fonts into memory
    for (int i = 0; i < 80; ++i)
        m_memory[i] = fontset[i];                     

    // rng
    srand(time(NULL));                             
}

void Chip8::handleOpcodeError(const char* opcodeStr, std::uint16_t opcodeVal) {
    std::cerr << "ERROR\t(chip8): Unknown opcode [" << opcodeStr << "]: " 
        << std::hex << std::uppercase << opcodeVal << "\n";
}

bool Chip8::loadROM(const char* path) {
    // initializing Chip8 and loading ROM from given path 
    reset();                                     
    std::ifstream file(path, std::ios::ate);         

    if (file.is_open()) {
        std::streampos fileSize = file.tellg();
        std::vector<char> buffer(fileSize);         

        // reading file from beginning 
        file.seekg(0, std::ios::beg);
        file.read(&buffer[0], fileSize);            // storing data within buffer
        file.close();

        for(long i = 0; i < fileSize; ++i)
            m_memory[0x200 + i] = buffer[i];        // loading buffer data to memory

        return true;
    }
    else {
        return false;
    }
}

// CPU cycles: fetch --> decode --> execute opcode
void Chip8::cycle() { 
    // fetching
    m_opcode  = m_memory[m_pc] << 8 | m_memory[m_pc + 1];

    mask    = m_opcode & 0x000F;

    byte    = m_opcode & 0x00FF;                    // aka "kk"
    addr    = m_opcode & 0x0FFF;                      

    x       = (m_opcode & 0x0F00) >> 8;
    y       = (m_opcode & 0x00F0) >> 4; 
    
    // decoding and executing
    switch(m_opcode & 0xF000) {
        case oc_00E_:                               // 00E? 
            switch (mask) { 
                case oc_00E0:                       // 00E0 (CLS) : clears display
                    CLS();
                    break;
                case oc_00EE:                       // 00EE (RET) : returns from subroutine
                     RET();
                    break;
                default:                            // invalid opcode
                    handleOpcodeError("[0x00E?]", m_opcode);
            } 
            break;
        case oc_1nnn:                               // 1nnn (JP) : jumps to addr
            JP_addr();
            break;
        case oc_2nnn:                               // 2nnn (CALL) : calls subroutine at addr
            CALL();
            break;
        case oc_3xkk:                               // 3xkk (SE) : skips next instruction if Vx = kk
            SE_Vx_byte();
            break;
        case oc_4xkk:                               // 4xkk (SNE) : skips next instruction if Vx != kk
            SNE_Vx_byte();
            break;
        case oc_5xy0:                               // 5xy0 (SE) : skips next instruction of Vx = Vy
            SE_VxVy();
            break;
        case oc_6xkk:                               // 6xkk (LD) : puts value of kk into register Vx 
            LD_Vx_byte();
            break;
        case oc_7xkk:                               // 7xkk (ADD) : adds kk to Vx, stores result in Vx
            ADD_Vx_byte();
            break;
        case oc_8xy_:                               // 8xy?
            switch (mask) {
                case oc_8xy0:                       // 8xy0 (LD) : stores value of Vy in Vx
                    LD_VxVy();
                    break;
                case oc_8xy1:                       // 8xy1 (OR) : performs OR on Vx and Vy, stores result in Vx
                    OR();
                    break;
                case oc_8xy2:                       // 8xy2 (AND) : performs AND on Vx and Vy, stores result in Vx 
                    AND();
                    break;
                case oc_8xy3:                       // 8xy3 (XOR) : performs XOR on Vx and Vy, stores result in Vx
                    XOR();
                    break;
                case oc_8xy4:                       // 8xy4 (ADD) : adds Vx and Vy. if result > 8 bits, VF=1, else VF=0.
                    ADD_VxVy();                     //              only lowest 8 bits of result are stored in Vx
                    break;
                case oc_8xy5:                       // 8xy5 (SUB) : Vx -= Vy. results are stored in Vx. 
                    SUB();                          //              then if Vx > Vy, VF=1, else VF=0.
                    break;
                case oc_8xy6:                       // 8xy6 (SHR) : if LSB of Vx=1, VF=1, else VF=0. then shift Vx right
                    SHR();                          //              by 1 (div by 2)
                    break;
                case oc_8xy7:                       // 8xy7 (SUBN) : Vx =- Vy. results are stored in Vx.
                    SUBN();                         //               then if Vx > Vy, VF=1, else VF=0.
                    break;
                case oc_8xyE:                       // 8xyE (SHL) : if MSB of Vx=1, VF=1, else VF=0. then shift Vx left
                    SHL();                          //              by 1 (mul by 2)
                    break;
                default:                            // invalid opcode
                    handleOpcodeError("[0x8xy?]", m_opcode);
                    return;
            }
            break;
        case oc_9xy0:                               // 9xy0 (SNE) : skip next instruction if Vx != Vy
            SNE_VxVy();
            break;
        case oc_Annn:                               // Annn (LD) : set I = nnn
            LD_I_addr();
            break;
        case oc_Bnnn:                               // Bnnn (JP) : jump to location nnn + V0
            JP_addrV0();
            break;
        case oc_Cxkk:                               // Cxkk (RND) : set Vx = random byte & kk
            RND();
            break;
        case oc_Dxyn:                               // Dxyn (DRW) : display n-byte sprite starting from 
            DRW();                                  //              memory location I at (Vx, Vy), set
            break;                                  //              VF = collision
        case oc_Ex__:                               // Ex??
            switch (byte) {
                case oc_Ex9E:                       // Ex9E (SKP) : skip next instruction if key with value of Vx is pressed
                    SKP();
                    break;
                case oc_ExA1:                       // ExA1 (SKNP) : skip next instruction if key with value of Vx isnt pressed
                    SKNP();
                    break;
                default:                            // invalid opcode
                    handleOpcodeError("[0xEx?]", m_opcode);
                    return;
            }
            break;
        case oc_Fx__:                               // Fx??
            switch (byte) {
                case oc_Fx07:                       // Fx07 (LD) : set Vx to the value of the delay timer
                    LD_Vx_t();
                    break;
                case oc_Fx0A:                       // Fx0A (LD) : wait for a key press, store the value into Vx
                    LD_Vx_k();
                    break;
                case oc_Fx15:                       // Fx15 (LD) : set delay timer to the value of Vx
                    LD_t_Vx(m_delayTimer);
                    break;
                case oc_Fx18:                       // Fx18 (LD) : set sound timer to the value of Vx
                    LD_t_Vx(m_soundTimer);
                    break;
                case oc_Fx1E:                       // Fx1E (ADD) : add Vx to I
                    ADD_I_Vx();
                    break;
                case oc_Fx29:                       // Fx29 (LD) : set I to location of sprite for digit Vx
                    LD_F_Vx();
                    break;
                case oc_Fx33:                       // Fx33  (LD) : store BCD representation of Vx in I, I+1 and I+2
                    LD_BCD();
                    break;
                case oc_Fx55:                       // Fx55 (LD) : store registers V0-Vx in memory starting at location I
                    LD_wVF();
                    break;
                case oc_Fx65:                       // Fx65 (LD) : read registers V0-Vx from memory starting at location I
                    LD_rVF();
                    break;
                default:                            // invalid opcode
                    handleOpcodeError("[0xF000]", m_opcode);
                    return;
            }
            break;
        default:                                    // invalid opcode, starts with val outside of hex range 0-F
            handleOpcodeError("[0x?000]", m_opcode);
            return;
    }

    // updating timers 
    if (m_delayTimer > 0)
        --m_delayTimer;

    if (m_soundTimer > 0)
        --m_soundTimer;
}

// 00E0
void Chip8::CLS() {
    for (int i = 0; i < 2048; ++i) 
        display[i] = 0;

    drawFlag = true;
    m_pc += 2;
}                                                   

// 00EE
void Chip8::RET() {
    --m_sp;
    m_pc = m_stack[m_sp];
    m_pc += 2;
}                                         

// 1nnn
void Chip8::JP_addr() { 
    m_pc = addr; 
}                                                        

// 2nnn
void Chip8::CALL() {
    m_stack[m_sp] = m_pc;
    ++m_sp;
    m_pc = addr;
}                                                        

// 3xkk
void Chip8::SE_Vx_byte() { 
    m_pc += (m_V[x] == byte) ? 4 : 2; 
}                                            

// 4xkk
void Chip8::SNE_Vx_byte() { 
    m_pc += (m_V[x] != byte) ? 4 : 2; 
}                                           

// 5xy0
void Chip8::SE_VxVy() { 
    m_pc += (m_V[x] == m_V[y]) ? 4 : 2; 
}                                              

// 6xkk
void Chip8::LD_Vx_byte() { 
    m_V[x] = byte; m_pc += 2; 
}                                                         

// 7xkk
void Chip8::ADD_Vx_byte() { 
    m_V[x] += byte; m_pc += 2; 
}                                                        

// 8xy0
void Chip8::LD_VxVy() { 
    m_V[x] = m_V[y]; m_pc += 2; 
}                                                         

// 8xy1
void Chip8::OR() { 
    m_V[x] |= m_V[y]; m_pc += 2; 
}                                                         

// 8xy2
void Chip8::AND() { 
    m_V[x] &= m_V[y]; m_pc += 2; 
}                                                         

// 8xy3
void Chip8::XOR() { 
    m_V[x] ^= m_V[y]; m_pc += 2; 
}                                                          

// 8xy4
void Chip8::ADD_VxVy() 
{
    m_V[x] += m_V[y];                   
    m_V[0xF] =  (m_V[ y] > 0x00F) ? 1 : 0;
    m_pc += 2;
}                                

// 8xy5
void Chip8::SUB() 
{
    m_V[x] = m_V[x] - m_V[y];            
    m_V[0xF] =  (m_V[ x] > m_V[y]) ? 1 : 0;
    m_pc += 2; 
}                                

// 8xy6
void Chip8::SHR() 
{
    m_V[0xF] =  (m_V[ x] & 0x01);
    if (m_V[ x] >> 1)
        m_V[x] >>= 1;

    m_pc += 2; 
}                                

// 8xy7
void Chip8::SUBN() 
{
    m_V[x] = m_V[y] - m_V[x];             
    m_V[0xF] =  (m_V[ x] > m_V[y]) ? 0 : 1;
    m_pc += 2;  
}                               

// 8xyE
void Chip8::SHL() 
{
    m_V[x] <<= 1;
    m_V[0xF] =  (m_V[ x] >> 7);
    m_pc += 2; 
}                                             

// 9xy0
void Chip8::SNE_VxVy() { 
    m_pc +=  (m_V[ x] != m_V[y]) ? 4 : 2; 
}                                            

// Annn
void Chip8::LD_I_addr() { 
    m_index = addr; m_pc += 2; 
}                                                     

// Bnnn
void Chip8::JP_addrV0() { 
    m_pc = (addr) + m_V[0]; 
}                               

// Cxkk
void Chip8::RND() { 
    m_V[x] = (rand() % (0xFF + 1)) & byte; m_pc += 2; 
}                                                       

// Dxyn
void Chip8::DRW() {
    uint16_t xPos = m_V[x] % 64;           
    uint16_t yPos = m_V[y] % 32;
    uint16_t px;

    m_V[0xF] = 0;
    for (int i = 0; i < mask; ++i) {
        px = m_memory[m_index + i];             
        for(int j = 0; j < 8; ++j) {
            if((px & (0x80 >> j)) != 0) {
                if(display[(xPos + j + ((yPos + i) * 64))] == 1)
                    m_V[0xF] = 1;

                display[xPos + j + ((yPos + i) * 64)] ^= 1;
            }
        }
    }
    drawFlag = true;
    m_pc += 2; 
} 

// Ex9E
void Chip8::SKP() { 
    m_pc += (key[m_V[x]] != 0) ? 4 : 2; 
}                  

// ExA1
void Chip8::SKNP() {
    if (key[m_V[x]] == 0)
        m_pc += 4;
    else
        m_pc += 2;
}                 

// Fx07
void Chip8::LD_Vx_t() { 
    m_V[x] = m_delayTimer; m_pc += 2; 
}                                                           

// Fx0A
void Chip8::LD_Vx_k() {
    bool key_pressed = false;
    for(int i = 0; i < 16; ++i) {
        if(key[i] != 0) {
            m_V[x] = i;
            key_pressed = true;
        }
    }

    if(!key_pressed)    
        return;

    m_pc += 2; 
}                                                          

// Fx15 & Fx18
void Chip8::LD_t_Vx(std::uint8_t& timer) { 
    timer = m_V[x]; m_pc += 2; 
}                                                           

// Fx1E        
void Chip8::ADD_I_Vx() {
    if(m_index + m_V[x] > 0xFFF)    
        m_V[0xF] = 1;
    else 
        m_V[0xF] = 0;

    m_index += m_V[x];
    m_pc += 2; 
}            

// Fx29
void Chip8::LD_F_Vx() { 
    m_index = m_V[x] * 0x5; m_pc += 2; 
}            

// Fx33
void Chip8::LD_BCD() {
    m_memory[m_index]     = m_V[x] / 100;
    m_memory[m_index + 1] = (m_V[ x] / 10) % 10;
    m_memory[m_index + 2] = m_V[x] % 10;
    m_pc += 2; 
}                  

// Fx55 (write)
void Chip8::LD_wVF() {
    for (int i = 0; i <= x; ++i) 
        m_memory[m_index + i] = m_V[i];

    m_index += x + 1;
    m_pc += 2; 
}            

// Fx65 (read)
void Chip8::LD_rVF() {
    for (int i = 0; i <= x; ++i)
        m_V[i] = m_memory[m_index + i];
  
    m_index += x + 1;
    m_pc += 2; 
} 