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
	uint8_t lo = bus.bus_read(cpu.PC++);
	uint8_t hi = bus.bus_read(cpu.PC++);
	reg.lo = lo;
	reg.hi = hi;
}
void Instruction::ld_addr_rr_a(CPU::RegisterPair& reg, int ifthing) {
	uint16_t address = reg.full;
	//std::cout << address << " ld addr, rr\n";
	bus.bus_write(address, cpu.AF.hi); // AF.hi = A
	if (ifthing == -1) {
		reg.full--;
	}
	else if (ifthing == 1) {
		reg.full++;
	}
	cpu.PC++;
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
	//printf("hi %d", getReg(reg));
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
	uint8_t lo = bus.bus_read(cpu.PC++); // Read low byte, increment PC
	uint8_t hi = bus.bus_read(cpu.PC++); // Read high byte, increment PC
	uint16_t addr = (hi << 8) | lo;
	bus.bus_write(addr, cpu.SP & 0xFF);
	bus.bus_write(addr + 1, (cpu.SP >> 8) & 0xFF);
}
void Instruction::add_hl_rr(const CPU::RegisterPair& rr) {
	uint32_t result = cpu.HL.full + rr.full;

	setFlag(cpu.FLAG_N, false);
	setFlag(cpu.FLAG_H, ((cpu.HL.full & 0x0FFF) + (rr.full & 0x0FFF)) > 0x0FFF);
	setFlag(cpu.FLAG_C, result > 0xFFFF);

	cpu.HL.full = result & 0xFFFF;
}
void Instruction::ld_a_addr_rr(CPU::RegisterPair& reg, int decOrInc) {
	uint16_t address = reg.full;
	cpu.AF.hi = bus.bus_read(address);  // Load value at address pointed by reg into A

	if (decOrInc == -1)
		reg.full--;
	else
		reg.full++;
	
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
void Instruction::rla() {
	uint8_t& A = cpu.AF.hi;
	uint8_t bit7 = (A >> 7) & 0x01;  // Get bit 7 (highest bit of A)
	uint8_t carry = getFlag(cpu.FLAG_C); // Get current Carry flag value

	A = (A << 1) | carry; // Shift A left and place carry in bit 0
	setFlag(cpu.FLAG_C, bit7); // Set Carry flag to bit 7 of A

	setFlag(cpu.FLAG_Z, false); // Always cleared
	setFlag(cpu.FLAG_N, false); // Always cleared
	setFlag(cpu.FLAG_H, false); // Always cleared
}
void Instruction::jr_n() {
	int8_t r8 = static_cast<int8_t>(bus.bus_read(cpu.PC + 1)); // Read the signed byte at PC + 1
	cpu.PC += r8;
	cpu.PC += 2; // Since JR takes 2 bytes, move the program counter forward by 2
}
void Instruction::rr_r(Register8 reg) {
	uint8_t& r = getReg(reg);
	uint8_t bit0 = r & 0x01;  // Get bit 0 (lowest bit of A)
	uint8_t carry = getFlag(cpu.FLAG_C); // Get current Carry flag value

	r = (r >> 1) | (carry << 7); // Shift A right and place carry in bit 7
	setFlag(cpu.FLAG_C, bit0); // Set Carry flag to bit 0 of A

	setFlag(cpu.FLAG_Z, false); // Always cleared
	setFlag(cpu.FLAG_N, false); // Always cleared
	setFlag(cpu.FLAG_H, false); // Always cleared
}
void Instruction::jr_f(int8_t r8, CPU::Flag flag, bool ifNot) {
	// Check the selected flag ()
	if (ifNot) {
		// Perform the relative jump if flag is not set (N(F))
		if (!getFlag(flag))
			cpu.PC += r8;
		else
			cpu.PC += 2;
	}
	else {
		// If the flag is set, do not jump and just increment PC by 2
		if (!getFlag(flag))
			cpu.PC += 2;
		else
			cpu.PC += r8;
	}
}
void Instruction::daa() {
	uint8_t& A = cpu.AF.hi;
	bool N = getFlag(cpu.FLAG_N);
	bool H = getFlag(cpu.FLAG_H);
	bool C = getFlag(cpu.FLAG_C);

	uint8_t correction = 0;

	if (!N) { // after addition
		if (C || A > 0x99) {
			correction |= 0x60;
			setFlag(cpu.FLAG_C, true);
		}
		if (H || (A & 0x0F) > 0x09) {
			correction |= 0x06;
		}
		A += correction;
	}
	else { // after subtraction
		if (C) correction |= 0x60;
		if (H) correction |= 0x06;
		A -= correction;
	}

	setFlag(cpu.FLAG_Z, A == 0);
	setFlag(cpu.FLAG_H, false); // Always cleared
	// N is unchanged
}
void Instruction::cpl() {
	cpu.AF.hi = ~cpu.AF.hi;         // A = NOT A
	setFlag(cpu.FLAG_N, true);  // Set N flag
	setFlag(cpu.FLAG_H, true);  // Set H flag
}
Instruction::Instruction() {

}


