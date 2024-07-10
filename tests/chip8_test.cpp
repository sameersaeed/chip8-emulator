#include "chip8.hpp"
#include <gtest/gtest.h>

class Chip8Tests : public ::testing::Test {
protected:
    Chip8 chip8;

    void SetUp() override {
        chip8.reset(); 
    }
};

class GTestOut : public std::stringstream {
public:
    ~GTestOut() {
        // reference: https://stackoverflow.com/a/29155677
        std::cout << "\u001b[32m[   INFO   ] \u001b[33m" << str() << "\u001b[0m" << std::flush << "\n";
    }
};

#define GTCOUT GTestOut()

// CLS - clear screen (opcode 0x00E0)
TEST_F(Chip8Tests, Test_CLS) {
    for (int i = 0; i < 2048; ++i)          // draw on all pixels
        chip8.display[i] = 1;
    EXPECT_EQ(chip8.drawFlag, false); 
    int pcBefore = chip8.m_pc;

    GTCOUT << "set all pixels to 1, i.e., drew to all pixels on screen";
    chip8.CLS();
    GTCOUT << "CLS should clear screen so no pixels are visible on it";

    for (const auto& pixel : chip8.display) // all pixels should now be set to 0
        EXPECT_EQ(pixel, 0);
    EXPECT_EQ(chip8.drawFlag, true); 
    EXPECT_EQ(chip8.m_pc, pcBefore + 2);
}

// RET - return from subroutine (opcode 0x00EE)
TEST_F(Chip8Tests, Test_RET) {
    chip8.m_sp = 0x003;
    chip8.m_stack[chip8.m_sp] = 0x001;
    chip8.m_pc = 0x221;

    int pcBefore = chip8.m_pc;

    GTCOUT << "stack at original index (SP-1) should point to current PC: " << chip8.m_pc;
    GTCOUT << "stack pointer (SP) should decrement by 1, from " << chip8.m_sp << " to " << chip8.m_sp - 1;
    GTCOUT << "PC should jump to the address stored in the stack";

    chip8.RET();

    EXPECT_EQ(chip8.m_sp, 0x002); // SP should have decremented by 1
    EXPECT_EQ(chip8.m_pc - 2, chip8.m_stack[chip8.m_sp]); // stack should point to prev. PC
}

// JP_addr - jump to address (opcode 0x1nnn)
TEST_F(Chip8Tests, Test_JP_addr) {
    chip8.m_pc = 0x023;
    GTCOUT << "the PC is currently equal to: " << chip8.m_pc;

    chip8.addr = 0x201;
    GTCOUT << "the current address is: " << chip8.addr;
    GTCOUT << "so, the PC should jump to a value of: " << chip8.addr;

    chip8.JP_addr();
    EXPECT_EQ(chip8.m_pc, chip8.addr);
}

// CALL - call subroutine (opcode 0x2nnn)
TEST_F(Chip8Tests, Test_CALL) {
    chip8.m_sp = 0x003;
    chip8.m_stack[chip8.m_sp] = 0x001;
    chip8.m_pc = 0x221;
    chip8.addr = 0x321;

    int pcBefore = chip8.m_pc;
    

    GTCOUT << "stack at original index (SP-1) should point to current PC: " << chip8.m_pc;
    GTCOUT << "stack pointer (SP) should increment by 1, from " << chip8.m_sp << " to " << chip8.m_sp + 1;
    GTCOUT << "PC should jump to address: " << chip8.addr;

    chip8.CALL();

    EXPECT_EQ(chip8.m_sp, 0x004); // should have incremented by 1
    EXPECT_EQ(chip8.m_stack[chip8.m_sp - 1], pcBefore); // should point to prev. PC
    EXPECT_EQ(chip8.m_pc, chip8.addr); // PC val should have jumped to addr
}

