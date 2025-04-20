#include <instructions.h>
#include <ext_instructions.h>
#include <cpu.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <bus.h>

ExtInstruction::ExtInstruction() {

}
void ExtInstruction::ExecuteExtInstruction(uint8_t opcode) {
	uint8_t value, result, offset;
	uint16_t addr;

	cpu.temp_t_cycles += extendedInstructionTicks[opcode];

	switch (opcode) {
	case 0x00: rlc_r(insts.REG_B); break; // RLC B
	case 0x01: rlc_r(insts.REG_C); break; // RLC C
	case 0x02: rlc_r(insts.REG_D); break; // RLC D
	case 0x03: rlc_r(insts.REG_E); break; // RLC E
	case 0x04: rlc_r(insts.REG_H); break; // RLC H
	case 0x05: rlc_r(insts.REG_L); break; // RLC L
	case 0x06: rlc_rr(cpu.HL); break; // RLC (HL)
	case 0x07: rlc_r(insts.REG_A); break; // RLC A
	case 0x08: rrc_r(insts.REG_B); break; // RRC B
	case 0x09: rrc_r(insts.REG_C); break; // RRC C
	case 0x0A: rrc_r(insts.REG_D); break; // RRC D
	case 0x0B: rrc_r(insts.REG_E); break; // RRC E
	case 0x0C: rrc_r(insts.REG_H); break; // RRC H
	case 0x0D: rrc_r(insts.REG_L); break; // RRC L
	case 0x0E: rrc_rr(cpu.HL); break; // RRC (HL)
	case 0x0F: rrc_r(insts.REG_A); break; // RRC A



	case 0x10: rl_r(insts.REG_B); break; // RL B
	case 0x11: rl_r(insts.REG_C); break; // RL C
	case 0x12: rl_r(insts.REG_D); break; // RL D
	case 0x13: rl_r(insts.REG_E); break; // RL E
	case 0x14: rl_r(insts.REG_H); break; // RL H
	case 0x15: rl_r(insts.REG_L); break; // RL L
	case 0x16: rl_rr(cpu.HL); break; // RL (HL)
	case 0x17: rl_r(insts.REG_A); break; // RL A
	case 0x18: rr_r(insts.REG_B); break; // RR B
	case 0x19: rr_r(insts.REG_C); break; // RR C
	case 0x1A: rr_r(insts.REG_D); break; // RR D
	case 0x1B: rr_r(insts.REG_E); break; // RR E
	case 0x1C: rr_r(insts.REG_H); break; // RR H
	case 0x1D: rr_r(insts.REG_L); break; // RR L
	case 0x1E: rr_rr(cpu.HL); break; // RR (HL)
	case 0x1F: rr_r(insts.REG_A); break; // RR A


	case 0x20: sla_r(insts.REG_B); break; // SLA B
	case 0x21: sla_r(insts.REG_C); break; // SLA C
	case 0x22: sla_r(insts.REG_D); break; // SLA D
	case 0x23: sla_r(insts.REG_E); break; // SLA E
	case 0x24: sla_r(insts.REG_H); break; // SLA H
	case 0x25: sla_r(insts.REG_L); break; // SLA L
	case 0x26: sla_rr(cpu.HL); break; // SLA (HL)
	case 0x27: sla_r(insts.REG_A); break; // SLA A
	case 0x28: sra_r(insts.REG_B); break; // SRA B
	case 0x29: sra_r(insts.REG_C); break; // SRA C
	case 0x2A: sra_r(insts.REG_D); break; // SRA D
	case 0x2B: sra_r(insts.REG_E); break; // SRA E
	case 0x2C: sra_r(insts.REG_H); break; // SRA H
	case 0x2D: sra_r(insts.REG_L); break; // SRA L
	case 0x2E: sra_rr(cpu.HL); break; // SRA (HL)
	case 0x2F: sra_r(insts.REG_A); break; // SRA A



	case 0x30: swap_r(insts.REG_B); break; // SWAP B
	case 0x31: swap_r(insts.REG_C); break; // SWAP C
	case 0x32: swap_r(insts.REG_D); break; // SWAP D
	case 0x33: swap_r(insts.REG_E); break; // SWAP E
	case 0x34: swap_r(insts.REG_H); break; // SWAP H
	case 0x35: swap_r(insts.REG_L); break; // SWAP L
	case 0x36: swap_rr(cpu.HL); break; // SWAP (HL)
	case 0x37: swap_r(insts.REG_A); break; // SWAP A
	case 0x38: srl_r(insts.REG_B); break; // SRL B
	case 0x39: srl_r(insts.REG_C); break; // SRL C
	case 0x3A: srl_r(insts.REG_D); break; // SRL D
	case 0x3B: srl_r(insts.REG_E); break; // SRL E
	case 0x3C: srl_r(insts.REG_H); break; // SRL H
	case 0x3D: srl_r(insts.REG_L); break; // SRL L
	case 0x3E: srl_rr(cpu.HL); break; // SRL (HL)
	case 0x3F: srl_r(insts.REG_A); break; // SRL A



	default:

		printf("[ERROR] Unknown EXTENDED opcode: 0x%02x at 0x%04x | ", opcode, cpu.PC);
		//printf("DIV: %d\n", mmu->timer.div);
		printf("Cycles: %d\n", cpu.t_cycles);
		return;
		break;
	}
	printf("[INFO] EXTENDED Opcode 0x%02x at 0x%04x executed\n", opcode, cpu.PC);
}



