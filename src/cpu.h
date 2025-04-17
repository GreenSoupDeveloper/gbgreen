//
// Created by sarbajit on 5/5/17.
//

#ifndef CHIP8_CHIP8EMULATOR_H
#define CHIP8_CHIP8EMULATOR_H


#include <cstdint>
#include <string>
#include <chrono>
#include <random>
#include <cartridge.h>
class CPU
{

public:
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
    uint8_t wram[0x2000];   // 8KB Work RAM (0xC000–0xDFFF)
    uint8_t vram[0x2000];   // 8KB Video RAM (0x8000–0x9FFF)
    uint8_t eram[0x2000];   // 8KB External RAM (0xA000–0xBFFF)
    uint8_t oam[0xA0];      // Object Attribute Memory (0xFE00–0xFE9F)
    uint8_t hram[0x7F];     // High RAM (0xFF80–0xFFFE)
    uint8_t io[0x80]; // I/O ports (0xFF00–0xFF7F)
    uint8_t IE = 0x00;    // 0xFFFF
    uint8_t IF = 0xE1;    // 0xFF0F

    uint64_t temp_t_cycles;
    uint64_t t_cycles;
    uint64_t m_cycles;
    bool IME = false; // Interrupt Master Enable
    bool halted = false;
    bool haltBug = false;
    bool interrupts_enabled;

    const uint8_t nintendoLogo[0x30] = {
        0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
    };

   
    

    // Helpers

  

    void setFlag(Flag flag, bool value);
    bool getFlag(Flag flag);

    uint8_t ReadByte(uint16_t addr);
    void Push16(uint16_t value);
    void HandleInterrupt();
    void ExecuteInstruction(uint8_t opcode);
    uint8_t& getReg8(Register8 reg);
    

    void halt();
 
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
    void ret_nz();
    void pop_rr(RegisterPair pair);
    void jp_nz_a16();
    void jp_a16();
    void call_nz_a16();
    void push_rr(RegisterPair pair);
    void add_a_d8();
    void rst_0();
    void ret_z();
    void ret();
    void jp_z();
    void call_z();
    void call();
    void adc_a_d8();
    void rst_1();
    void ret_nc();
    void jp_nc_a16();
    void call_nc_a16();
    void sub_d8();
    void rst_2();
    void ret_c();
    void reti();
    void jp_c_a16();
    void call_z_a16();
    void sbc_a_d8();
    void rst_3();
    void ld_a_a8();
    
    void and_d8();
    void rst_4();
    void add_sp_s8();
    void ld_a16_a();
    void xor_d8();
    void rst_5();
    void ld_a8_a();
    void ld_c_a();
    void di();
    void or_d8();
    void rst_6();
    void ld_hl_sp_s8();
    void ld_a_a16();
    void ei();
    void cp_d8();
    void rst_7();


    CPU();
    bool LoadROM(std::string filename);
    void Cycle();
    
    void d_PrintState(); // Executes one instruction
    ~CPU();
};


#endif
extern CPU cpu;