// SE_Vx_byte - skip next instruction if Vx equals byte (opcode 0x3xkk)
TEST_F(Chip8Tests, Test_SE_Vx_byte) {
    chip8.m_V[0] = 0x0A;
    chip8.byte = 0x0A;
    chip8.m_pc = 0x201;

    int pcBefore = chip8.m_pc;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "byte is currently equal to: " << chip8.byte;
    GTCOUT << "PC is currently equal to: " << chip8.m_pc;

    GTCOUT << "since V0 is equal to byte, the next instruction should be skipped";
    chip8.SE_Vx_byte();

    EXPECT_EQ(chip8.m_pc, pcBefore + 4);
}

// SNE_Vx_byte - skip next instruction if Vx does not equal byte (opcode 0x4xkk)
TEST_F(Chip8Tests, Test_SNE_Vx_byte) {
    chip8.m_V[0] = 0x0A;
    chip8.byte = 0x0B;
    chip8.m_pc = 0x201;

    int pcBefore = chip8.m_pc;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "byte is currently equal to: " << chip8.byte;
    GTCOUT << "PC is currently equal to: " << chip8.m_pc;

    GTCOUT << "since V0 is not equal to byte, the next instruction should not be skipped";
    chip8.SNE_Vx_byte();

    EXPECT_EQ(chip8.m_pc, pcBefore + 4);
}

// SE_VxVy - skip next instruction if Vx equals Vy (opcode 0x5xy0)
TEST_F(Chip8Tests, Test_SE_VxVy) {
    chip8.m_V[0] = 0x0A;
    chip8.m_V[1] = 0x0A;
    chip8.m_pc = 0x201;

    int pcBefore = chip8.m_pc;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "V1 is currently equal to: " << static_cast<int>(chip8.m_V[1]);
    GTCOUT << "PC is currently equal to: " << chip8.m_pc;

    GTCOUT << "since V0 is equal to V1, the next instruction should be skipped";
    chip8.SE_VxVy();

    EXPECT_EQ(chip8.m_pc, pcBefore + 4);
}

// LD_Vx_byte - set Vx to byte (opcode 0x6xkk)
TEST_F(Chip8Tests, Test_LD_Vx_byte) {
    chip8.m_V[0] = 0x0A;
    chip8.byte = 0x0B;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "byte is currently equal to: " << chip8.byte;

    GTCOUT << "V0 should be set to byte";
    chip8.LD_Vx_byte();

    EXPECT_EQ(chip8.m_V[0], chip8.byte);
}

// ADD_Vx_byte - add byte to Vx (opcode 0x7xkk)
TEST_F(Chip8Tests, Test_ADD_Vx_byte) {
    chip8.m_V[0] = 0x0A;
    chip8.byte = 0x0B;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "byte is currently equal to: " << chip8.byte;

    GTCOUT << "V0 should be incremented by byte";
    chip8.ADD_Vx_byte();

    EXPECT_EQ(chip8.m_V[0], 0x15);
}

// LD_VxVy - set Vx to Vy (opcode 0x8xy0)
TEST_F(Chip8Tests, Test_LD_VxVy) {
    chip8.x = 0x0;
    chip8.y = 0x1;

    chip8.m_V[chip8.x] = 0x0A;
    chip8.m_V[chip8.y] = 0x0B;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[chip8.x]);
    GTCOUT << "V1 is currently equal to: " << static_cast<int>(chip8.m_V[chip8.y]);

    GTCOUT << "V0 should be set to V1";
    chip8.LD_VxVy();

    EXPECT_EQ(chip8.m_V[chip8.x], chip8.m_V[chip8.y]);
}

// OR - bitwise OR Vx with Vy (opcode 0x8xy1)
TEST_F(Chip8Tests, Test_OR) {
    chip8.m_V[0] = 0x0A;
    chip8.m_V[1] = 0x0B;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "V1 is currently equal to: " << static_cast<int>(chip8.m_V[1]);

    GTCOUT << "V0 should be set to the result of V0 | V1";
    chip8.OR();

    EXPECT_EQ(chip8.m_V[0], 0x0B);   
}

// AND - bitwise AND Vx with Vy (opcode 0x8xy2)
TEST_F(Chip8Tests, Test_AND) {
    chip8.m_V[0] = 0x0A;
    chip8.m_V[1] = 0x0B;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "V1 is currently equal to: " << static_cast<int>(chip8.m_V[1]);

    GTCOUT << "V0 should be set to the result of V0 & V1";
    chip8.AND();

    EXPECT_EQ(chip8.m_V[0], 0x0A);   
}

