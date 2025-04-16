#include <cstring>
#include <fstream>
#include <iostream>
#include <cpu.h>
#include <tools.h>
#include <cartridge.h>
CPU::CPU()
{

}
//helper stuff here..


uint8_t& CPU::getReg8(Register8 reg) {
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

void CPU::halt() {
	if (!IME && (IE & IF) != 0) {
		// HALT bug triggers
		haltBug = true;
	}
	else {
		halted = true;
	}
	temp_t_cycles += 4;
}

void CPU::setFlag(Flag flag, bool value) {
	if (value)
		AF.lo |= flag;
	else
		AF.lo &= ~flag;
}

bool CPU::getFlag(Flag flag) {
	return AF.lo & flag;
}

void CPU::ld_r_n(Register8 reg, uint8_t value) {
	getReg8(reg) = value;
	temp_t_cycles += 8;
}
void CPU::ld_r_nn(Register8 reg, RegisterPair pair) {
	getReg8(reg) = wram[pair.full];
	temp_t_cycles += 8;
}



void CPU::ld_r_r(Register8 dest, Register8 src) {
	getReg8(dest) = getReg8(src);
	temp_t_cycles += 4;
}
void CPU::ld_rr_d16(RegisterPair& reg) {
	uint8_t lo = wram[PC++];
	uint8_t hi = wram[PC++];
	reg.lo = lo;
	reg.hi = hi;
	temp_t_cycles += 12;
}


void CPU::ld_addr_rr_a(RegisterPair& reg) {
	uint16_t address = reg.full;
	wram[address] = AF.hi; // AF.hi = A
	temp_t_cycles += 8;
}
void CPU::xor_r_r(Register8 reg1, Register8 reg2) {
	getReg8(reg1) ^= getReg8(reg2);
	setFlag(FLAG_Z, (getReg8(reg1) == 0));
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, false);
	temp_t_cycles += 4;
}
void CPU::inc_rr(RegisterPair& reg) {
	reg.full += 1;
	temp_t_cycles += 12;
}
void CPU::inc_r(Register8 reg) {
	uint8_t& r = getReg8(reg);
	uint8_t result = r + 1;

	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, (r & 0x0F) + 1 > 0x0F);

	r = result;
	temp_t_cycles += 4;
}
void CPU::dec_r(Register8 reg) {
	uint8_t& r = getReg8(reg);
	uint8_t result = r - 1;

	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, (r & 0x0F) == 0x00); // Borrow from bit 4?

	r = result;
	temp_t_cycles += 4;
}
void CPU::dec_rr(RegisterPair& reg) {
	reg.full -= 1;
	temp_t_cycles += 8;
}
void CPU::rlca() {
	uint8_t& A = AF.hi;
	uint8_t bit7 = (A & 0x80) >> 7; // Get bit 7

	A = (A << 1) | bit7;

	setFlag(FLAG_Z, false); // Always cleared
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, bit7);
	temp_t_cycles += 4;
}
void CPU::ld_a16_sp() {
	uint8_t lo = wram[PC++];
	uint8_t hi = wram[PC++];
	uint16_t addr = (hi << 8) | lo;

	wram[addr] = SP & 0xFF;       // low byte
	wram[addr + 1] = (SP >> 8) & 0xFF; // high byte
	temp_t_cycles += 20;
}
void CPU::add_hl_rr(const RegisterPair& rr) {
	uint32_t result = HL.full + rr.full;

	setFlag(FLAG_N, false);
	setFlag(FLAG_H, ((HL.full & 0x0FFF) + (rr.full & 0x0FFF)) > 0x0FFF);
	setFlag(FLAG_C, result > 0xFFFF);

	HL.full = result & 0xFFFF;
	temp_t_cycles += 8;
}
void CPU::ld_a_addr_rr(const RegisterPair& reg) {
	uint16_t address = reg.full;
	AF.hi = wram[address];  // Load value at address pointed by reg into A
	temp_t_cycles += 8;
}
void CPU::rrca() {
	uint8_t& A = AF.hi;
	uint8_t bit0 = A & 0x01; // Get bit 0 (lowest bit of A)
	uint8_t carry = getFlag(FLAG_C); // Get current Carry flag value

	A = (A >> 1) | (carry << 7); // Shift A right and place carry in bit 7
	setFlag(FLAG_C, bit0); // Set Carry flag to bit 0 of A

	setFlag(FLAG_Z, false); // Always cleared
	setFlag(FLAG_N, false); // Always cleared
	setFlag(FLAG_H, false); // Always cleared
	temp_t_cycles += 4;
}
void CPU::rla() {
	uint8_t& A = AF.hi;
	uint8_t bit7 = (A >> 7) & 0x01;  // Get bit 7 (highest bit of A)
	uint8_t carry = getFlag(FLAG_C); // Get current Carry flag value

	A = (A << 1) | carry; // Shift A left and place carry in bit 0
	setFlag(FLAG_C, bit7); // Set Carry flag to bit 7 of A

	setFlag(FLAG_Z, false); // Always cleared
	setFlag(FLAG_N, false); // Always cleared
	setFlag(FLAG_H, false); // Always cleared
	temp_t_cycles += 4;
}
void CPU::jr(int8_t offset) {
	PC += offset; // Jump to PC + offset (signed 8-bit)
	temp_t_cycles += 12;
}
void CPU::rra() {
	uint8_t& A = AF.hi;
	uint8_t bit0 = A & 0x01;  // Get bit 0 (lowest bit of A)
	uint8_t carry = getFlag(FLAG_C); // Get current Carry flag value

	A = (A >> 1) | (carry << 7); // Shift A right and place carry in bit 7
	setFlag(FLAG_C, bit0); // Set Carry flag to bit 0 of A

	setFlag(FLAG_Z, false); // Always cleared
	setFlag(FLAG_N, false); // Always cleared
	setFlag(FLAG_H, false); // Always cleared
	temp_t_cycles += 4;
}
void CPU::jr_nz(int8_t r8) {
	// Check the Zero flag (Z)
	if (!getFlag(FLAG_Z)) {
		// Perform the relative jump if Z is not set (NZ)
		PC += r8;
		temp_t_cycles += 12;
	}
	else {
		// If the Zero flag is set, do not jump and just increment PC by 2
		PC += 2;
		temp_t_cycles += 8;
	}
}
void CPU::jr_nc(int8_t r8) {
	if (!getFlag(FLAG_C)) {
		PC += r8;
		temp_t_cycles += 12;
	}
	else {
		PC += 2;
		temp_t_cycles += 8;
	}
}
void CPU::jr_z(int8_t r8) {
	if (getFlag(FLAG_Z)) {
		PC += r8;
		temp_t_cycles += 12;
	}
	else {
		PC += 2; // Skip jump operand
		temp_t_cycles += 8;
	}
}
void CPU::daa() {
	uint8_t& A = AF.hi;
	bool N = getFlag(FLAG_N);
	bool H = getFlag(FLAG_H);
	bool C = getFlag(FLAG_C);

	uint8_t correction = 0;

	if (!N) { // after addition
		if (C || A > 0x99) {
			correction |= 0x60;
			setFlag(FLAG_C, true);
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

	setFlag(FLAG_Z, A == 0);
	setFlag(FLAG_H, false); // Always cleared
	// N is unchanged
	temp_t_cycles += 4;
}
void CPU::cpl() {
	AF.hi = ~AF.hi;         // A = NOT A
	setFlag(FLAG_N, true);  // Set N flag
	setFlag(FLAG_H, true);  // Set H flag
	temp_t_cycles += 4;
}
void CPU::inc_hl_ptr() {
	uint8_t value = wram[HL.full];
	uint8_t result = value + 1;

	// Set flags
	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, (value & 0x0F) + 1 > 0x0F);

	wram[HL.full] = result;
	temp_t_cycles += 12;
}
void CPU::dec_hl_ptr() {
	uint8_t value = wram[HL.full];
	uint8_t result = value - 1;

	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, (value & 0x0F) == 0);

	wram[HL.full] = result;
	temp_t_cycles += 12;
}
void CPU::ld_hl_d8() {
	uint8_t value = wram[PC + 1];  // Read immediate value
	wram[HL.full] = value;         // Store it at address pointed by HL
	PC += 2;                         // Advance PC (opcode + 1 byte)
	temp_t_cycles += 12;
}
void CPU::jr_c_s8() {
	int8_t offset = static_cast<int8_t>(wram[PC + 1]);
	if (getFlag(FLAG_C)) {
		PC += offset + 2;  // Include the size of the instruction
		temp_t_cycles += 12;
	}
	else {
		PC += 2;  // Skip over operand
		temp_t_cycles += 8;
	}
}
void CPU::add_hl_sp() {
	uint32_t result = HL.full + SP;

	setFlag(FLAG_N, false);
	setFlag(FLAG_H, ((HL.full & 0x0FFF) + (SP & 0x0FFF)) > 0x0FFF);
	setFlag(FLAG_C, result > 0xFFFF);

	HL.full = static_cast<uint16_t>(result);
	temp_t_cycles += 8;
}
void CPU::ld_a_hld() {
	AF.hi = wram[HL.full];  // Load byte from wram at HL into A
	HL.full--;            // Decrement HL after loading
	temp_t_cycles += 8;
}
void CPU::ccf() {
	setFlag(FLAG_N, false);  // Clear subtract
	setFlag(FLAG_H, false);  // Clear half-carry
	setFlag(FLAG_C, !getFlag(FLAG_C));  // Toggle carry
	temp_t_cycles += 4;
}
void CPU::ld_rr_r(RegisterPair pair, Register8 reg) {
	wram[pair.full] = reg;  // Store the value in B into wram at address HL
	temp_t_cycles += 8;
}
void CPU::add_r_r(Register8 reg1, Register8 reg2) {
	uint16_t result = getReg8(reg1) + getReg8(reg2);  // Add A and B with potential overflow

	getReg8(reg1) = static_cast<uint8_t>(result);  // Store result in A (only the lower 8 bits)



	// Update Zero flag (Z)
	setFlag(FLAG_Z, (getReg8(reg1) == 0));

	// Update Subtract flag (N) - Not used in addition, so we clear it
	setFlag(FLAG_N, false);

	// Update Half Carry flag (H)
	setFlag(FLAG_H, ((getReg8(reg1) & 0x0F) + (getReg8(reg2) & 0x0F)) > 0x0F);

	// Update Carry flag (C)
	setFlag(FLAG_C, (result > 0xFF));
	temp_t_cycles += 4;
}
void CPU::add_r_rr(Register8 reg, RegisterPair pair) {
	uint8_t value = wram[pair.full];         // Load value from wram
	uint16_t result = getReg8(reg) + value;             // Add with potential overflow

	// Update A with result
	getReg8(reg) = static_cast<uint8_t>(result);

	// Set flags
	setFlag(FLAG_Z, (getReg8(reg) == 0));                            // Zero flag
	setFlag(FLAG_N, false);                          // Subtract flag cleared
	setFlag(FLAG_H, ((getReg8(reg) & 0x0F) + (value & 0x0F)) > 0x0F); // Half Carry
	setFlag(FLAG_C, (result > 0xFF));                // Full Carry
	temp_t_cycles += 8;
}
void CPU::adc_r_r(Register8 reg1, Register8 reg2) {
	uint8_t carry = getFlag(FLAG_C) ? 1 : 0;
	uint16_t result = getReg8(reg1) + getReg8(reg2) + carry;

	// Half carry: check carry from bit 3
	setFlag(FLAG_H, ((getReg8(reg1) & 0x0F) + (getReg8(reg2) & 0x0F) + carry) > 0x0F);

	// Carry flag (from bit 7)
	setFlag(FLAG_C, result > 0xFF);

	getReg8(reg1) = static_cast<uint8_t>(result);

	// Zero flag
	setFlag(FLAG_Z, (getReg8(reg1) == 0));

	// Subtract flag is always cleared for ADC
	setFlag(FLAG_N, false);
	temp_t_cycles += 4;
}
void CPU::adc_r_rr(Register8 reg, RegisterPair pair) {
	uint8_t value = wram[pair.full];
	uint8_t carry = getFlag(FLAG_C) ? 1 : 0;
	uint16_t result = getReg8(reg) + value + carry;

	setFlag(FLAG_H, ((getReg8(reg) & 0x0F) + (value & 0x0F) + carry) > 0x0F);
	setFlag(FLAG_C, result > 0xFF);

	getReg8(reg) = static_cast<uint8_t>(result);

	setFlag(FLAG_Z, (getReg8(reg) == 0));
	setFlag(FLAG_N, false);
	temp_t_cycles += 8;
}
void CPU::sub_r(Register8 reg) {
	uint8_t result = getReg8(REG_A) - getReg8(reg);

	setFlag(FLAG_Z, (result == 0));
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, ((getReg8(REG_A) & 0x0F) < (getReg8(reg) & 0x0F)));
	setFlag(FLAG_C, (getReg8(REG_A) < getReg8(reg)));

	getReg8(REG_A) = result;
	temp_t_cycles += 4;
}
void CPU::sub_rr(RegisterPair pair) {
	uint8_t value = wram[pair.full];
	uint8_t result = getReg8(REG_A) - value;

	setFlag(FLAG_Z, (result == 0));
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, (getReg8(REG_A) & 0x0F) < (value & 0x0F));
	setFlag(FLAG_C, getReg8(REG_A) < value);

	getReg8(REG_A) = result;
	temp_t_cycles += 8;
}
void CPU::sbc_r_r(Register8 reg1, Register8 reg2) {
	uint8_t carry = getFlag(FLAG_C) ? 1 : 0;
	uint16_t result = getReg8(reg1) - getReg8(reg2) - carry;

	setFlag(FLAG_H, ((getReg8(reg1) & 0x0F) < ((getReg8(reg2) & 0x0F) + carry)));
	setFlag(FLAG_C, getReg8(reg1) < (getReg8(reg2) + carry));
	getReg8(REG_A) = static_cast<uint8_t>(result);
	setFlag(FLAG_Z, (getReg8(reg1) == 0));
	setFlag(FLAG_N, true);
	temp_t_cycles += 4;
}
void CPU::sbc_r_rr(Register8 reg, RegisterPair pair) {
	uint8_t value = wram[pair.full];
	uint8_t carry = getFlag(FLAG_C) ? 1 : 0;
	uint16_t result = getReg8(reg) - value - carry;

	setFlag(FLAG_H, ((getReg8(reg) & 0x0F) < ((value & 0x0F) + carry)));
	setFlag(FLAG_C, getReg8(reg) < (value + carry));
	getReg8(reg) = static_cast<uint8_t>(result);
	setFlag(FLAG_Z, (getReg8(reg) == 0));
	setFlag(FLAG_N, true);
	temp_t_cycles += 8;
}
void CPU::and_r_r(Register8 reg1, Register8 reg2) {
	getReg8(reg1) &= getReg8(reg2);
	setFlag(FLAG_Z, (getReg8(reg1) == 0));
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, true);
	setFlag(FLAG_C, false);
	temp_t_cycles += 4;
}
void CPU::and_r_rr(Register8 reg, RegisterPair pair) {
	uint8_t value = wram[pair.full];
	getReg8(reg) &= value;

	setFlag(FLAG_Z, (getReg8(reg) == 0));
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, true);
	setFlag(FLAG_C, false);
	temp_t_cycles += 8;
}
void CPU::xor_r_rr(Register8 reg, RegisterPair pair) {
	getReg8(reg) ^= wram[pair.full];

	setFlag(FLAG_Z, (getReg8(reg) == 0));
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, false);
	temp_t_cycles += 8;
}
void CPU::or_r_r(Register8 reg1, Register8 reg2) {
	getReg8(reg1) |= getReg8(reg2);

	setFlag(FLAG_Z, (getReg8(reg1) == 0));
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, false);
	temp_t_cycles += 4;
}
void CPU::or_r_rr(Register8 reg, RegisterPair pair) {
	getReg8(reg) |= wram[pair.full];

	setFlag(FLAG_Z, (getReg8(reg) == 0));
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, false);
	temp_t_cycles += 8;
}
void CPU::cp_r_r(Register8 reg1, Register8 reg2) {
	uint8_t result = getReg8(reg1) - getReg8(reg2);

	setFlag(FLAG_Z, (getReg8(reg1) == 0));
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, ((getReg8(reg1) & 0xF) < (getReg8(reg2) & 0xF))); // Borrow from bit 4
	setFlag(FLAG_C, (getReg8(reg1) < getReg8(reg2)));
	temp_t_cycles += 4;
}
void CPU::cp_r_rr(Register8 reg, RegisterPair pair) {
	uint8_t value = wram[pair.full];
	uint8_t result = getReg8(reg) - value;

	setFlag(FLAG_Z, (getReg8(reg) == 0));
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, ((getReg8(reg) & 0xF) < (value & 0xF))); // Half carry check (borrow)
	setFlag(FLAG_C, (getReg8(reg) < value));
	temp_t_cycles += 8;
}
void CPU::ret_nz() {
	if (!getFlag(FLAG_Z)) {
		uint8_t low = wram[SP++];
		uint8_t high = wram[SP++];
		PC = (high << 8) | low;
		temp_t_cycles += 12; // Extra temp_t_cycles for branching
	}
}
void CPU::pop_rr(RegisterPair pair) {
	pair.lo = wram[SP++];   // Load the lower byte from the stack into C
	pair.hi = wram[SP++];   // Load the higher byte from the stack into B
	temp_t_cycles += 12; // Add temp_t_cycles for stack operation
}
void CPU::jp_nz_a16() {
	if (!getFlag(FLAG_Z)) {
		uint8_t low = wram[PC++];
		uint8_t high = wram[PC++];
		PC = (high << 8) | low;
		temp_t_cycles += 16; // Extra temp_t_cycles for jumping
	}
	else {
		temp_t_cycles += 12; // If not jumping, just proceed
	}
}
void CPU::jp_a16() {
	uint8_t low = wram[PC++];
	uint8_t high = wram[PC++];
	PC = (high << 8) | low;
	temp_t_cycles += 16;
}
void CPU::call_nz_a16() {
	uint8_t low = wram[PC++];
	uint8_t high = wram[PC++];
	uint16_t addr = (high << 8) | low;

	if (!getFlag(FLAG_Z)) {
		SP--;
		wram[SP] = (PC >> 8) & 0xFF;  // High byte
		SP--;
		wram[SP] = PC & 0xFF;         // Low byte
		PC = addr;
		temp_t_cycles += 24;
	}
	else {
		temp_t_cycles += 12;
	}
}
void CPU::push_rr(RegisterPair pair) {
	SP--;
	wram[SP] = pair.hi;  // Push high byte (B)
	SP--;
	wram[SP] = pair.lo;  // Push low byte (C)
	temp_t_cycles += 16;

}
void CPU::add_a_d8() {
	uint8_t value = wram[PC++];
	uint16_t result = getReg8(REG_A) + value;

	// Flags
	setFlag(FLAG_Z, ((result & 0xFF) == 0));
	setFlag(FLAG_N, 0);
	setFlag(FLAG_H, ((getReg8(REG_A) & 0xF) + (value & 0xF)) > 0xF);
	setFlag(FLAG_C, result > 0xFF);

	getReg8(REG_A) = result & 0xFF;
	temp_t_cycles += 8;
}
void CPU::rst_0() {
	SP--;
	wram[SP] = (PC >> 8) & 0xFF; // Push high byte
	SP--;
	wram[SP] = PC & 0xFF;        // Push low byte
	PC = 0x00;
	temp_t_cycles += 16;
}
void CPU::ret_z() {
	if (!getFlag(FLAG_Z)) {
		uint8_t lo = wram[SP++];
		uint8_t hi = wram[SP++];
		PC = (hi << 8) | lo;
		temp_t_cycles += 20;
	}
	else {
		temp_t_cycles += 8;
	}
}
void CPU::ret() {
	uint16_t returnAddress = wram[SP++];
	returnAddress |= (wram[SP++] << 8);

	// Set PC to the popped address
	PC = returnAddress;

	temp_t_cycles += 16;
}
void CPU::jp_z() {
	uint16_t address = wram[PC++];
	address |= (wram[PC++] << 8);

	if (!getFlag(FLAG_Z)) {
		PC = address;
		temp_t_cycles += 16;
	}
	else {
		temp_t_cycles += 12;
	}
}
void CPU::call_z() {
	uint16_t address = wram[PC++];
	address |= (wram[PC++] << 8);

	if (!getFlag(FLAG_Z)) {
		// Push high byte first, then low
		wram[--SP] = (PC >> 8) & 0xFF;
		wram[--SP] = PC & 0xFF;
		PC = address;
		temp_t_cycles += 24;
	}
	else {
		temp_t_cycles += 12;
	}
}
void CPU::call() {
	uint16_t address = wram[PC++];
	address |= (wram[PC++] << 8);

	// Push current PC (after this instruction) onto the stack
	wram[--SP] = (PC >> 8) & 0xFF;
	wram[--SP] = PC & 0xFF;

	PC = address;
	temp_t_cycles += 24;
}
void CPU::adc_a_d8() {
	uint8_t d8 = wram[PC++];
	uint16_t result = getReg8(REG_A) + d8 + (getFlag(FLAG_C) ? 1 : 0);

	// Set the Zero flag if the result is 0
	setFlag(FLAG_Z, (result == 0));

	// Set the Subtract flag (always 0 for ADC)
	setFlag(FLAG_N, 0);

	// Set the Half Carry flag (carry between bit 3 and 4)
	setFlag(FLAG_H, ((getReg8(REG_A) & 0xF) + (d8 & 0xF) + (getFlag(FLAG_C) ? 1 : 0)) > 0xF);

	// Set the Carry flag if there's an overflow
	setFlag(FLAG_C, result > 0xFF);

	// Store the result in A (only the lower 8 bits)
	getReg8(REG_A) = result & 0xFF;

	temp_t_cycles += 8;
}
void CPU::rst_1() {
	// Push current PC onto the stack
	wram[--SP] = (PC >> 8) & 0xFF;  // Push high byte of PC
	wram[--SP] = PC & 0xFF;         // Push low byte of PC

	// Set PC to the restart address (0x08)
	PC = 0x08;

	temp_t_cycles += 16;
}
void CPU::ret_nc() {
	if (!getFlag(FLAG_C)) {
		uint8_t low = wram[SP++];
		uint8_t high = wram[SP++];
		PC = (high << 8) | low;
		temp_t_cycles += 20;
	}
	else {
		temp_t_cycles += 8;
	}
}
void CPU::jp_nc_a16() {
	uint16_t addr = wram[PC + 1] | (wram[PC + 2] << 8);

	if (!getFlag(FLAG_C)) {
		PC = addr;
		temp_t_cycles += 16;
	}
	else {
		PC += 3; // Move past this instruction if not jumping
		temp_t_cycles += 12;
	}
}
void CPU::call_nc_a16() {
	uint16_t addr = wram[PC + 1] | (wram[PC + 2] << 8);

	if (!getFlag(FLAG_C)) {
		uint16_t returnAddr = PC + 3;
		wram[--SP] = (returnAddr >> 8) & 0xFF; // High byte
		wram[--SP] = returnAddr & 0xFF;        // Low byte
		PC = addr;
		temp_t_cycles += 24;
	}
	else {
		PC += 3;
		temp_t_cycles += 12;
	}
}
void CPU::sub_d8() {
	uint8_t value = wram[PC + 1];
	uint8_t result = getReg8(REG_A) - value;

	setFlag(FLAG_Z, (result == 0));
	setFlag(FLAG_N, 1);
	setFlag(FLAG_H, ((getReg8(REG_A) & 0x0F) < (value & 0x0F)));
	setFlag(FLAG_C, (getReg8(REG_A) < value));

	getReg8(REG_A) = result;
	PC += 2;
	temp_t_cycles += 8;
}
void CPU::rst_2() {
	uint16_t returnAddr = PC + 1;
	wram[--SP] = (returnAddr >> 8) & 0xFF; // high byte
	wram[--SP] = returnAddr & 0xFF;        // low byte

	PC = 0x10;
	temp_t_cycles += 16;
}
void CPU::ret_c() {
	if (!getFlag(FLAG_C)) {
		uint8_t low = wram[SP++];
		uint8_t high = wram[SP++];
		PC = (high << 8) | low;
		temp_t_cycles += 20;
	}
	else {
		PC++;
		temp_t_cycles += 8;
	}
}
void CPU::reti() {
	uint8_t low = wram[SP++];
	uint8_t high = wram[SP++];
	PC = (high << 8) | low;

	IME = true; // Enable interrupts
	temp_t_cycles += 16;
}
void CPU::jp_c_a16() {
	uint16_t address = wram[PC + 1] | (wram[PC + 2] << 8);
	if (!getFlag(FLAG_C)) {
		PC = address;
		temp_t_cycles += 16;
	}
	else {
		PC += 3;
		temp_t_cycles += 12;
	}
}
void CPU::call_z_a16() {
	uint16_t address = wram[PC + 1] | (wram[PC + 2] << 8);
	if (!getFlag(FLAG_Z)) {
		uint16_t returnAddress = PC + 3;
		wram[--SP] = (returnAddress >> 8) & 0xFF;
		wram[--SP] = returnAddress & 0xFF;
		PC = address;
		temp_t_cycles += 24;
	}
	else {
		PC += 3;
		temp_t_cycles += 12;
	}
}
void CPU::sbc_a_d8() {
	uint8_t value = wram[PC + 1];
	uint8_t carry = getFlag(FLAG_C) ? 1 : 0;
	uint16_t result = getReg8(REG_A) - value - carry;

	setFlag(FLAG_Z, ((result & 0xFF) == 0));
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, ((getReg8(REG_A) & 0x0F) < ((value & 0x0F) + carry)));
	setFlag(FLAG_C, (getReg8(REG_A) < (value + carry)));

	getReg8(REG_A) = result & 0xFF;

	PC += 2;
	temp_t_cycles += 8;
}
void CPU::rst_3() {
	uint16_t returnAddress = PC;
	wram[--SP] = (returnAddress >> 8) & 0xFF; // Push high byte
	wram[--SP] = returnAddress & 0xFF;        // Push low byte

	PC = 0x18;
	temp_t_cycles += 16;
}
void CPU::ld_a_a8() {
	uint16_t address = wram[PC + 1] | (wram[PC + 2] << 8);
	wram[address] = getReg8(REG_A);

	PC += 3;
	temp_t_cycles += 16;
}

