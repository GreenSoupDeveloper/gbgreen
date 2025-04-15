#include <cstring>
#include <fstream>
#include <iostream>
#include <cpu.h>

enum Register8 {
    REG_A, REG_B, REG_C, REG_D, REG_E, REG_H, REG_L, REG_F
};

uint8_t& getReg8(Register8 reg) {
    switch (reg) {
    case REG_A: return AF.hi;
    case REG_F: return AF.lo;
    case REG_B: return BC.hi;
    case REG_C: return BC.lo;
    case REG_D: return DE.hi;
    case REG_E: return DE.lo;
    case REG_H: return HL.hi;
    case REG_L: return HL.lo;
    default: throw std::runtime_error("Invalid 8-bit register");
    }
}