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
void Instruction::add_hl_rr(const uint16_t rr) {
	uint32_t result = cpu.HL.full + rr;

	setFlag(cpu.FLAG_N, false);
	setFlag(cpu.FLAG_H, ((cpu.HL.full & 0x0FFF) + (rr & 0x0FFF)) > 0x0FFF);
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
	bool conditionMet = getFlag(flag);
	if (ifNot) conditionMet = !conditionMet;

	// Always consume base cycles (whether jump is taken or not)
	cpu.temp_t_cycles += 8;

	if (conditionMet) {
		cpu.PC += r8;  // Apply relative offset
		cpu.temp_t_cycles += 4;  // Additional cycles for taken jump (total 12)
	}

	// Always advance past the instruction (2 bytes)
	cpu.PC += 2;
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
void Instruction::ld_r_r(Register8 dest, Register8 src) {
	getReg(dest) = getReg(src);
}
void Instruction::ld_r_rr(Register8 reg, CPU::RegisterPair pair) {
	getReg(reg) = bus.bus_read(pair.full);
}
void Instruction::ld_rr_r(CPU::RegisterPair pair, Register8 reg) {
	bus.bus_write(pair.full, reg);  // Store the value in B into memory at address HL
}
void Instruction::add_r_r(Register8 reg1, uint8_t reg2) {
    uint8_t reg1_val = getReg(reg1);  // Store original values before modification
    uint8_t reg2_val = reg2;
    uint16_t result = reg1_val + reg2_val;

    getReg(reg1) = static_cast<uint8_t>(result);  // Store 8-bit result

    // Set flags
    setFlag(cpu.FLAG_Z, (getReg(reg1) == 0));
    setFlag(cpu.FLAG_N, false);
    // FIXED: Use original values for half-carry check
    setFlag(cpu.FLAG_H, ((reg1_val & 0x0F) + (reg2_val & 0x0F) > 0x0F));
    setFlag(cpu.FLAG_C, (result > 0xFF));
}
void Instruction::add_r_rr(Register8 reg, CPU::RegisterPair pair) {
    uint8_t regValue = getReg(reg);          // Get register value first
    uint8_t memValue = bus.bus_read(pair.full); // Read from memory address in pair
    uint16_t result = regValue + memValue;   // Calculate result with potential overflow

    // Update register with 8-bit result
    getReg(reg) = static_cast<uint8_t>(result);

    // Set flags
    setFlag(cpu.FLAG_Z, (getReg(reg) == 0)); // Zero flag (check 8-bit result)
    setFlag(cpu.FLAG_N, false);               // Subtract flag cleared
    // Half-carry: check bit 3 carry
    setFlag(cpu.FLAG_H, ((regValue & 0xF) + (memValue & 0xF)) > 0xF);
    // Full carry: check bit 7 carry
    setFlag(cpu.FLAG_C, (result > 0xFF));
}
void Instruction::adc_r_r(Register8 reg1, uint8_t reg2) {
	uint8_t reg1_val = getReg(reg1);  // Store original values
	uint8_t reg2_val = reg2;
	uint8_t carry = getFlag(cpu.FLAG_C) ? 1 : 0;
	uint16_t result = reg1_val + reg2_val + carry;

	// Set flags BEFORE modifying register
	setFlag(cpu.FLAG_H, ((reg1_val & 0x0F) + (reg2_val & 0x0F) + carry) > 0x0F);
	setFlag(cpu.FLAG_C, result > 0xFF);

	getReg(reg1) = static_cast<uint8_t>(result);  // Store result
	setFlag(cpu.FLAG_Z, (getReg(reg1) == 0));    // Check Z after store
	setFlag(cpu.FLAG_N, false);
}
void Instruction::adc_r_rr(Register8 reg, CPU::RegisterPair pair) {
	uint8_t reg_val = getReg(reg);          // Store original register value
	uint8_t mem_val = bus.bus_read(pair.full);
	uint8_t carry = getFlag(cpu.FLAG_C) ? 1 : 0;
	uint16_t result = reg_val + mem_val + carry;

	// Set flags BEFORE modifying register
	setFlag(cpu.FLAG_H, ((reg_val & 0x0F) + (mem_val & 0x0F) + carry) > 0x0F);
	setFlag(cpu.FLAG_C, result > 0xFF);

	getReg(reg) = static_cast<uint8_t>(result);
	setFlag(cpu.FLAG_Z, (getReg(reg) == 0));
	setFlag(cpu.FLAG_N, false);
}
void Instruction::sub_r_r(Register8 reg, uint8_t reg2) {
	uint8_t a = getReg(reg);  // Store original A value
	uint8_t b = reg2;
	uint8_t result = a - b;

	setFlag(cpu.FLAG_Z, (result == 0));
	setFlag(cpu.FLAG_N, true);
	// FIXED: Half-carry checks borrow from bit 4
	setFlag(cpu.FLAG_H, ((a & 0x0F) - (b & 0x0F)) < 0);
	// FIXED: Carry checks borrow from bit 7
	setFlag(cpu.FLAG_C, a < b);

	getReg(REG_A) = result;
}
void Instruction::sub_rr(CPU::RegisterPair pair) {
	uint8_t a = getReg(REG_A);  // Store original A value
	uint8_t b = bus.bus_read(pair.full);
	uint8_t result = a - b;

	setFlag(cpu.FLAG_Z, (result == 0));
	setFlag(cpu.FLAG_N, true);
	// FIXED: Half-carry checks borrow from bit 4
	setFlag(cpu.FLAG_H, ((a & 0x0F) - (b & 0x0F)) < 0);
	// FIXED: Carry checks borrow from bit 7
	setFlag(cpu.FLAG_C, a < b);

	getReg(REG_A) = result;
}
void Instruction::sbc_r_r(Register8 reg1, uint8_t reg2) {
	uint8_t a = getReg(reg1);  // Cache original values
	uint8_t b = reg2;
	uint8_t carry = getFlag(cpu.FLAG_C) ? 1 : 0;
	uint16_t result = a - b - carry;

	// Set flags BEFORE modifying register!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	setFlag(cpu.FLAG_H, ((a & 0x0F) - (b & 0x0F) - carry < 0));  // Half-carry (bit 4 borrow)
	setFlag(cpu.FLAG_C, result > 0xFF);  // Full carry (bit 7 borrow)

	getReg(reg1) = static_cast<uint8_t>(result);
	setFlag(cpu.FLAG_Z, (getReg(reg1) == 0));  // Zero flag after store
	setFlag(cpu.FLAG_N, true);
}
void Instruction::sbc_r_rr(Register8 reg, CPU::RegisterPair pair) {
	uint8_t a = getReg(reg);  // Cache original register value
	uint8_t b = bus.bus_read(pair.full);
	uint8_t carry = getFlag(cpu.FLAG_C) ? 1 : 0;
	uint16_t result = a - b - carry;

	// set flags BEFORE modifying register
	setFlag(cpu.FLAG_H, ((a & 0x0F) - (b & 0x0F) - carry < 0));  // Half-carry
	setFlag(cpu.FLAG_C, result > 0xFF);  // Full carry

	getReg(reg) = static_cast<uint8_t>(result);
	setFlag(cpu.FLAG_Z, (getReg(reg) == 0));
	setFlag(cpu.FLAG_N, true);
}
void Instruction::and_r_r(Register8 reg1, uint8_t reg2) {
	getReg(reg1) &= reg2;
	setFlag(cpu.FLAG_Z, (getReg(reg1) == 0));
	setFlag(cpu.FLAG_N, false);
	setFlag(cpu.FLAG_H, true);
	setFlag(cpu.FLAG_C, false);
}
void Instruction::and_r_rr(Register8 reg, CPU::RegisterPair pair) {
	uint8_t value = bus.bus_read(pair.full);
	getReg(reg) &= value;

	setFlag(cpu.FLAG_Z, (getReg(reg) == 0));
	setFlag(cpu.FLAG_N, false);
	setFlag(cpu.FLAG_H, true);
	setFlag(cpu.FLAG_C, false);

}
void Instruction::xor_r_r(Register8 reg1, uint8_t reg2) {
	getReg(reg1) ^= reg2;
	setFlag(cpu.FLAG_Z, (getReg(reg1) == 0));
	setFlag(cpu.FLAG_N, false);
	setFlag(cpu.FLAG_H, false);
	setFlag(cpu.FLAG_C, false);
}
void Instruction::xor_r_rr(Register8 reg, CPU::RegisterPair pair) {
	getReg(reg) ^= bus.bus_read(pair.full);

	setFlag(cpu.FLAG_Z, (getReg(reg) == 0));
	setFlag(cpu.FLAG_N, false);
	setFlag(cpu.FLAG_H, false);
	setFlag(cpu.FLAG_C, false);
}
void Instruction::or_r_r(Register8 reg1, uint8_t reg2) {
	getReg(reg1) |= reg2;

	setFlag(cpu.FLAG_Z, (getReg(reg1) == 0));
	setFlag(cpu.FLAG_N, false);
	setFlag(cpu.FLAG_H, false);
	setFlag(cpu.FLAG_C, false);
}
void Instruction::or_r_rr(Register8 reg, CPU::RegisterPair pair) {
	getReg(reg) |= bus.bus_read(pair.full);

	setFlag(cpu.FLAG_Z, (getReg(reg) == 0));
	setFlag(cpu.FLAG_N, false);
	setFlag(cpu.FLAG_H, false);
	setFlag(cpu.FLAG_C, false);
}
void Instruction::cp_r_r(Register8 reg1, uint8_t reg2) {
	uint8_t a = getReg(reg1);  // Store original values
	uint8_t b = reg2;
	uint8_t result = a - b;

	// Set flags (don't modify registers)
	setFlag(cpu.FLAG_Z, (result == 0));  // ZERO: Set if result is 0
	setFlag(cpu.FLAG_N, true);           // SUB: Always set for CP
	// HALF-CARRY: Check borrow from bit 4
	setFlag(cpu.FLAG_H, ((a & 0xF) - (b & 0xF)) < 0);
	// CARRY: Check borrow from bit 7
	setFlag(cpu.FLAG_C, a < b);
}
void Instruction::cp_r_rr(Register8 reg, CPU::RegisterPair pair) {
	uint8_t a = getReg(reg);          // Store original register value
	uint8_t b = bus.bus_read(pair.full);
	uint8_t result = a - b;

	// Set flags (don't modify registers)
	setFlag(cpu.FLAG_Z, (result == 0));
	setFlag(cpu.FLAG_N, true);
	// HALF-CARRY: Check borrow from bit 4
	setFlag(cpu.FLAG_H, ((a & 0xF) - (b & 0xF)) < 0);
	// CARRY: Check borrow from bit 7
	setFlag(cpu.FLAG_C, a < b);
}
void Instruction::ret(CPU::Flag flag, bool ifNot) {
	// Always consume 8 cycles (minimum for checking condition)
	cpu.temp_t_cycles += 8;

	bool conditionMet = getFlag(flag);
	if (ifNot) {
		conditionMet = !conditionMet;
	}

	if (conditionMet) {
		// Read return address from stack (little-endian)
		uint8_t lo = bus.bus_read(cpu.SP++);
		uint8_t hi = bus.bus_read(cpu.SP++);
		cpu.PC = (hi << 8) | lo;

		// Additional 12 cycles for actual return (total 20)
		cpu.temp_t_cycles += 12;
	}
}
void Instruction::pop_rr(CPU::RegisterPair& pair) {
	pair.lo = bus.bus_read(cpu.SP++);   // Load the lower byte from the stack into C
	pair.hi = bus.bus_read(cpu.SP++);   // Load the higher byte from the stack into B
}
void Instruction::jp_f(uint16_t address, CPU::Flag flag, bool ifNot) {
	bool conditionMet = getFlag(flag);
	if (ifNot) conditionMet = !conditionMet;

	if (conditionMet) {
		cpu.PC = address;  // Absolute jump
		cpu.temp_t_cycles += 16;  // Total cycles for taken jump
	}
	else {
		cpu.temp_t_cycles += 12;  // Cycles for not taken
	}
}
void Instruction::jp_a16() {
	uint8_t low = bus.bus_read(cpu.PC++);
	uint8_t high = bus.bus_read(cpu.PC++);
	cpu.PC = (high << 8) | low;
}
void Instruction::call_f(uint16_t address, CPU::Flag flag, bool ifNot) {
	// Always consume 12 cycles (minimum for checking condition)
	cpu.temp_t_cycles += 12;

	bool conditionMet = getFlag(flag);
	if (ifNot) conditionMet = !conditionMet;

	if (conditionMet) {
		// Push return address onto stack (little-endian)
		cpu.SP--;
		bus.bus_write(cpu.SP--, (cpu.PC >> 8) & 0xFF);  // High byte
		bus.bus_write(cpu.SP, cpu.PC & 0xFF);           // Low byte

		// Jump to target address
		cpu.PC = address;

		// Additional 12 cycles for actual call (total 24)
		cpu.temp_t_cycles += 12;
	}
}
void Instruction::push_rr(CPU::RegisterPair pair) {
	bus.bus_write(--cpu.SP, pair.hi);  // Push high byte first (e.g., B for BC)
	bus.bus_write(--cpu.SP, pair.lo);
}
void Instruction::rst_addr(uint16_t address) {
	// 1. Push return address onto stack (little-endian)
	cpu.SP--;
	bus.bus_write(cpu.SP, (cpu.PC >> 8) & 0xFF);  // High byte
	cpu.SP--;
	bus.bus_write(cpu.SP, cpu.PC & 0xFF);         // Low byte

	// 2. Jump to fixed address
	cpu.PC = address;	
}
void Instruction::o_ret() {
	// Read the return address from the stack (little-endian)
	uint8_t low = bus.bus_read(cpu.SP++);
	uint8_t high = bus.bus_read(cpu.SP++);

	uint16_t returnAddress = (high << 8) | low;

	// Jump to the return address
	cpu.PC = returnAddress;
}

void Instruction::cb_prefix() {
	uint8_t opcode = bus.bus_read(cpu.PC++);
	//ExtendedInsts_Execute(opcode); <-- implement that later.
}
void Instruction::call_a16() {
	// Fetch the 16-bit address from the instruction
	uint8_t low = bus.bus_read(cpu.PC++);
	uint8_t high = bus.bus_read(cpu.PC++);
	uint16_t address = (high << 8) | low;

	// Push current PC onto the stack (high first because Game Boy is little-endian stack-wise)
	cpu.SP--;
	bus.bus_write(cpu.SP, (cpu.PC >> 8) & 0xFF); // High byte
	cpu.SP--;
	bus.bus_write(cpu.SP, cpu.PC & 0xFF);        // Low byte

	// Jump to the new address
	cpu.PC = address;
}
void Instruction::add_sp_r8(int8_t r8) {
    uint16_t sp = cpu.SP;
    uint16_t result = sp + r8;

    // Flags are cleared (Z and N always 0)
    setFlag(cpu.FLAG_Z, false);
    setFlag(cpu.FLAG_N, false);

    // Correct carry/half-carry calculations (16-bit!)
    setFlag(cpu.FLAG_H, ((sp & 0x0F) + (r8 & 0x0F)) > 0x0F);
    setFlag(cpu.FLAG_C, ((sp & 0xFF) + (r8 & 0xFF)) > 0xFF);

    cpu.SP = result;
    cpu.temp_t_cycles += 16;  // 16 cycles total
}
void Instruction::ld_a16_a() {
	// Read 16-bit address (little-endian)
	uint8_t low = bus.bus_read(cpu.PC++);
	uint8_t high = bus.bus_read(cpu.PC++);
	uint16_t address = (high << 8) | low;

	// Write A to memory
	bus.bus_write(address, getReg(REG_A));

}
void Instruction::ld_hl_sp_r8() {
	   // 1. Read signed offset (1 byte)
    int8_t offset = static_cast<int8_t>(bus.bus_read(cpu.PC++));
    
    // 2. Calculate 16-bit result (SP + offset)
    uint32_t result = cpu.SP + offset;
    
    // 3. Store in HL (16-bit)
    cpu.HL.full = static_cast<uint16_t>(result);
    
    // 4. Set flags
    setFlag(cpu.FLAG_Z, false);  // Always 0
    setFlag(cpu.FLAG_N, false);  // Always 0
    setFlag(cpu.FLAG_H, ((cpu.SP ^ offset ^ (result & 0xFFFF)) & 0x10) != 0);
    setFlag(cpu.FLAG_C, (result & 0x10000) != 0);
}
void Instruction::ld_a_a16() {
	// Read 16-bit address (little-endian)
	uint8_t low = bus.bus_read(cpu.PC++);
	uint8_t high = bus.bus_read(cpu.PC++);
	uint16_t address = (high << 8) | low;

	// Load from memory into A
	getReg(REG_A) = bus.bus_read(address);

}




Instruction::Instruction() {

}