void CPU::and_d8() {
	uint8_t value = wram[PC + 1];
	getReg8(REG_A) &= value;

	setFlag(FLAG_Z, (getReg8(REG_A) == 0));
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, true);
	setFlag(FLAG_C, false);

	PC += 2;
	temp_t_cycles += 8;
}
void CPU::rst_4() {
	uint16_t returnAddress = PC;
	wram[--SP] = (returnAddress >> 8) & 0xFF; // Push high byte
	wram[--SP] = returnAddress & 0xFF;        // Push low byte

	PC = 0x20;
	temp_t_cycles += 16;
}
void CPU::add_sp_s8() {
	int8_t offset = static_cast<int8_t>(wram[PC + 1]); // Read the signed 8-bit immediate value
	uint16_t result = SP + offset;

	// Update the flags
	setFlag(FLAG_Z, false);  // Z is not affected by ADD SP
	setFlag(FLAG_N, false);  // N is reset
	setFlag(FLAG_H, ((SP & 0xF) + (offset & 0xF)) > 0xF);  // Carry from lower 4 bits
	setFlag(FLAG_C, (SP & 0xFF) + (offset & 0xFF) > 0xFF);  // Carry from lower 8 bits

	SP = result;
	PC += 2;
	temp_t_cycles += 16;
}
void CPU::ld_a16_a() {
	uint16_t address = (wram[PC + 1] << 8) | wram[PC + 2];  // Get the 16-bit address
	wram[address] = getReg8(REG_A);  // Store the value of A into the wram at address a16

	PC += 3;  // Move the program counter to the next instruction
	temp_t_cycles += 16;  // Add the number of temp_t_cycles for this instruction
}
void CPU::xor_d8() {
	uint8_t immediate = wram[PC + 1];  // Fetch the immediate byte (d8)
	getReg8(REG_A) ^= immediate;  // Perform XOR between A and d8

	// Set flags
	setFlag(FLAG_Z, (getReg8(REG_A) == 0));  // Zero flag set if result is zero
	setFlag(FLAG_N, false);  // No subtraction in XOR, so N flag is reset
	setFlag(FLAG_H, false);  // No half-carry in XOR
	setFlag(FLAG_C, false);     // No carry in XOR

	PC += 2;  // Move the program counter to the next instruction
	temp_t_cycles += 8;  // Add the number of temp_t_cycles for this instruction
}
void CPU::rst_5() {
	// Push the current PC onto the stack
	SP -= 2;  // Decrease the stack pointer
	wram[SP] = (PC >> 8) & 0xFF;  // Store the high byte of PC
	wram[SP + 1] = PC & 0xFF;     // Store the low byte of PC

	// Load the PC with the address for RST 5 (0x28)
	PC = 0x28;

	temp_t_cycles += 16;  // Add the number of temp_t_cycles for this instruction
}
void CPU::ld_a8_a() {
	uint16_t address = (wram[PC + 1] << 8) | wram[PC + 2];  // Get the 16-bit address (a8)
	getReg8(REG_A) = wram[address];  // Load the value from wram at address a8 into A

	PC += 3;  // Move the program counter to the next instruction
	temp_t_cycles += 13;  // Add the number of temp_t_cycles for this instruction
}
void CPU::ld_c_a() {
	uint16_t address = 0xFF00 + getFlag(FLAG_C);  // Calculate the wram address 0xFF00 + C
	getReg8(REG_A) = wram[address];  // Load the value at the address into A

	PC += 1;  // Move the program counter to the next instruction
	temp_t_cycles += 8;  // Add the number of temp_t_cycles for this instruction
}
void CPU::di() {
	interrupts_enabled = false;  // Disable interrupts

	PC += 1;  // Move the program counter to the next instruction
	temp_t_cycles += 4;  // Add the number of temp_t_cycles for this instruction
}
void CPU::or_d8() {
	uint8_t d8 = wram[PC + 1];  // Get the immediate byte d8
	getReg8(REG_A) |= d8;  // Perform bitwise OR between A and d8

	// Set the flags based on the result
	setFlag(FLAG_Z, (getReg8(REG_A) == 0));  // Zero flag is set if the result is 0
	setFlag(FLAG_N, false);          // Negative flag is reset
	setFlag(FLAG_H, false);          // Half carry flag is reset
	setFlag(FLAG_C, false);          // Carry flag is reset

	PC += 2;  // Move the program counter to the next instruction
	temp_t_cycles += 8;  // Add the number of temp_t_cycles for this instruction
}
void CPU::rst_6() {
	uint16_t return_address = PC + 1;  // The address to return to after the instruction is done
	SP -= 2;  // Decrement the stack pointer to make space for the return address
	wram[SP] = return_address >> 8;  // Push the high byte of the return address onto the stack
	wram[SP + 1] = return_address & 0xFF;  // Push the low byte of the return address onto the stack

	PC = 0x0030;  // Jump to the address for RST 6

	temp_t_cycles += 16;  // Add the number of temp_t_cycles for this instruction
}
void CPU::ld_hl_sp_s8() {
	int8_t s8 = wram[PC + 1];  // Get the signed 8-bit immediate value s8
	uint16_t result = SP + s8;   // Add the signed immediate value to the SP

	HL.full = result;  // Store the result in the HL register pair

	// Set the flags
	setFlag(FLAG_Z, (HL.full == 0));        // Zero flag is set if the result is 0
	setFlag(FLAG_N, false);                // Negative flag is reset
	setFlag(FLAG_H, ((SP ^ result ^ s8) & 0x10) != 0); // Half Carry flag
	setFlag(FLAG_C, (result > 0xFFFF)); // Carry flag is set if the result overflows

	PC += 2;  // Move the program counter to the next instruction
	temp_t_cycles += 12;  // Add the number of temp_t_cycles for this instruction
}
void CPU::ld_a_a16() {
	// Fetch the 16-bit address from the next two bytes in wram
	uint16_t address = wram[PC + 1] | (wram[PC + 2] << 8);

	// Load the value from wram at the given address into the A register
	getReg8(REG_A) = wram[address];

	// Increment the PC to move past the 3 bytes of this instruction
	PC += 3;

	temp_t_cycles += 16;  // Add the number of temp_t_cycles for this instruction
}
void CPU::ei() {
	IME = 1;  // Enable interrupts by setting the Interrupt Master Enable flag (IME)

	temp_t_cycles += 4;  // Add the number of temp_t_cycles for this instruction
	PC += 1;      // Move the program counter to the next instruction
}
void CPU::cp_d8() {
	uint8_t value = wram[PC + 1];  // Fetch the immediate byte d8

	// Subtract the value from A, but don't store the result in A
	uint16_t result = getReg8(REG_A) - value;

	// Update flags based on the result of the subtraction
	setFlag(FLAG_Z, (result == 0));  // Zero flag set if result is zero
	setFlag(FLAG_N, true);              // Subtract flag is always set
	setFlag(FLAG_H, ((getReg8(REG_A) & 0xF) < (value & 0xF)));  // Half-carry flag is set if there is a borrow from bit 4
	setFlag(FLAG_C, (getReg8(REG_A) < value));    // Carry flag is set if there is a borrow

	// Increment the PC by 2 to move past the instruction (0xFE + d8)
	PC += 2;

	temp_t_cycles += 8;  // Add the number of temp_t_cycles for this instruction
}
void CPU::rst_7() {
	// Push the current program counter (PC) onto the stack
	SP -= 2;  // Decrement SP by 2 to make room for the return address
	wram[SP] = (PC >> 8) & 0xFF;  // Store the high byte of PC
	wram[SP + 1] = PC & 0xFF;     // Store the low byte of PC

	// Jump to address 0x38 (reset address for RST 7)
	PC = 0x38;

	temp_t_cycles += 16;  // Add the number of temp_t_cycles for this instruction
}























