void ExtInstruction::rlc_r(Instruction::Register8 reg) {
	uint8_t b = insts.getReg(reg);
	uint8_t bit7 = (b >> 7) & 1;
	uint8_t result = ((b << 1) | bit7) & 0xFF;

	insts.getReg(reg) = result;

	// Flags: Z 0 0 C
	insts.setFlag(cpu.FLAG_Z, result == 0);
	insts.setFlag(cpu.FLAG_N, false);
	insts.setFlag(cpu.FLAG_H, false);
	insts.setFlag(cpu.FLAG_C, bit7);
}
void ExtInstruction::rlc_rr(CPU::RegisterPair pair) {
	// Read from memory address in HL
	uint16_t address = pair.full;
	uint8_t value = bus.bus_read(address);

	// Perform rotation
	uint8_t bit7 = (value >> 7) & 1;
	uint8_t result = ((value << 1) | bit7) & 0xFF;

	// Write back to memory
	bus.bus_write(address, result);

	// Set flags
	insts.setFlag(cpu.FLAG_Z, result == 0);
	insts.setFlag(cpu.FLAG_N, false);
	insts.setFlag(cpu.FLAG_H, false);
	insts.setFlag(cpu.FLAG_C, bit7);
}
void ExtInstruction::rrc_r(Instruction::Register8 reg) {
	uint8_t b = insts.getReg(reg);
	uint8_t bit0 = b & 0x01;  // Save LSB
	uint8_t result = (b >> 1) | (bit0 << 7);  // Rotate right with wrap-around

	insts.getReg(reg) = result;

	// Flags: Z 0 0 C
	insts.setFlag(cpu.FLAG_Z, result == 0);
	insts.setFlag(cpu.FLAG_N, false);
	insts.setFlag(cpu.FLAG_H, false);
	insts.setFlag(cpu.FLAG_C, bit0);  // Carry = original bit0
}
void ExtInstruction::rrc_rr(CPU::RegisterPair pair) {
	// 1. Read from memory address in HL
	uint16_t address = pair.full;
	uint8_t value = bus.bus_read(address);

	// 2. Perform rotation
	uint8_t bit0 = value & 0x01;
	uint8_t result = (value >> 1) | (bit0 << 7);

	// 3. Write back to memory
	bus.bus_write(address, result);

	// 4. Set flags
	insts.setFlag(cpu.FLAG_Z, result == 0);  // Z if result is zero
	insts.setFlag(cpu.FLAG_N, false);        // N always 0
	insts.setFlag(cpu.FLAG_H, false);        // H always 0
	insts.setFlag(cpu.FLAG_C, bit0);         // C = original bit0
}
void ExtInstruction::rl_r(Instruction::Register8 reg) {
	uint8_t b = insts.getReg(reg);
	uint8_t carry = insts.getFlag(cpu.FLAG_C) ? 1 : 0;
	uint8_t new_carry = (b >> 7) & 1;  // Save MSB before rotation

	uint8_t result = ((b << 1) | carry) & 0xFF;
	insts.getReg(reg) = result;

	// Flags: Z 0 0 new_carry
	insts.setFlag(cpu.FLAG_Z, result == 0);
	insts.setFlag(cpu.FLAG_N, false);
	insts.setFlag(cpu.FLAG_H, false);
	insts.setFlag(cpu.FLAG_C, new_carry);
}
void ExtInstruction::rl_rr(CPU::RegisterPair pair) {
	// 1. Read from memory address in HL
	uint16_t address = pair.full;
	uint8_t value = bus.bus_read(address);

	// 2. Perform rotation through carry
	uint8_t carry_in = insts.getFlag(cpu.FLAG_C) ? 1 : 0;
	uint8_t carry_out = (value >> 7) & 1;
	uint8_t result = ((value << 1) | carry_in) & 0xFF;

	// 3. Write back to memory
	bus.bus_write(address, result);

	// 4. Set flags
	insts.setFlag(cpu.FLAG_Z, result == 0);
	insts.setFlag(cpu.FLAG_N, false);
	insts.setFlag(cpu.FLAG_H, false);
	insts.setFlag(cpu.FLAG_C, carry_out);
}
void ExtInstruction::rr_r(Instruction::Register8 reg) {
	uint8_t b = insts.getReg(reg);
	uint8_t carry_in = insts.getFlag(cpu.FLAG_C) ? 1 : 0;
	uint8_t bit0 = b & 0x01;  // Save LSB for carry out

	// Rotate right through carry
	uint8_t result = ((b >> 1) | (carry_in << 7)) & 0xFF;
	insts.getReg(reg) = result;

	// Flags: Z 0 0 C
	insts.setFlag(cpu.FLAG_Z, result == 0);
	insts.setFlag(cpu.FLAG_N, false);
	insts.setFlag(cpu.FLAG_H, false);
	insts.setFlag(cpu.FLAG_C, bit0);
}
void ExtInstruction::rr_rr(CPU::RegisterPair pair) {
	// 1. Read from memory address in HL
	uint16_t address = pair.full;
	uint8_t value = bus.bus_read(address);

	// 2. Perform rotation through carry
	uint8_t carry_in = insts.getFlag(cpu.FLAG_C) ? 1 : 0;
	uint8_t bit0 = value & 0x01;
	uint8_t result = ((value >> 1) | (carry_in << 7)) & 0xFF;

	// 3. Write back to memory
	bus.bus_write(address, result);

	// 4. Set flags
	insts.setFlag(cpu.FLAG_Z, result == 0);
	insts.setFlag(cpu.FLAG_N, false);
	insts.setFlag(cpu.FLAG_H, false);
	insts.setFlag(cpu.FLAG_C, bit0);
}
void ExtInstruction::sla_r(Instruction::Register8 reg) {
	uint8_t b = insts.getReg(reg);
	uint8_t new_carry = (b >> 7) & 1;  // Save MSB before shift

	// Shift left, filling LSB with 0
	uint8_t result = (b << 1) & 0xFF;
	insts.getReg(reg) = result;

	// Flags: Z 0 0 new_carry
	insts.setFlag(cpu.FLAG_Z, result == 0);
	insts.setFlag(cpu.FLAG_N, false);
	insts.setFlag(cpu.FLAG_H, false);
	insts.setFlag(cpu.FLAG_C, new_carry);
}
void ExtInstruction::sla_rr(CPU::RegisterPair pair) {
	// 1. Read from memory address in HL
	uint16_t address = pair.full;
	uint8_t value = bus.bus_read(address);

	// 2. Perform arithmetic left shift
	uint8_t carry_out = (value >> 7) & 1;
	uint8_t result = (value << 1) & 0xFF;

	// 3. Write back to memory
	bus.bus_write(address, result);

	// 4. Set flags
	insts.setFlag(cpu.FLAG_Z, result == 0);  // Z if result is zero
	insts.setFlag(cpu.FLAG_N, false);        // N always 0
	insts.setFlag(cpu.FLAG_H, false);        // H always 0
	insts.setFlag(cpu.FLAG_C, carry_out);    // C = original bit7
}
void ExtInstruction::sra_r(Instruction::Register8 reg) {
	uint8_t b = insts.getReg(reg);
	uint8_t msb = b & 0x80;  // Preserve sign bit
	uint8_t carry_out = b & 0x01;  // LSB becomes new carry

	// Arithmetic right shift (preserves sign bit)
	uint8_t result = (b >> 1) | msb;
	insts.getReg(reg) = result;

	// Flags: Z 0 0 C
	insts.setFlag(cpu.FLAG_Z, result == 0);
	insts.setFlag(cpu.FLAG_N, false);
	insts.setFlag(cpu.FLAG_H, false);
	insts.setFlag(cpu.FLAG_C, carry_out);
}
void ExtInstruction::sra_rr(CPU::RegisterPair pair) {
	// 1. Read from memory address in HL
	uint16_t address = pair.full;
	uint8_t value = bus.bus_read(address);

	// 2. Perform arithmetic right shift (preserves sign bit)
	uint8_t msb = value & 0x80;       // Save sign bit
	uint8_t carry_out = value & 0x01; // LSB becomes carry
	uint8_t result = (value >> 1) | msb;

	// 3. Write back to memory
	bus.bus_write(address, result);

	// 4. Set flags
	insts.setFlag(cpu.FLAG_Z, result == 0);  // Z if result is zero
	insts.setFlag(cpu.FLAG_N, false);        // N always 0
	insts.setFlag(cpu.FLAG_H, false);        // H always 0
	insts.setFlag(cpu.FLAG_C, carry_out);    // C = original bit0
}
void ExtInstruction::swap_r(Instruction::Register8 reg) {
	uint8_t b = insts.getReg(reg);

	// Swap upper and lower nibbles
	uint8_t result = ((b & 0x0F) << 4) | ((b & 0xF0) >> 4);
	insts.getReg(reg) = result;

	// Flags: Z 0 0 0
	insts.setFlag(cpu.FLAG_Z, result == 0);
	insts.setFlag(cpu.FLAG_N, false);
	insts.setFlag(cpu.FLAG_H, false);
	insts.setFlag(cpu.FLAG_C, false);
}
void ExtInstruction::swap_rr(CPU::RegisterPair pair) {
	// 1. Read from memory address in HL
	uint16_t address = pair.full;
	uint8_t value = bus.bus_read(address);

	// 2. Swap nibbles (high and low 4 bits)
	uint8_t result = ((value & 0x0F) << 4) | ((value & 0xF0) >> 4);

	// 3. Write back to memory
	bus.bus_write(address, result);

	// 4. Set flags
	insts.setFlag(cpu.FLAG_Z, result == 0);  // Z if result is zero
	insts.setFlag(cpu.FLAG_N, false);        // N always 0
	insts.setFlag(cpu.FLAG_H, false);        // H always 0
	insts.setFlag(cpu.FLAG_C, false);        // C always 0
}
void ExtInstruction::srl_r(Instruction::Register8 reg) {
	uint8_t b = insts.getReg(reg);
	uint8_t carry_out = b & 0x01;  // Save LSB for carry

	// Logical right shift (MSB becomes 0)
	uint8_t result = (b >> 1) & 0x7F;  // Clear new MSB
	insts.getReg(reg) = result;

	// Flags: Z 0 0 C
	insts.setFlag(cpu.FLAG_Z, result == 0);
	insts.setFlag(cpu.FLAG_N, false);
	insts.setFlag(cpu.FLAG_H, false);
	insts.setFlag(cpu.FLAG_C, carry_out);
}
void ExtInstruction::srl_rr(CPU::RegisterPair pair) {
	// 1. Read from memory address in HL
	uint16_t address = pair.full;
	uint8_t value = bus.bus_read(address);

	// 2. Perform logical right shift (MSB=0)
	uint8_t carry_out = value & 0x01;
	uint8_t result = (value >> 1) & 0x7F;  // Clear new MSB

	// 3. Write back to memory
	bus.bus_write(address, result);

	// 4. Set flags
	insts.setFlag(cpu.FLAG_Z, result == 0);  // Z if result is zero
	insts.setFlag(cpu.FLAG_N, false);        // N always 0
	insts.setFlag(cpu.FLAG_H, false);        // H always 0
	insts.setFlag(cpu.FLAG_C, carry_out);    // C = original bit0
}