// XOR - bitwise XOR Vx with Vy (opcode 0x8xy3)
TEST_F(Chip8Tests, Test_XOR) {
    chip8.m_V[0] = 0x0A;
    chip8.m_V[1] = 0x0B;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "V1 is currently equal to: " << static_cast<int>(chip8.m_V[1]);

    GTCOUT << "V0 should be set to the result of V0 ^ V1";
    chip8.XOR();

    EXPECT_EQ(chip8.m_V[0], 0x01);   
}

// ADD_VxVy - add Vy to Vx (opcode 0x8xy4)
TEST_F(Chip8Tests, Test_ADD_VxVy) {
    chip8.m_V[0] = 0x0A;
    chip8.m_V[1] = 0x0B;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "V1 is currently equal to: " << static_cast<int>(chip8.m_V[1]);

    GTCOUT << "V0 should be incremented by V1";
    chip8.ADD_VxVy();

    EXPECT_EQ(chip8.m_V[0], 0x15);
    EXPECT_EQ(chip8.m_V[0xF], 0x00); // VF should be 0
}

// SUB - subtract Vy from Vx (opcode 0x8xy5)
TEST_F(Chip8Tests, Test_SUB) {
    chip8.m_V[0] = 0x0B;
    chip8.m_V[1] = 0x0A;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "V1 is currently equal to: " << static_cast<int>(chip8.m_V[1]);

    GTCOUT << "V0 should be decremented by V1";
    chip8.SUB();

    EXPECT_EQ(chip8.m_V[0], 0x01);
    EXPECT_EQ(chip8.m_V[0xF], 0); // VF should be 0 (false) as V1 > V0 now
}

// SHR - shift Vx right by 1 (opcode 0x8xy6)
TEST_F(Chip8Tests, Test_SHR) {
    chip8.m_V[0] = 0x0B;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);

    GTCOUT << "V0 should be shifted right by 1";
    chip8.SHR();

    EXPECT_EQ(chip8.m_V[0], 0x05);
    EXPECT_EQ(chip8.m_V[0xF], 0x01); // VF should be 1
}

// SUBN - subtract Vx from Vy (opcode 0x8xy7)
TEST_F(Chip8Tests, Test_SUBN) {
    chip8.m_V[0] = 0x0A;
    chip8.m_V[1] = 0x0B;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "V1 is currently equal to: " << static_cast<int>(chip8.m_V[1]);

    GTCOUT << "V0 should be decremented by V1";
    chip8.SUBN();

    EXPECT_EQ(chip8.m_V[0], 0x01);
    EXPECT_EQ(chip8.m_V[0xF], 0x01); // VF should be 1
}

// SHL - shift Vx left by 1 (opcode 0x8xyE)
TEST_F(Chip8Tests, Test_SHL) {
    chip8.m_V[0] = 0x0B;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);

    GTCOUT << "V0 should be shifted left by 1";
    chip8.SHL();

    EXPECT_EQ(chip8.m_V[0], 0x16);
    EXPECT_EQ(chip8.m_V[0xF], 0x00); // VF should be 0
}

// SNE_VxVy - skip next instruction if Vx does not equal Vy (opcode 0x9xy0)
TEST_F(Chip8Tests, Test_SNE_VxVy) {
    chip8.m_V[0] = 0x0A;
    chip8.m_V[1] = 0x0B;
    chip8.m_pc = 0x201;

    int pcBefore = chip8.m_pc;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "V1 is currently equal to: " << static_cast<int>(chip8.m_V[1]);
    GTCOUT << "PC is currently equal to: " << chip8.m_pc;

    GTCOUT << "since V0 is not equal to V1, the next instruction should be skipped";
    chip8.SNE_VxVy();

    EXPECT_EQ(chip8.m_pc, pcBefore + 4);
}