//main cpu stuff here now yay



uint8_t CPU::ReadByte(uint16_t addr) {
	if (addr >= 0xFF00 && addr <= 0xFF7F)
		return io[addr - 0xFF00];
	// ... rest of wram access logic
}


uint8_t CPU::readFromMem(uint16_t addr) {
	if (addr <= 0x7FFF) {
		return cart.rom_data[addr]; // ROM region
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF) {
		return vram[addr - 0x8000];
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		return eram[addr - 0xA000];
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {
		return wram[addr - 0xC000];
	}
	else if (addr >= 0xE000 && addr <= 0xFDFF) {
		return wram[addr - 0xE000]; // Echo of 0xC000–0xDDFF
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		return oam[addr - 0xFE00];
	}
	else if (addr >= 0xFF00 && addr <= 0xFF7F) {
		return io[addr - 0xFF00];
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFE) {
		return hram[addr - 0xFF80];
	}
	/*else if (addr == 0xFFFF) {
		return interrupt_enable;
	}*/
	return 0xFF; // Unusable or unimplemented area
}


void CPU::Push16(uint16_t value) {
	SP--;
	wram[SP] = (value >> 8) & 0xFF;
	SP--;
	wram[SP] = value & 0xFF;
}
void CPU::HandleInterrupt() {
	uint8_t triggered = IE & IF;
	if (triggered == 0)
		return;

	for (int i = 0; i < 5; ++i) {
		if (triggered & (1 << i)) {
			// Clear halted state
			halted = false;

			// Clear IF bit for the interrupt
			IF &= ~(1 << i);

			// Disable IME so we don't nest interrupts
			IME = false;

			// Push current PC to stack
			Push16(PC);

			// Jump to interrupt vector
			switch (i) {
			case 0: PC = 0x40; break; // V-Blank
			case 1: PC = 0x48; break; // LCD STAT
			case 2: PC = 0x50; break; // Timer
			case 3: PC = 0x58; break; // Serial
			case 4: PC = 0x60; break; // Joypad
			}

			return; // Only handle one interrupt per call
		}
	}
}


void CPU::ExecuteInstruction(uint8_t opcode) {

	size_t size = sizeof(cart.rom_data);
	//std::cout << "wram Used: " << size << "\n";
	if (size > 0x10000) {
		//std::cerr << "ERROR: wram Overflow!!" << "\n";
	}

	switch (opcode) {
	case 0x00: PC++; temp_t_cycles += 4; break; // NOP
	case 0x01: ld_rr_d16(BC); break; // LD BC,d16
	case 0x02: ld_addr_rr_a(BC); break; // LD (BC),A
	case 0x03: inc_rr(BC); break; // INC BC
	case 0x04: inc_r(REG_B); break; // INC B
	case 0x05: dec_r(REG_B); break; // DEC B
	case 0x06: ld_r_n(REG_B, wram[PC++]); break; // LD B, d8
	case 0x07: rlca(); break; // RLCA
	case 0x08: ld_a16_sp(); break; // LD (a16), SP
	case 0x09: add_hl_rr(BC); break; // ADD HL, BC
	case 0x0A: ld_a_addr_rr(BC); break; // LD A, (BC)
	case 0x0B: dec_rr(BC); break; // DEC BC
	case 0x0C: inc_r(REG_C); break; // INC C
	case 0x0D: dec_r(REG_C); break; // DEC C
	case 0x0E: ld_r_n(REG_C, wram[PC++]); break; // LD C, d8
	case 0x0F: rrca(); break; // RRCA



	case 0x10: halt(); break; // STOP 0
	case 0x11: ld_rr_d16(DE); break; // LD DE,d16
	case 0x12: ld_addr_rr_a(DE); break; // LD (DE),A
	case 0x13: inc_rr(DE); break; // INC DE
	case 0x14: inc_r(REG_D); break; // INC D
	case 0x15: dec_r(REG_D); break; // DEC D
	case 0x16: ld_r_n(REG_D, wram[PC++]); break; // LD D, d8
	case 0x17: rla(); break; // RLA
	case 0x18: // JR r8
	{
		int8_t r8 = static_cast<int8_t>(wram[PC + 1]); // Read the signed byte at PC + 1
		jr(r8);

	}
	case 0x19: add_hl_rr(DE); break; // ADD HL, DE
	case 0x1A: ld_a_addr_rr(DE); break; // LD A, (DE)
	case 0x1B: dec_rr(DE); break; // DEC DE
	case 0x1C: inc_r(REG_E); break; // INC E
	case 0x1D: dec_r(REG_E); break; // DEC E
	case 0x1E: ld_r_n(REG_E, wram[PC++]); break; // LD E, d8
	case 0x1F: rra(); break;
		


	case 0x20: // JR NZ, r8
	{
		int8_t r8 = static_cast<int8_t>(wram[PC + 1]); // Read the signed byte at PC + 1
		jr_nz(r8);

		break;
	}
	case 0x21: ld_rr_d16(HL); break; // LD DE,d16
	case 0x22: ld_addr_rr_a(HL); break; // LD (DE),A
	case 0x23: inc_rr(HL); break; // INC DE
	case 0x24: inc_r(REG_H); break; // INC D
	case 0x25: dec_r(REG_H); break; // DEC D
	case 0x26: ld_r_n(REG_H, wram[PC++]); break; // LD D, d8
	case 0x27: daa(); break;
	case 0x28: jr_z(static_cast<int8_t>(wram[PC + 1])); break;
	case 0x29: add_hl_rr(HL); break; // ADD HL, HL
	case 0x2A: AF.hi = wram[HL.full]; HL.full++; temp_t_cycles += 8;break;
	case 0x2B: dec_rr(HL); break; // DEC HL
	case 0x2C: inc_r(REG_L); break; // INC L
	case 0x2D: dec_r(REG_L); break; // DEC L
	case 0x2E: ld_r_n(REG_L, wram[PC++]); break; // LD L, d8
	case 0x2F: cpl(); break; // cpl



	case 0x30: jr_nc(static_cast<int8_t>(wram[PC + 1])); break;
	case 0x31: SP = wram[PC + 1] | (wram[PC + 2] << 8); PC += 3; temp_t_cycles += 12; break; // LD DE,d16
	case 0x32: wram[HL.full] = AF.hi; HL.full--; temp_t_cycles += 8; break; // LD (DE),A
	case 0x33: SP++; temp_t_cycles += 8; break; // INC DE
	case 0x34: inc_hl_ptr(); break; // INC D
	case 0x35: dec_hl_ptr(); break; // DEC D
	case 0x36: ld_hl_d8(); break; // LD D, d8
	case 0x37: setFlag(FLAG_C, true); setFlag(FLAG_N, false); setFlag(FLAG_H, false); temp_t_cycles += 4; break;
	case 0x38: jr_c_s8(); break;
	case 0x39: add_hl_sp(); break; // ADD HL, HL
	case 0x3A: ld_a_hld(); break;
	case 0x3B: SP--; temp_t_cycles += 8; break;
	case 0x3C: inc_r(REG_A); break; // INC L
	case 0x3D: dec_r(REG_A); break; // DEC L
	case 0x3E: ld_r_n(REG_A, wram[PC++]); break; // LD L, d8
	case 0x3F: ccf(); break; // cpl



	case 0x40: ld_r_r(REG_B, REG_B); break;
	case 0x41: ld_r_r(REG_B, REG_C); break;
	case 0x42: ld_r_r(REG_B, REG_D); break;
	case 0x43: ld_r_r(REG_B, REG_E); break;
	case 0x44: ld_r_r(REG_B, REG_H); break;
	case 0x45: ld_r_r(REG_B, REG_L); break;
	case 0x46: ld_r_nn(REG_B, HL); break;
	case 0x47: ld_r_r(REG_B, REG_A); break;
	case 0x48: ld_r_r(REG_C, REG_B); break;
	case 0x49: ld_r_r(REG_C, REG_C); break;
	case 0x4A: ld_r_r(REG_C, REG_D); break;
	case 0x4B: ld_r_r(REG_C, REG_E); break;
	case 0x4C: ld_r_r(REG_C, REG_H); break;
	case 0x4D: ld_r_r(REG_C, REG_L); break;
	case 0x4E: ld_r_nn(REG_C, HL); break;
	case 0x4F: ld_r_r(REG_C, REG_A); break;



	case 0x50: ld_r_r(REG_D, REG_B); break;
	case 0x51: ld_r_r(REG_D, REG_C); break;
	case 0x52: ld_r_r(REG_D, REG_D); break;
	case 0x53: ld_r_r(REG_D, REG_E); break;
	case 0x54: ld_r_r(REG_D, REG_H); break;
	case 0x55: ld_r_r(REG_D, REG_L); break;
	case 0x56: ld_r_nn(REG_D, HL); break;
	case 0x57: ld_r_r(REG_D, REG_A); break;
	case 0x58: ld_r_r(REG_E, REG_B); break;
	case 0x59: ld_r_r(REG_E, REG_C); break;
	case 0x5A: ld_r_r(REG_E, REG_D); break;
	case 0x5B: ld_r_r(REG_E, REG_E); break;
	case 0x5C: ld_r_r(REG_E, REG_H); break;
	case 0x5D: ld_r_r(REG_E, REG_L); break;
	case 0x5E: ld_r_nn(REG_E, HL); break;
	case 0x5F: ld_r_r(REG_E, REG_A); break;


	case 0x60: ld_r_r(REG_H, REG_B); break;
	case 0x61: ld_r_r(REG_H, REG_C); break;
	case 0x62: ld_r_r(REG_H, REG_D); break;
	case 0x63: ld_r_r(REG_H, REG_E); break;
	case 0x64: ld_r_r(REG_H, REG_H); break;
	case 0x65: ld_r_r(REG_H, REG_L); break;
	case 0x66: ld_r_nn(REG_H, HL); break;
	case 0x67: ld_r_r(REG_H, REG_A); break;
	case 0x68: ld_r_r(REG_L, REG_B); break;
	case 0x69: ld_r_r(REG_L, REG_C); break;
	case 0x6A: ld_r_r(REG_L, REG_D); break;
	case 0x6B: ld_r_r(REG_L, REG_E); break;
	case 0x6C: ld_r_r(REG_L, REG_H); break;
	case 0x6D: ld_r_r(REG_L, REG_L); break;
	case 0x6E: ld_r_nn(REG_L, HL); break;
	case 0x6F: ld_r_r(REG_L, REG_A); break;



	case 0x70: ld_rr_r(HL, REG_B); break;
	case 0x71: ld_rr_r(HL, REG_C); break;
	case 0x72: ld_rr_r(HL, REG_D); break;
	case 0x73: ld_rr_r(HL, REG_E); break;
	case 0x74: ld_rr_r(HL, REG_H); break;
	case 0x75: ld_rr_r(HL, REG_L); break;
	case 0x76: halt(); break;
	case 0x77: ld_rr_r(HL, REG_A); break;
	case 0x78: ld_r_r(REG_A, REG_B); break;
	case 0x79: ld_r_r(REG_A, REG_C); break;
	case 0x7A: ld_r_r(REG_A, REG_D); break;
	case 0x7B: ld_r_r(REG_A, REG_E); break;
	case 0x7C: ld_r_r(REG_A, REG_H); break;
	case 0x7D: ld_r_r(REG_A, REG_L); break;
	case 0x7E: ld_r_nn(REG_A, HL); break;
	case 0x7F: ld_r_r(REG_A, REG_A); break;



	case 0x80: add_r_r(REG_A, REG_B); break;
	case 0x81: add_r_r(REG_A, REG_C); break;
	case 0x82: add_r_r(REG_A, REG_D); break;
	case 0x83: add_r_r(REG_A, REG_E); break;
	case 0x84: add_r_r(REG_A, REG_H); break;
	case 0x85: add_r_r(REG_A, REG_L); break;
	case 0x86: add_r_rr(REG_A, HL); break;
	case 0x87: add_r_r(REG_A, REG_A); break;
	case 0x88: adc_r_r(REG_A, REG_B); break;
	case 0x89: adc_r_r(REG_A, REG_C); break;
	case 0x8A: adc_r_r(REG_A, REG_D); break;
	case 0x8B: adc_r_r(REG_A, REG_E); break;
	case 0x8C: adc_r_r(REG_A, REG_H); break;
	case 0x8D: adc_r_r(REG_A, REG_L); break;
	case 0x8E: adc_r_rr(REG_A, HL); break;
	case 0x8F: adc_r_r(REG_A, REG_A); break;



	case 0x90: sub_r(REG_B); break;
	case 0x91: sub_r(REG_C); break;
	case 0x92: sub_r(REG_D); break;
	case 0x93: sub_r(REG_E); break;
	case 0x94: sub_r(REG_H); break;
	case 0x95: sub_r(REG_L); break;
	case 0x96: sub_rr(HL); break;
	case 0x97: sub_r(REG_A); break;
	case 0x98: sbc_r_r(REG_A, REG_B); break;
	case 0x99: sbc_r_r(REG_A, REG_C); break;
	case 0x9A: sbc_r_r(REG_A, REG_D); break;
	case 0x9B: sbc_r_r(REG_A, REG_E); break;
	case 0x9C: sbc_r_r(REG_A, REG_H); break;
	case 0x9D: sbc_r_r(REG_A, REG_L); break;
	case 0x9E: sbc_r_rr(REG_A, HL); break;
	case 0x9F: sbc_r_r(REG_A, REG_A); break;



	case 0xA0: and_r_r(REG_A, REG_B); break;
	case 0xA1: and_r_r(REG_A, REG_C); break;
	case 0xA2: and_r_r(REG_A, REG_D); break;
	case 0xA3: and_r_r(REG_A, REG_E); break;
	case 0xA4: and_r_r(REG_A, REG_H); break;
	case 0xA5: and_r_r(REG_A, REG_L); break;
	case 0xA6: and_r_rr(REG_A, HL); break;
	case 0xA7: and_r_r(REG_A, REG_A); break;
	case 0xA8: xor_r_r(REG_A, REG_B); break;
	case 0xA9: xor_r_r(REG_A, REG_C); break;
	case 0xAA: xor_r_r(REG_A, REG_D); break;
	case 0xAB: xor_r_r(REG_A, REG_E); break;
	case 0xAC: xor_r_r(REG_A, REG_H); break;
	case 0xAD: xor_r_r(REG_A, REG_L); break;
	case 0xAE: xor_r_rr(REG_A, HL); break;
	case 0xAF: xor_r_r(REG_A, REG_A); break;



	case 0xB0: or_r_r(REG_A, REG_B); break;
	case 0xB1: or_r_r(REG_A, REG_C); break;
	case 0xB2: or_r_r(REG_A, REG_D); break;
	case 0xB3: or_r_r(REG_A, REG_E); break;
	case 0xB4: or_r_r(REG_A, REG_H); break;
	case 0xB5: or_r_r(REG_A, REG_L); break;
	case 0xB6: or_r_rr(REG_A, HL); break;
	case 0xB7: or_r_r(REG_A, REG_A); break;
	case 0xB8: cp_r_r(REG_A, REG_B); break;
	case 0xB9: cp_r_r(REG_A, REG_C); break;
	case 0xBA: cp_r_r(REG_A, REG_D); break;
	case 0xBB: cp_r_r(REG_A, REG_E); break;
	case 0xBC: cp_r_r(REG_A, REG_H); break;
	case 0xBD: cp_r_r(REG_A, REG_L); break;
	case 0xBE: cp_r_rr(REG_A, HL); break;
	case 0xBF: cp_r_r(REG_A, REG_A); break;



	case 0xC0: ret_nz(); break;
	case 0xC1: pop_rr(BC); break;
	case 0xC2: jp_nz_a16(); break;
	case 0xC3: jp_a16(); break;
	case 0xC4: call_nz_a16(); break;
	case 0xC5: push_rr(BC); break;
	case 0xC6: add_a_d8(); break;
	case 0xC7: rst_0(); break;
	case 0xC8: ret_z(); break;
	case 0xC9: ret(); break;
	case 0xCA: jp_z(); break;
	case 0xCB: /*nothing */ break;
	case 0xCC: call_z(); break;
	case 0xCD: call(); break;
	case 0xCE: adc_a_d8(); break;
	case 0xCF: rst_1(); break;



	case 0xD0: ret_nc(); break;
	case 0xD1: pop_rr(DE); break;
	case 0xD2: jp_nc_a16(); break;
	case 0xD3: /*nothing */ break;
	case 0xD4: call_nc_a16(); break;
	case 0xD5: push_rr(DE); break;
	case 0xD6: sub_d8(); break;
	case 0xD7: rst_2(); break;
	case 0xD8: ret_c(); break;
	case 0xD9: reti(); break;
	case 0xDA: jp_c_a16(); break;
	case 0xDB: /*nothing */ break;
	case 0xDC: call_z_a16(); break;
	case 0xDD: /*nothing */ break;
	case 0xDE: sbc_a_d8(); break;
	case 0xDF: rst_3(); break;



	case 0xE0: ld_a_a8(); break;
	case 0xE1: pop_rr(HL); break;
	case 0xE2: ld_c_a(); break;
	case 0xE3: /*nothing */ break;
	case 0xE4: /*nothing */ break;
	case 0xE5: push_rr(HL); break;
	case 0xE6: and_d8(); break;
	case 0xE7: rst_4(); break;
	case 0xE8: add_sp_s8(); break;
	case 0xE9: PC = HL.full; temp_t_cycles += 4; break;
	case 0xEA: ld_a16_a(); break;
	case 0xEE: xor_d8(); break;
	case 0xEF: rst_5(); break;



	case 0xF0: ld_a8_a(); break;
	case 0xF1: pop_rr(AF); break;
	case 0xF2: ld_c_a(); break;
	case 0xF3: di(); break;
	case 0xF4: /*nothing*/ break;
	case 0xF5: push_rr(AF); break;
	case 0xF6: or_d8(); break;
	case 0xF7: rst_6(); break;
	case 0xF8: ld_hl_sp_s8(); break;
	case 0xF9: SP = HL.full; temp_t_cycles += 8; break;
	case 0xFA: ld_a_a16(); break;
	case 0xFB: ei(); break;
	case 0xFC: /*nothing*/ break;
	case 0xFD: /*nothing*/ break;
	case 0xFE: cp_d8(); break;
	case 0xFF: rst_7(); break;



	default:
		std::cerr << "ERROR: Unknown opcode: 0x" << std::hex << (int)opcode << "\n";
		break;
	}

	//std::cout << "cycle: " << temp_t_cycles << " | total: "<< t_cycles << "\n";
}
void CPU::Cycle() {
	if (halted) {
		if ((IE & IF) != 0) {
			halted = false;
			if (IME)
				HandleInterrupt();
		}
		t_cycles += 4;
		m_cycles += 1;
		return;
	}

	uint8_t opcode = readFromMem(PC++);
	if (haltBug) {
		// Don't increment PC on this instruction
		opcode = readFromMem(PC++);
		haltBug = false;
	}
	else {
		opcode = readFromMem(PC++);
	}
	temp_t_cycles = 0; // Reset before instruction
	ExecuteInstruction(opcode);
	t_cycles += temp_t_cycles;
	m_cycles += temp_t_cycles / 4;
}


void CPU::d_PrintState() {
	std::cout << "INFO: A: " << std::hex << (int)AF.hi
		<< " B: " << (int)BC.hi
		<< " C: " << (int)BC.lo
		<< " D: " << (int)DE.hi
		<< " E: " << (int)DE.lo
		<< " F: " << (int)AF.lo
		<< " H: " << (int)HL.hi
		<< " L: " << (int)HL.lo
		<< " SP: " << (int)SP
		<< " PC: " << PC << "\n";
}