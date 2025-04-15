#include <cstring>
#include <fstream>
#include <iostream>
#include <cpu.h>
#include <tools.h>
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
	// In a real emulator, we would halt execution here, entering a low-power state.
	// For this simulation, we just prevent any further instructions from running.
	// We can set a flag or simply stop execution.
	halted = true; // A flag to indicate that the CPU is halted

	// We would also typically handle entering low power state or idle mode here
	// depending on the emulation specifics.
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
}
void CPU::ld_r_nn(Register8 reg, RegisterPair pair) {
	getReg8(reg) = memory[pair.full];

}



void CPU::ld_r_r(Register8 dest, Register8 src) {
	getReg8(dest) = getReg8(src);
}
void CPU::ld_rr_d16(RegisterPair& reg) {
	uint8_t lo = memory[PC++];
	uint8_t hi = memory[PC++];
	reg.lo = lo;
	reg.hi = hi;
}


void CPU::ld_addr_rr_a(RegisterPair& reg) {
	uint16_t address = reg.full;
	memory[address] = AF.hi; // AF.hi = A
}
void CPU::xor_a(Register8 src) {
	AF.hi ^= getReg8(src);
	setFlag(FLAG_Z, AF.hi == 0);
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, false);
}
void CPU::inc_rr(RegisterPair& reg) {
	reg.full += 1;
}
void CPU::inc_r(Register8 reg) {
	uint8_t& r = getReg8(reg);
	uint8_t result = r + 1;

	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, (r & 0x0F) + 1 > 0x0F);

	r = result;
}
void CPU::dec_r(Register8 reg) {
	uint8_t& r = getReg8(reg);
	uint8_t result = r - 1;

	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, (r & 0x0F) == 0x00); // Borrow from bit 4?

	r = result;
}
void CPU::dec_rr(RegisterPair& reg) {
	reg.full -= 1;
}
void CPU::rlca() {
	uint8_t& A = AF.hi;
	uint8_t bit7 = (A & 0x80) >> 7; // Get bit 7

	A = (A << 1) | bit7;

	setFlag(FLAG_Z, false); // Always cleared
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, bit7);
}
void CPU::ld_a16_sp() {
	uint8_t lo = memory[PC++];
	uint8_t hi = memory[PC++];
	uint16_t addr = (hi << 8) | lo;

	memory[addr] = SP & 0xFF;       // low byte
	memory[addr + 1] = (SP >> 8) & 0xFF; // high byte
}
void CPU::add_hl_rr(const RegisterPair& rr) {
	uint32_t result = HL.full + rr.full;

	setFlag(FLAG_N, false);
	setFlag(FLAG_H, ((HL.full & 0x0FFF) + (rr.full & 0x0FFF)) > 0x0FFF);
	setFlag(FLAG_C, result > 0xFFFF);

	HL.full = result & 0xFFFF;
}
void CPU::ld_a_addr_rr(const RegisterPair& reg) {
	uint16_t address = reg.full;
	AF.hi = memory[address];  // Load value at address pointed by reg into A
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
}
void CPU::jr(int8_t offset) {
	PC += offset; // Jump to PC + offset (signed 8-bit)
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
}
void CPU::jr_nz(int8_t r8) {
	// Check the Zero flag (Z)
	if (!getFlag(FLAG_Z)) {
		// Perform the relative jump if Z is not set (NZ)
		PC += r8;
	}
	else {
		// If the Zero flag is set, do not jump and just increment PC by 2
		PC += 2;
	}
}
void CPU::jr_nc(int8_t r8) {
	if (!getFlag(FLAG_C)) {
		PC += r8;
	}
	else {
		PC += 2;
	}
}
void CPU::jr_z(int8_t r8) {
	if (getFlag(FLAG_Z)) {
		PC += r8;
	}
	else {
		PC += 2; // Skip jump operand
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
}
void CPU::cpl() {
	AF.hi = ~AF.hi;         // A = NOT A
	setFlag(FLAG_N, true);  // Set N flag
	setFlag(FLAG_H, true);  // Set H flag
}
void CPU::inc_hl_ptr() {
	uint8_t value = memory[HL.full];
	uint8_t result = value + 1;

	// Set flags
	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, (value & 0x0F) + 1 > 0x0F);

	memory[HL.full] = result;
}
void CPU::dec_hl_ptr() {
	uint8_t value = memory[HL.full];
	uint8_t result = value - 1;

	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, (value & 0x0F) == 0);

	memory[HL.full] = result;
}
void CPU::ld_hl_d8() {
	uint8_t value = memory[PC + 1];  // Read immediate value
	memory[HL.full] = value;         // Store it at address pointed by HL
	PC += 2;                         // Advance PC (opcode + 1 byte)
}
void CPU::jr_c_s8() {
	int8_t offset = static_cast<int8_t>(memory[PC + 1]);
	if (getFlag(FLAG_C)) {
		PC += offset + 2;  // Include the size of the instruction
		cycles += 12;
	}
	else {
		PC += 2;  // Skip over operand
		cycles += 8;
	}
}
void CPU::add_hl_sp() {
	uint32_t result = HL.full + SP;

	setFlag(FLAG_N, false);
	setFlag(FLAG_H, ((HL.full & 0x0FFF) + (SP & 0x0FFF)) > 0x0FFF);
	setFlag(FLAG_C, result > 0xFFFF);

	HL.full = static_cast<uint16_t>(result);
}
void CPU::ld_a_hld() {
	AF.hi = memory[HL.full];  // Load byte from memory at HL into A
	HL.full--;            // Decrement HL after loading
}
void CPU::ccf() {
	setFlag(FLAG_N, false);  // Clear subtract
	setFlag(FLAG_H, false);  // Clear half-carry
	setFlag(FLAG_C, !getFlag(FLAG_C));  // Toggle carry
}
void CPU::ld_rr_r(RegisterPair pair, Register8 reg) {
	memory[pair.full] = reg;  // Store the value in B into memory at address HL
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
}
void CPU::add_r_rr(Register8 reg, RegisterPair pair) {
	uint8_t value = memory[pair.full];         // Load value from memory
	uint16_t result = getReg8(reg) + value;             // Add with potential overflow

	// Update A with result
	getReg8(reg) = static_cast<uint8_t>(result);

	// Set flags
	setFlag(FLAG_Z, (getReg8(reg) == 0));                            // Zero flag
	setFlag(FLAG_N, false);                          // Subtract flag cleared
	setFlag(FLAG_H, ((getReg8(reg) & 0x0F) + (value & 0x0F)) > 0x0F); // Half Carry
	setFlag(FLAG_C, (result > 0xFF));                // Full Carry
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
}
void CPU::adc_r_rr(Register8 reg, RegisterPair pair) {
	uint8_t value = memory[pair.full];
	uint8_t carry = getFlag(FLAG_C) ? 1 : 0;
	uint16_t result = getReg8(reg) + value + carry;

	setFlag(FLAG_H, ((getReg8(reg) & 0x0F) + (value & 0x0F) + carry) > 0x0F);
	setFlag(FLAG_C, result > 0xFF);

	getReg8(reg) = static_cast<uint8_t>(result);

	setFlag(FLAG_Z, (getReg8(reg) == 0));
	setFlag(FLAG_N, false);
}