// LD_I_addr - set index (I) to address (opcode 0xAnnn)
TEST_F(Chip8Tests, Test_LD_I_addr) {
    chip8.m_index = 0x0A;
    chip8.addr = 0x0B;

    GTCOUT << "I is currently equal to: " << chip8.m_index;
    GTCOUT << "address is currently equal to: " << chip8.addr;

    GTCOUT << "I should be set to address";
    chip8.LD_I_addr();

    EXPECT_EQ(chip8.m_index, chip8.addr);
}

// JP_addrV0 - jump to address + V0 (opcode 0xBnnn)
TEST_F(Chip8Tests, Test_JP_addrV0) {
    chip8.m_V[0] = 0x0A;
    chip8.addr = 0x0B;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "address is currently equal to: " << chip8.addr;

    GTCOUT << "PC should be set to address + V0";
    chip8.JP_addrV0();

    EXPECT_EQ(chip8.m_pc, 0x15);
}

// RND - set Vx to random byte AND kk (opcode 0xCxkk)
TEST_F(Chip8Tests, Test_RND) {
    chip8.m_V[0] = 0x0A;
    chip8.byte = 0x0B;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "byte is currently equal to: " << chip8.byte;

    GTCOUT << "V0 should be set to a random byte AND byte";
    chip8.RND();

    EXPECT_EQ(chip8.m_V[0] & chip8.byte, chip8.m_V[0]);
}

// DRW - draw sprite at Vx, Vy (opcode 0xDxyn)
TEST_F(Chip8Tests, Test_DRW) {
    chip8.m_V[0] = 0x0A;
    chip8.m_V[1] = 0x0B;
    chip8.m_index = 0x0;
    chip8.m_memory[0] = 0xF0;
    chip8.m_memory[1] = 0x90;
    chip8.m_memory[2] = 0x90;
    chip8.m_memory[3] = 0x90;
    chip8.m_memory[4] = 0xF0;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "V1 is currently equal to: " << static_cast<int>(chip8.m_V[1]);
    GTCOUT << "I is currently equal to: " << chip8.m_index;

    GTCOUT << "a sprite should be drawn at V0, V1";
    chip8.DRW();

    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(chip8.display[i], 0x00);
    }
}

// SKP - skip next instruction if key with value of Vx is pressed (opcode 0xEx9E)
TEST_F(Chip8Tests, Test_SKP) {
    chip8.m_V[0] = 0x0A;
    chip8.key[0x0A] = 1;
    chip8.m_pc = 0x201;

    int pcBefore = chip8.m_pc;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "key with value of V0 is currently pressed";

    GTCOUT << "since key with value of V0 is pressed, the next instruction should be skipped";
    chip8.SKP();

    EXPECT_EQ(chip8.m_pc, pcBefore + 4);
}

// SKNP - skip next instruction if key with value of Vx is not pressed (opcode 0xExA1)
TEST_F(Chip8Tests, Test_SKNP) {
    chip8.m_V[0] = 0x0A;
    chip8.key[0x0A] = 0;
    chip8.m_pc = 0x201;

    int pcBefore = chip8.m_pc;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "key with value of V0 is currently not pressed";

    GTCOUT << "since key with value of V0 is not pressed, the next instruction should not be skipped";
    chip8.SKNP();

    EXPECT_EQ(chip8.m_pc, pcBefore + 4);
}

// LD_Vx_t - set Vx to delay timer (opcode 0xFx07)
TEST_F(Chip8Tests, Test_LD_Vx_t) {
    chip8.m_V[0] = 0x0A;
    chip8.m_delayTimer = 0x0B;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "delay timer is currently equal to: " << chip8.m_delayTimer;

    GTCOUT << "V0 should be set to delay timer";
    chip8.LD_Vx_t();

    EXPECT_EQ(chip8.m_V[0], chip8.m_delayTimer);
}

// LD_Vx_k - wait for key press and store value in Vx (opcode 0xFx0A)
TEST_F(Chip8Tests, Test_LD_Vx_k) {
    int keyPressed = 0x0A;
    chip8.key[keyPressed] = 1;
    chip8.x = keyPressed;

    GTCOUT << "key 0x0A is currently being pressed\n";
    GTCOUT << "V0 should be set to the value of the key being pressed";
    chip8.LD_Vx_k();

    EXPECT_EQ(chip8.m_V[keyPressed], keyPressed); // key[0x0A] = V[10]
    chip8.x = 0x0;
}

