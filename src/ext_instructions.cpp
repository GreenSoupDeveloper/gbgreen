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



	case 0x40: bit_n_r(0, insts.REG_B); break; // BIT 0, B
	case 0x41: bit_n_r(0, insts.REG_C); break; // BIT 0, C
	case 0x42: bit_n_r(0, insts.REG_D); break; // BIT 0, D
	case 0x43: bit_n_r(0, insts.REG_E); break; // BIT 0, E
	case 0x44: bit_n_r(0, insts.REG_H); break; // BIT 0, H
	case 0x45: bit_n_r(0, insts.REG_L); break; // BIT 0, L
	case 0x46: bit_n_rr(0, cpu.HL); break; // BIT 0, (HL)
	case 0x47: bit_n_r(0, insts.REG_A); break; // BIT 0, A
	case 0x48: bit_n_r(1, insts.REG_B); break; // BIT 1, B
	case 0x49: bit_n_r(1, insts.REG_C); break; // BIT 1, C
	case 0x4A: bit_n_r(1, insts.REG_D); break; // BIT 1, D
	case 0x4B: bit_n_r(1, insts.REG_E); break; // BIT 1, E
	case 0x4C: bit_n_r(1, insts.REG_H); break; // BIT 1, H
	case 0x4D: bit_n_r(1, insts.REG_L); break; // BIT 1, L
	case 0x4E: bit_n_rr(1, cpu.HL); break; // BIT 1, (HL)
	case 0x4F: bit_n_r(1, insts.REG_A); break; // BIT 1, A



	case 0x50: bit_n_r(2, insts.REG_B); break; // BIT 2, B
	case 0x51: bit_n_r(2, insts.REG_C); break; // BIT 2, C
	case 0x52: bit_n_r(2, insts.REG_D); break; // BIT 2, D
	case 0x53: bit_n_r(2, insts.REG_E); break; // BIT 2, E
	case 0x54: bit_n_r(2, insts.REG_H); break; // BIT 2, H
	case 0x55: bit_n_r(2, insts.REG_L); break; // BIT 2, L
	case 0x56: bit_n_rr(2, cpu.HL); break; // BIT 2, (HL)
	case 0x57: bit_n_r(2, insts.REG_A); break; // BIT 2, A
	case 0x58: bit_n_r(3, insts.REG_B); break; // BIT 3, B
	case 0x59: bit_n_r(3, insts.REG_C); break; // BIT 3, C
	case 0x5A: bit_n_r(3, insts.REG_D); break; // BIT 3, D
	case 0x5B: bit_n_r(3, insts.REG_E); break; // BIT 3, E
	case 0x5C: bit_n_r(3, insts.REG_H); break; // BIT 3, H
	case 0x5D: bit_n_r(3, insts.REG_L); break; // BIT 3, L
	case 0x5E: bit_n_rr(3, cpu.HL); break; // BIT 3, (HL)
	case 0x5F: bit_n_r(3, insts.REG_A); break; // BIT 3, A



	case 0x60: bit_n_r(4, insts.REG_B); break; // BIT 4, B
	case 0x61: bit_n_r(4, insts.REG_C); break; // BIT 4, C
	case 0x62: bit_n_r(4, insts.REG_D); break; // BIT 4, D
	case 0x63: bit_n_r(4, insts.REG_E); break; // BIT 4, E
	case 0x64: bit_n_r(4, insts.REG_H); break; // BIT 4, H
	case 0x65: bit_n_r(4, insts.REG_L); break; // BIT 4, L
	case 0x66: bit_n_rr(4, cpu.HL); break; // BIT 4, (HL)
	case 0x67: bit_n_r(4, insts.REG_A); break; // BIT 4, A
	case 0x68: bit_n_r(5, insts.REG_B); break; // BIT 5, B
	case 0x69: bit_n_r(5, insts.REG_C); break; // BIT 5, C
	case 0x6A: bit_n_r(5, insts.REG_D); break; // BIT 5, D
	case 0x6B: bit_n_r(5, insts.REG_E); break; // BIT 5, E
	case 0x6C: bit_n_r(5, insts.REG_H); break; // BIT 5, H
	case 0x6D: bit_n_r(5, insts.REG_L); break; // BIT 5, L
	case 0x6E: bit_n_rr(5, cpu.HL); break; // BIT 5, (HL)
	case 0x6F: bit_n_r(5, insts.REG_A); break; // BIT 5, A



	case 0x70: bit_n_r(6, insts.REG_B); break; // BIT 6, B
	case 0x71: bit_n_r(6, insts.REG_C); break; // BIT 6, C
	case 0x72: bit_n_r(6, insts.REG_D); break; // BIT 6, D
	case 0x73: bit_n_r(6, insts.REG_E); break; // BIT 6, E
	case 0x74: bit_n_r(6, insts.REG_H); break; // BIT 6, H
	case 0x75: bit_n_r(6, insts.REG_L); break; // BIT 6, L
	case 0x76: bit_n_rr(6, cpu.HL); break; // BIT 6, (HL)
	case 0x77: bit_n_r(6, insts.REG_A); break; // BIT 6, A
	case 0x78: bit_n_r(7, insts.REG_B); break; // BIT 7, B
	case 0x79: bit_n_r(7, insts.REG_C); break; // BIT 7, C
	case 0x7A: bit_n_r(7, insts.REG_D); break; // BIT 7, D
	case 0x7B: bit_n_r(7, insts.REG_E); break; // BIT 7, E
	case 0x7C: bit_n_r(7, insts.REG_H); break; // BIT 7, H
	case 0x7D: bit_n_r(7, insts.REG_L); break; // BIT 7, L
	case 0x7E: bit_n_rr(7, cpu.HL); break; // BIT 7, (HL)
	case 0x7F: bit_n_r(7, insts.REG_A); break; // BIT 7, A



	case 0x80: res_n_r(0, insts.REG_B); break; // RES 0, B
	case 0x81: res_n_r(0, insts.REG_C); break; // RES 0, C
	case 0x82: res_n_r(0, insts.REG_D); break; // RES 0, D
	case 0x83: res_n_r(0, insts.REG_E); break; // RES 0, E
	case 0x84: res_n_r(0, insts.REG_H); break; // RES 0, H
	case 0x85: res_n_r(0, insts.REG_L); break; // RES 0, L
	case 0x86: res_n_rr(0, cpu.HL); break; // RES 0, (HL)
	case 0x87: res_n_r(0, insts.REG_A); break; // RES 0, A
	case 0x88: res_n_r(1, insts.REG_B); break; // RES 1, B
	case 0x89: res_n_r(1, insts.REG_C); break; // RES 1, C
	case 0x8A: res_n_r(1, insts.REG_D); break; // RES 1, D
	case 0x8B: res_n_r(1, insts.REG_E); break; // RES 1, E
	case 0x8C: res_n_r(1, insts.REG_H); break; // RES 1, H
	case 0x8D: res_n_r(1, insts.REG_L); break; // RES 1, L
	case 0x8E: res_n_rr(1, cpu.HL); break; // RES 1, (HL)
	case 0x8F: res_n_r(1, insts.REG_A); break; // RES 1, A



	case 0x90: res_n_r(2, insts.REG_B); break; // RES 2, B
	case 0x91: res_n_r(2, insts.REG_C); break; // RES 2, C
	case 0x92: res_n_r(2, insts.REG_D); break; // RES 2, D
	case 0x93: res_n_r(2, insts.REG_E); break; // RES 2, E
	case 0x94: res_n_r(2, insts.REG_H); break; // RES 2, H
	case 0x95: res_n_r(2, insts.REG_L); break; // RES 2, L
	case 0x96: res_n_rr(2, cpu.HL); break; // RES 2, (HL)
	case 0x97: res_n_r(2, insts.REG_A); break; // RES 2, A
	case 0x98: res_n_r(3, insts.REG_B); break; // RES 3, B
	case 0x99: res_n_r(3, insts.REG_C); break; // RES 3, C
	case 0x9A: res_n_r(3, insts.REG_D); break; // RES 3, D
	case 0x9B: res_n_r(3, insts.REG_E); break; // RES 3, E
	case 0x9C: res_n_r(3, insts.REG_H); break; // RES 3, H
	case 0x9D: res_n_r(3, insts.REG_L); break; // RES 3, L
	case 0x9E: res_n_rr(3, cpu.HL); break; // RES 3, (HL)
	case 0x9F: res_n_r(3, insts.REG_A); break; // RES 3, A



	case 0xA0: res_n_r(4, insts.REG_B); break; // RES 4, B
	case 0xA1: res_n_r(4, insts.REG_C); break; // RES 4, C
	case 0xA2: res_n_r(4, insts.REG_D); break; // RES 4, D
	case 0xA3: res_n_r(4, insts.REG_E); break; // RES 4, E
	case 0xA4: res_n_r(4, insts.REG_H); break; // RES 4, H
	case 0xA5: res_n_r(4, insts.REG_L); break; // RES 4, L
	case 0xA6: res_n_rr(4, cpu.HL); break; // RES 4, (HL)
	case 0xA7: res_n_r(4, insts.REG_A); break; // RES 4, A
	case 0xA8: res_n_r(5, insts.REG_B); break; // RES 5, B
	case 0xA9: res_n_r(5, insts.REG_C); break; // RES 5, C
	case 0xAA: res_n_r(5, insts.REG_D); break; // RES 5, D
	case 0xAB: res_n_r(5, insts.REG_E); break; // RES 5, E
	case 0xAC: res_n_r(5, insts.REG_H); break; // RES 5, H
	case 0xAD: res_n_r(5, insts.REG_L); break; // RES 5, L
	case 0xAE: res_n_rr(5, cpu.HL); break; // RES 5, (HL)
	case 0xAF: res_n_r(5, insts.REG_A); break; // RES 5, A



	case 0xB0: res_n_r(6, insts.REG_B); break; // RES 6, B
	case 0xB1: res_n_r(6, insts.REG_C); break; // RES 6, C
	case 0xB2: res_n_r(6, insts.REG_D); break; // RES 6, D
	case 0xB3: res_n_r(6, insts.REG_E); break; // RES 6, E
	case 0xB4: res_n_r(6, insts.REG_H); break; // RES 6, H
	case 0xB5: res_n_r(6, insts.REG_L); break; // RES 6, L
	case 0xB6: res_n_rr(6, cpu.HL); break; // RES 6, (HL)
	case 0xB7: res_n_r(6, insts.REG_A); break; // RES 6, A
	case 0xB8: res_n_r(7, insts.REG_B); break; // RES 7, B
	case 0xB9: res_n_r(7, insts.REG_C); break; // RES 7, C
	case 0xBA: res_n_r(7, insts.REG_D); break; // RES 7, D
	case 0xBB: res_n_r(7, insts.REG_E); break; // RES 7, E
	case 0xBC: res_n_r(7, insts.REG_H); break; // RES 7, H
	case 0xBD: res_n_r(7, insts.REG_L); break; // RES 7, L
	case 0xBE: res_n_rr(7, cpu.HL); break; // RES 7, (HL)
	case 0xBF: res_n_r(7, insts.REG_A); break; // RES 7, A



	case 0xC0: set_n_r(0, insts.REG_B); break; // SET 0, B
	case 0xC1: set_n_r(0, insts.REG_C); break; // SET 0, C
	case 0xC2: set_n_r(0, insts.REG_D); break; // SET 0, D
	case 0xC3: set_n_r(0, insts.REG_E); break; // SET 0, E
	case 0xC4: set_n_r(0, insts.REG_H); break; // SET 0, H
	case 0xC5: set_n_r(0, insts.REG_L); break; // SET 0, L
	case 0xC6: set_n_rr(0, cpu.HL); break; // SET 0, (HL)
	case 0xC7: set_n_r(0, insts.REG_A); break; // SET 0, A
	case 0xC8: set_n_r(1, insts.REG_B); break; // SET 1, B
	case 0xC9: set_n_r(1, insts.REG_C); break; // SET 1, C
	case 0xCA: set_n_r(1, insts.REG_D); break; // SET 1, D
	case 0xCB: set_n_r(1, insts.REG_E); break; // SET 1, E
	case 0xCC: set_n_r(1, insts.REG_H); break; // SET 1, H
	case 0xCD: set_n_r(1, insts.REG_L); break; // SET 1, L
	case 0xCE: set_n_rr(1, cpu.HL); break; // SET 1, (HL)
	case 0xCF: set_n_r(1, insts.REG_A); break; // SET 1, A



	case 0xD0: set_n_r(2, insts.REG_B); break; // SET 2, B
	case 0xD1: set_n_r(2, insts.REG_C); break; // SET 2, C
	case 0xD2: set_n_r(2, insts.REG_D); break; // SET 2, D
	case 0xD3: set_n_r(2, insts.REG_E); break; // SET 2, E
	case 0xD4: set_n_r(2, insts.REG_H); break; // SET 2, H
	case 0xD5: set_n_r(2, insts.REG_L); break; // SET 2, L
	case 0xD6: set_n_rr(2, cpu.HL); break; // SET 2, (HL)
	case 0xD7: set_n_r(2, insts.REG_A); break; // SET 2, A
	case 0xD8: set_n_r(3, insts.REG_B); break; // SET 3, B
	case 0xD9: set_n_r(3, insts.REG_C); break; // SET 3, C
	case 0xDA: set_n_r(3, insts.REG_D); break; // SET 3, D
	case 0xDB: set_n_r(3, insts.REG_E); break; // SET 3, E
	case 0xDC: set_n_r(3, insts.REG_H); break; // SET 3, H
	case 0xDD: set_n_r(3, insts.REG_L); break; // SET 3, L
	case 0xDE: set_n_rr(3, cpu.HL); break; // SET 3, (HL)
	case 0xDF: set_n_r(3, insts.REG_A); break; // SET 3, A



	case 0xE0: set_n_r(4, insts.REG_B); break; // SET 4, B
	case 0xE1: set_n_r(4, insts.REG_C); break; // SET 4, C
	case 0xE2: set_n_r(4, insts.REG_D); break; // SET 4, D
	case 0xE3: set_n_r(4, insts.REG_E); break; // SET 4, E
	case 0xE4: set_n_r(4, insts.REG_H); break; // SET 4, H
	case 0xE5: set_n_r(4, insts.REG_L); break; // SET 4, L
	case 0xE6: set_n_rr(4, cpu.HL); break; // SET 4, (HL)
	case 0xE7: set_n_r(4, insts.REG_A); break; // SET 4, A
	case 0xE8: set_n_r(5, insts.REG_B); break; // SET 5, B
	case 0xE9: set_n_r(5, insts.REG_C); break; // SET 5, C
	case 0xEA: set_n_r(5, insts.REG_D); break; // SET 5, D
	case 0xEB: set_n_r(5, insts.REG_E); break; // SET 5, E
	case 0xEC: set_n_r(5, insts.REG_H); break; // SET 5, H
	case 0xED: set_n_r(5, insts.REG_L); break; // SET 5, L
	case 0xEE: set_n_rr(5, cpu.HL); break; // SET 5, (HL)
	case 0xEF: set_n_r(5, insts.REG_A); break; // SET 5, A



	case 0xF0: set_n_r(6, insts.REG_B); break; // SET 6, B
	case 0xF1: set_n_r(6, insts.REG_C); break; // SET 6, C
	case 0xF2: set_n_r(6, insts.REG_D); break; // SET 6, D
	case 0xF3: set_n_r(6, insts.REG_E); break; // SET 6, E
	case 0xF4: set_n_r(6, insts.REG_H); break; // SET 6, H
	case 0xF5: set_n_r(6, insts.REG_L); break; // SET 6, L
	case 0xF6: set_n_rr(6, cpu.HL); break; // SET 6, (HL)
	case 0xF7: set_n_r(6, insts.REG_A); break; // SET 6, A
	case 0xF8: set_n_r(7, insts.REG_B); break; // SET 7, B
	case 0xF9: set_n_r(7, insts.REG_C); break; // SET 7, C
	case 0xFA: set_n_r(7, insts.REG_D); break; // SET 7, D
	case 0xFB: set_n_r(7, insts.REG_E); break; // SET 7, E
	case 0xFC: set_n_r(7, insts.REG_H); break; // SET 7, H
	case 0xFD: set_n_r(7, insts.REG_L); break; // SET 7, L
	case 0xFE: set_n_rr(7, cpu.HL); break; // SET 7, (HL)
	case 0xFF: set_n_r(7, insts.REG_A); break; // SET 7, A



	default:

		printf("[ERROR] Unknown EXTENDED opcode: 0x%02X at PC: 0x%04X | ", opcode, cpu.PC);
		//printf("DIV: %d\n", mmu->timer.div);
		printf("Cycles: %d\n", cpu.t_cycles);
		return;
		break;
	}
	printf("[INFO] EXTENDED Opcode 0x%02X at PC: 0x%04X executed\n", opcode, cpu.PC);
}



