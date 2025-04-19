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
void Instruction::dec_r(Register8 reg) {
	uint8_t& r = getReg(reg);
	uint8_t result = r - 1;

	setFlag(cpu.FLAG_Z, result == 0);
	setFlag(cpu.FLAG_N, true);
	setFlag(cpu.FLAG_H, (r & 0x0F) == 0x00); // Borrow from bit 4?

	r = result;
}
void Instruction::ld_r_n(Register8 reg, uint8_t value) {
	getReg(reg) = value;
}

void Instruction::rlc_r(Register8 reg) {
	uint8_t& r = getReg(reg);
	uint8_t bit7 = (r & 0x80) >> 7; // Get bit 7

	r = (r << 1) | bit7;

	setFlag(cpu.FLAG_Z, false); // Always cleared
	setFlag(cpu.FLAG_N, false);
	setFlag(cpu.FLAG_H, false);
	setFlag(cpu.FLAG_C, bit7);
}
void Instruction::ld_a16_sp() {
	uint8_t lo = cart.rom_data[cpu.PC++];
	uint8_t hi = cart.rom_data[cpu.PC++];
	uint16_t addr = (hi << 8) | lo;

	cart.rom_data[addr] = cpu.SP & 0xFF;       // low byte
	cart.rom_data[addr + 1] = (cpu.SP >> 8) & 0xFF; // high byte
}
void Instruction::add_hl_rr(const CPU::RegisterPair& rr) {
	uint32_t result = cpu.HL.full + rr.full;

	setFlag(cpu.FLAG_N, false);
	setFlag(cpu.FLAG_H, ((cpu.HL.full & 0x0FFF) + (rr.full & 0x0FFF)) > 0x0FFF);
	setFlag(cpu.FLAG_C, result > 0xFFFF);

	cpu.HL.full = result & 0xFFFF;
}
void Instruction::ld_a_addr_rr(const CPU::RegisterPair& reg) {
	uint16_t address = reg.full;
	cpu.AF.hi = cart.rom_data[address];  // Load value at address pointed by reg into A
}
void Instruction::rrc(Register8 reg) {
	uint8_t& r = getReg(reg);
	uint8_t bit0 = r & 0x01; // Get bit 0 (lowest bit of A)
	uint8_t carry = getFlag(cpu.FLAG_C); // Get current Carry flag value

	r = (r >> 1) | (carry << 7); // Shift A right and place carry in bit 7
	setFlag(cpu.FLAG_C, bit0); // Set Carry flag to bit 0 of A

	setFlag(cpu.FLAG_Z, false); // Always cleared
	setFlag(cpu.FLAG_N, false); // Always cleared
	setFlag(cpu.FLAG_H, false); // Always cleared
}
Instruction::Instruction() {

}