// LD_t_Vx - set delay timer to Vx (opcode 0xFx15)
TEST_F(Chip8Tests, Test_LD_t_Vx) {
    chip8.m_V[0] = 0x0A;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);

    GTCOUT << "delay timer should be set to V0";
    chip8.LD_t_Vx(chip8.m_delayTimer);

    EXPECT_EQ(chip8.m_delayTimer, chip8.m_V[0]);
}

// ADD_I_Vx - set index (I) to I + Vx (opcode 0xFx1E)
TEST_F(Chip8Tests, Test_ADD_I_Vx) {
    chip8.m_index = 0x0A;
    chip8.m_V[0] = 0x0B;

    GTCOUT << "I is currently equal to: " << chip8.m_index;
    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);

    GTCOUT << "I should be set to I + V0";
    chip8.ADD_I_Vx();

    EXPECT_EQ(chip8.m_index, 0x15);
}

// LD_F_Vx - set index (I) to location of sprite for digit Vx (opcode 0xFx29)
TEST_F(Chip8Tests, Test_LD_F_Vx) {
    chip8.m_V[0] = 0x0A;
    int sum = chip8.m_V[0] * 0x5;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "I should be set to the location of the sprite for digit V0";
    GTCOUT << "i.e., I = Vx * 0x5 = " << sum;
    chip8.LD_F_Vx();

    EXPECT_EQ(chip8.m_index, sum);
}

// LD_BCD - store BCD representation of Vx in memory locations I, I+1, and I+2 (opcode 0xFx33)
TEST_F(Chip8Tests, Test_LD_BCD) {
    chip8.m_V[0] = 0xFF; // = 255
    chip8.m_index = 0x0;

    GTCOUT << "V0 is currently equal to: " << static_cast<int>(chip8.m_V[0]);
    GTCOUT << "I is currently equal to: " << chip8.m_index;

    GTCOUT << "BCD representation of V0 should be stored in memory locations I, I+1, and I+2";
    GTCOUT << "i.e., I = V0 / 100 = " << chip8.m_V[0] / 100 << ", I+1 = (V0 / 10) % 10 = " << (chip8.m_V[0] / 10) % 10 << ", I+2 = V0 % 10 = " << chip8.m_V[0] % 10;
    chip8.LD_BCD();

    EXPECT_EQ(chip8.m_memory[0], 0x2);  // 255 / 100 = 2
    EXPECT_EQ(chip8.m_memory[1], 0x5);  // (255 / 10) = 25.5, 25.5 % 10 = 5
    EXPECT_EQ(chip8.m_memory[2], 0x5);  // 255 % 10 = 5
}

// LD_wVF - store registers V0 through Vx in memory starting at location I (opcode 0xFx55)
TEST_F(Chip8Tests, Test_LD_wVF) {
    chip8.x = 0x0;
    for (int i = 0; i < chip8.x; ++i) {
        chip8.m_V[i] = i;
    }

    GTCOUT << "I is currently equal to: " << chip8.m_index;
    GTCOUT << "V0 through V4 should be stored in memory starting at location I";
    chip8.LD_wVF();

    for (int i = 0; i < chip8.x; ++i) {
        EXPECT_EQ(chip8.m_memory[chip8.m_index + i], i);
    }   
    chip8.x = 0x5; 
}

// LD_rVF - read registers V0 through Vx from memory starting at location I (opcode 0xFx65)
TEST_F(Chip8Tests, Test_LD_rVF) {
    chip8.m_index = 0x0;
    for (int i = 0; i < 0x5; ++i) {
        chip8.m_memory[i] = i;
    }

    GTCOUT << "I is currently equal to: " << chip8.m_index;
    GTCOUT << "V0 through V4 should be read from memory starting at location I";
    chip8.LD_rVF();

    for (int i = 0; i < 0x5; ++i) {
        EXPECT_EQ(chip8.m_V[i], i);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