//main cpu stuff here now yay

bool CPU::LoadROM(std::string filename) {
	tools.RenderToDisplay("assets/loading");
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		
		std::cout << "Loading ROM..\n";
		// get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];




		// go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();
		if (size > 32768)
			std::cout << "Careful, rom size is higher than 32768 bytes! MBC doing magic here.";

		// load the rom into the gb's memory, starting at 0x0100 (256 bytes)
		for (long i = 0; i < size; ++i)
		{
			memory[0x0100 + i] = buffer[i];
		}

		// clean the buffer
		delete[] buffer;
		return true;
	}
	return false;

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
			SP--;
			memory[SP] = (PC >> 8) & 0xFF; // High byte
			SP--;
			memory[SP] = PC & 0xFF;        // Low byte

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

	if (halted) {
		if (IME && (IE & IF) != 0) {
			HandleInterrupt();
			return;
		}
	}

	switch (opcode) {
	case 0x00: break; // NOP
	case 0x01: ld_rr_d16(BC); break; // LD BC,d16
	case 0x02: ld_addr_rr_a(BC); break; // LD (BC),A
	case 0x03: inc_rr(BC); break; // INC BC
	case 0x04: inc_r(REG_B); break; // INC B
	case 0x05: dec_r(REG_B); break; // DEC B
	case 0x06: ld_r_n(REG_B, memory[PC++]); break; // LD B, d8
	case 0x07: rlca(); break; // RLCA
	case 0x08: ld_a16_sp(); break; // LD (a16), SP
	case 0x09: add_hl_rr(BC); break; // ADD HL, BC
	case 0x0A: ld_a_addr_rr(BC); break; // LD A, (BC)
	case 0x0B: dec_rr(BC); break; // DEC BC
	case 0x0C: inc_r(REG_C); break; // INC C
	case 0x0D: dec_r(REG_C); break; // DEC C
	case 0x0E: ld_r_n(REG_C, memory[PC++]); break; // LD C, d8
	case 0x0F: rrca(); break; // RRCA



	case 0x10: halt(); break; // STOP 0
	case 0x11: ld_rr_d16(DE); break; // LD DE,d16
	case 0x12: ld_addr_rr_a(DE); break; // LD (DE),A
	case 0x13: inc_rr(DE); break; // INC DE
	case 0x14: inc_r(REG_D); break; // INC D
	case 0x15: dec_r(REG_D); break; // DEC D
	case 0x16: ld_r_n(REG_D, memory[PC++]); break; // LD D, d8
	case 0x17: rla(); break; // RLA
	case 0x18: // JR r8
	{
		int8_t r8 = static_cast<int8_t>(memory[PC + 1]); // Read the signed byte at PC + 1
		jr(r8);
		PC += 2; // Since JR takes 2 bytes, move the program counter forward by 2
	}
	case 0x19: add_hl_rr(DE); break; // ADD HL, DE
	case 0x1A: ld_a_addr_rr(DE); break; // LD A, (DE)
	case 0x1B: dec_rr(DE); break; // DEC DE
	case 0x1C: inc_r(REG_E); break; // INC E
	case 0x1D: dec_r(REG_E); break; // DEC E
	case 0x1E: ld_r_n(REG_E, memory[PC++]); break; // LD E, d8
	case 0xFF: rra(); break;



	case 0x20: // JR NZ, r8
	{
		int8_t r8 = static_cast<int8_t>(memory[PC + 1]); // Read the signed byte at PC + 1
		jr_nz(r8);
		PC += 2; // Since JR takes 2 bytes, move the program counter forward by 2
		break;
	}
	case 0x21: ld_rr_d16(HL); break; // LD DE,d16
	case 0x22: ld_addr_rr_a(HL); break; // LD (DE),A
	case 0x23: inc_rr(HL); break; // INC DE
	case 0x24: inc_r(REG_H); break; // INC D
	case 0x25: dec_r(REG_H); break; // DEC D
	case 0x26: ld_r_n(REG_H, memory[PC++]); break; // LD D, d8
	case 0x27: daa(); break;
	case 0x28: jr_z(static_cast<int8_t>(memory[PC + 1])); break;
	case 0x29: add_hl_rr(HL); break; // ADD HL, HL
	case 0x2A: AF.hi = memory[HL.full]; HL.full++;
	case 0x2B: dec_rr(HL); break; // DEC HL
	case 0x2C: inc_r(REG_L); break; // INC L
	case 0x2D: dec_r(REG_L); break; // DEC L
	case 0x2E: ld_r_n(REG_L, memory[PC++]); break; // LD L, d8
	case 0x2F: cpl(); break; // cpl



	case 0x30: // JR NZ, r8
	{
		// Read the signed byte at PC + 1
		jr_nc(static_cast<int8_t>(memory[PC + 1]));
		PC += 2; // Since JR takes 2 bytes, move the program counter forward by 2
		break;
	}
	case 0x31: SP = memory[PC + 1] | (memory[PC + 2] << 8); PC += 3; break; // LD DE,d16
	case 0x32: memory[HL.full] = AF.hi; HL.full--; break; // LD (DE),A
	case 0x33: SP++; break; // INC DE
	case 0x34: inc_hl_ptr(); break; // INC D
	case 0x35: dec_hl_ptr(); break; // DEC D
	case 0x36: ld_hl_d8(); break; // LD D, d8
	case 0x37: setFlag(FLAG_C, true); setFlag(FLAG_N, false); setFlag(FLAG_H, false); break;
	case 0x38: jr_c_s8(); break;
	case 0x39: add_hl_sp(); break; // ADD HL, HL
	case 0x3A: { // LD A, (HL-)
		ld_a_hld();          // Then decrement HL
		cycles += 8;     // Instruction takes 8 cycles
		break;
	}
	case 0x3B: SP--; break;
	case 0x3C: inc_r(REG_A); break; // INC L
	case 0x3D: dec_r(REG_A); break; // DEC L
	case 0x3E: ld_r_n(REG_A, memory[PC++]); break; // LD L, d8
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



	case 0xAF: xor_a(REG_A); break;               // XOR A
		// Add more using helper voids...
	default:
		std::cerr << "Unknown opcode: 0x" << std::hex << (int)opcode << "\n";
		break;
	}
}
void CPU::Cycle() {
	uint8_t opcode = memory[PC++];
	ExecuteInstruction(opcode);
}


void CPU::d_PrintState() {
	std::cout << "A: " << std::hex << (int)AF.hi
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