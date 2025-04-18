#include <instructions.h>
#include <cpu.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <bus.h>


void Instruction::setFlag(CPU::Flag flag, bool value) {
	if (value)
		cpu.AF.lo |= flag;
	else
		cpu.AF.lo &= ~flag;
}
bool Instruction::getFlag(CPU::Flag flag) {
	return cpu.AF.lo & flag;
}
uint8_t& Instruction::getReg(Register8 reg) {
	switch (reg) {
	case REG_A: return cpu.AF.hi;
	case REG_F: return cpu.AF.lo;
	case REG_B: return cpu.BC.hi;
	case REG_C: return cpu.BC.lo;
	case REG_D: return cpu.DE.hi;
	case REG_E: return cpu.DE.lo;
	case REG_H: return cpu.HL.hi;
	case REG_L: return cpu.HL.lo;
	default: throw std::runtime_error("Invalid 8-bit register");
	}
}







void Instruction::ld_rr_d16(CPU::RegisterPair& reg) {
	uint8_t lo = cart.rom_data[cpu.PC++];
	uint8_t hi = cart.rom_data[cpu.PC++];
	reg.lo = lo;
	reg.hi = hi;
}
void Instruction::ld_addr_rr_a(CPU::RegisterPair& reg, bool subtract, bool add) {
	uint16_t address = reg.full;
	bus.wram[address] = cpu.AF.hi; // AF.hi = A
	if (subtract) {
		reg.full--;
	}
	else if (add) {
		reg.full++;
	}
}
void Instruction::inc_r(Register8 reg) {
	uint8_t& r = getReg(reg);
	uint8_t result = r + 1;

	setFlag(cpu.FLAG_Z, result == 0);
	setFlag(cpu.FLAG_N, false);
	setFlag(cpu.FLAG_H, (r & 0x0F) + 1 > 0x0F);

	r = result;
}

Instruction::Instruction() {

}


