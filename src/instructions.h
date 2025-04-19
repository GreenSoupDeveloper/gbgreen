#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cpu.h>

class Instruction {

public:
    const uint8_t instructionTicks[256] = {
       4,  12, 8, 8,  4,  4,  8,  4,  20, 8,  8,  8, 4, 4,  8, 4,   // 0x0_
       4,  12, 8, 8,  4,  4,  8,  4,  12, 8,  8,  8, 4, 4,  8, 4,   // 0x1_
       0,  12, 8, 8,  4,  4,  8,  4,  0,  8,  8,  8, 4, 4,  8, 4,   // 0x2_
       0,  12, 8, 8,  12, 12, 12, 4,  0,  8,  8,  8, 4, 4,  8, 4,   // 0x3_
       4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,   // 0x4_
       4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,   // 0x5_
       4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,   // 0x6_
       8,  8,  8, 8,  8,  8,  4,  8,  4,  4,  4,  4, 4, 4,  8, 4,   // 0x7_
       4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,   // 0x8_
       4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,   // 0x9_
       4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,   // 0xa_
       4,  4,  4, 4,  4,  4,  8,  4,  4,  4,  4,  4, 4, 4,  8, 4,   // 0xb_
       0,  12, 0, 16, 0,  16, 8,  16, 0,  16, 0,  0, 0, 24, 8, 16,  // 0xc_
       0,  12, 0, 0,  0,  16, 8,  16, 0,  16, 0,  0, 0, 0,  8, 16,  // 0xd_
       12, 12, 8, 0,  0,  16, 8,  16, 16, 4,  16, 0, 0, 0,  8, 16,  // 0xe_
       12, 12, 8, 4,  0,  16, 8,  16, 12, 8,  16, 4, 0, 0,  8, 16   // 0xf_
    };
    const uint8_t extendedInstructionTicks[256] = {
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0x0_
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0x1_
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0x2_
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0x3_
        8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,  // 0x4_
        8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,  // 0x5_
        8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,  // 0x6_
        8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,  // 0x7_
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0x8_
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0x9_
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0xa_
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0xb_
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0xc_
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0xd_
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0xe_
        8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8   // 0xf_
    };
    enum Register8 {
        REG_A, REG_B, REG_C, REG_D, REG_E, REG_H, REG_L, REG_F
    };


   

    Instruction();
    void setFlag(CPU::Flag flag, bool value);
    bool getFlag(CPU::Flag flag);
    uint8_t& getReg(Register8 reg);



    void ld_rr_d16(CPU::RegisterPair& reg);
    void ld_addr_rr_a(CPU::RegisterPair& reg, int ifthing);
    void inc_r(Register8 reg);
    void dec_r(Register8 reg);
    void ld_r_n(Register8 reg, uint8_t value);
    void rlc_r(Register8 reg);
    void ld_a16_sp();
    void add_hl_rr(const uint16_t rr);
    void ld_a_addr_rr(CPU::RegisterPair& reg, int decOrInc);
    void rrc(Register8 reg);
    void rla();
    void jr_n();
    void rr_r(Register8 reg);
    void jr_f(int8_t r8, CPU::Flag flag, bool ifNot);
    void daa();
    void cpl();
    void ld_r_r(Register8 dest, Register8 src);
    void ld_r_rr(Register8 reg, CPU::RegisterPair pair);
    void ld_rr_r(CPU::RegisterPair pair, Register8 reg);
    void add_r_r(Register8 reg1, Register8 reg2);
    void add_r_rr(Register8 reg, CPU::RegisterPair pair);
    void adc_r_r(Register8 reg1, Register8 reg2);
    void adc_r_rr(Register8 reg, CPU::RegisterPair pair);
    void sub_r(Register8 reg);
    void sub_rr(CPU::RegisterPair pair);
    void sbc_r_r(Register8 reg1, Register8 reg2);
    void sbc_r_rr(Register8 reg, CPU::RegisterPair pair);


    ~Instruction();

   

};
extern Instruction insts;
