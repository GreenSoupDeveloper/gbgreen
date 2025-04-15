//
// Created by sarbajit on 5/5/17.
//

#ifndef CHIP8_CHIP8EMULATOR_H
#define CHIP8_CHIP8EMULATOR_H


#include <cstdint>
#include <string>
#include <chrono>
#include <random>
class CPU
{

private:
    union RegisterPair {
        struct {
            uint8_t lo;
            uint8_t hi;
        };
        uint16_t full;

        RegisterPair() : full(0) {}
    };
    enum Register8 {
        REG_A, REG_B, REG_C, REG_D, REG_E, REG_H, REG_L, REG_F
    };
    enum Flag {
        FLAG_Z = 1 << 7, // Zero
        FLAG_N = 1 << 6, // Subtract
        FLAG_H = 1 << 5, // Half Carry
        FLAG_C = 1 << 4  // Carry
    };

    RegisterPair AF, BC, DE, HL;
    uint16_t SP, PC;

    // Memory (simplified, 64KB)
    uint8_t memory[0x10000];

    uint64_t cycles;
    bool IME = false; // Interrupt Master Enable
    bool halted = false;

    uint8_t& IF = memory[0xFF0F]; // Interrupt Flags
    uint8_t& IE = memory[0xFFFF]; // Interrupt Enable
    

    // Helpers
  
    uint8_t& getReg8( Register8 reg);

    void halt();

    void setFlag( Flag flag, bool value);
    bool getFlag( Flag flag);

    void HandleInterrupt();

    void ExecuteInstruction(uint8_t opcode);
    void ld_r_n(Register8 reg, uint8_t value);
    void ld_r_nn(Register8 reg, RegisterPair pair);
    void ld_r_r(Register8 dest, Register8 src);
    void ld_rr_d16(RegisterPair& reg);
    void ld_addr_rr_a(RegisterPair& reg);
    void xor_r_r(Register8 reg1, Register8 reg2);
    void inc_rr(RegisterPair& reg);
    void inc_r(Register8 reg);
    void dec_r(Register8 reg);
    void dec_rr(RegisterPair& reg);
    void rlca();
    void ld_a16_sp();
    void add_hl_rr(const RegisterPair& rr);
    void ld_a_addr_rr(const RegisterPair& reg);
    void rrca();
    void rla();
    void jr(int8_t offset);
    void rra();
    void jr_nc(int8_t r8);
    void jr_nz(int8_t r8);
    void jr_z(int8_t r8);
    void daa();
    void cpl();
    void inc_hl_ptr();
    void dec_hl_ptr();
    void ld_hl_d8();
    void jr_c_s8();
    void add_hl_sp();
    void ld_a_hld();
    void ccf();
    void ld_rr_r(RegisterPair pair, Register8 reg);
    void add_r_r(Register8 reg1, Register8 reg2);
    void add_r_rr(Register8 reg, RegisterPair pair);
    void adc_r_r(Register8 reg1, Register8 reg2);
    void adc_r_rr(Register8 reg, RegisterPair pair);
    void sub_r(Register8 reg);
    void sub_rr(RegisterPair pair);
    void sbc_r_r(Register8 reg1, Register8 reg2);
    void sbc_r_rr(Register8 reg, RegisterPair pair);
    void and_r_r(Register8 reg1, Register8 reg2);
    void and_r_rr(Register8 reg, RegisterPair pair);
    void xor_r_rr(Register8 reg, RegisterPair pair);
    void or_r_r(Register8 reg1, Register8 reg2);
    void or_r_rr(Register8 reg, RegisterPair pair);
    void cp_r_r(Register8 reg1, Register8 reg2);
    void cp_r_rr(Register8 reg, RegisterPair pair);
public:

    CPU();
    bool LoadROM(std::string filename);
    void Cycle();
    
    void d_PrintState(); // Executes one instruction
    ~CPU();
};


#endif