void ExtInstruction::rlc_r(Instruction::Register8 reg) {
	uint8_t b = insts.getReg(reg);
	uint8_t bit7 = (b >> 7) & 0x01;
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
void ExtInstruction::bit_n_r(uint8_t bit, Instruction::Register8 reg) {
	uint8_t val = insts.getReg(reg);
	bool bitIsZero = (val & (1 << bit)) == 0;

	insts.setFlag(cpu.FLAG_Z, bitIsZero);
	insts.setFlag(cpu.FLAG_N, false);
	insts.setFlag(cpu.FLAG_H, true);
	insts.setFlag(cpu.FLAG_C, insts.getFlag(cpu.FLAG_C));
}
void ExtInstruction::bit_n_rr(uint8_t bit, CPU::RegisterPair reg) {
	// Read the byte at [HL]
	uint16_t addr = reg.full;
	uint8_t value = bus.bus_read(addr);

	// Test bit 0 (LSB)
	bool bit0 = (value & (1 << bit)) == 0;

	// Flags: Z=1 if bit0 is 0, N=0, H=1, C unchanged
	insts.setFlag(cpu.FLAG_Z, bit0);            // Z=1 if bit 0 is 0
	insts.setFlag(cpu.FLAG_N, false);          // N always 0
	insts.setFlag(cpu.FLAG_H, true);           // H always 1
	insts.setFlag(cpu.FLAG_C, insts.getFlag(cpu.FLAG_C)); // C unchanged
}
void ExtInstruction::res_n_r(uint8_t bit, Instruction::Register8 reg) {
	uint8_t val = insts.getReg(reg);
	val &= ~(1 << bit);  // Clear the specified bit
	insts.getReg(reg) = val;
}
void ExtInstruction::res_n_rr(uint8_t bit, CPU::RegisterPair reg) {
	uint16_t addr = reg.full;
	uint8_t value = bus.bus_read(addr);
	value &= ~(1 << bit);  // Clear the specified bit
	bus.bus_write(addr, value);
}
void ExtInstruction::set_n_r(uint8_t bit, Instruction::Register8 reg) {
	uint8_t val = insts.getReg(reg);
	val |= (1 << bit);  // Set the specified bit
	insts.getReg(reg) = val;
}
void ExtInstruction::set_n_rr(uint8_t bit, CPU::RegisterPair reg) {
	uint16_t address = reg.full;
	uint8_t value = bus.bus_read(address);
	value |= (1 << bit);  // Set specified bit
	bus.bus_write(address, value);
}