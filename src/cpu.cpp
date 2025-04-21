#include <cstring>
#include <fstream>
#include <iostream>
#include <cpu.h>
#include <tools.h>
#include <cartridge.h>
#include <bus.h>
#include <iomanip>
#include <instructions.h>
#include <io.h>

#include <bitset>
CPU::CPU()
{
	//if its not bootrom
	PC = 0x100; //is it 0x100 or 0x0ff?
	AF.full = 0x0130;
	SP = 0xFFFE;
}

//helper stuff here..
bool haltedshow = false;

void CPU::halt() {
	if (!IME && (bus.IE & bus.IF) != 0) {
		// HALT bug triggers
		haltBug = true;
		haltedshow = false;
	}
	else {
		halted = true;
		haltedshow = true;
	}
	
}
















































//main cpu stuff here now yay

void CPU::Push16(uint16_t value) {
	SP--;
	bus.bus_write(SP, (value >> 8) & 0xFF);
	SP--;
	bus.bus_write(SP, value & 0xFF);
}
uint16_t CPU::Read16(uint16_t addr) {
	uint8_t lo = bus.bus_read(addr);
	uint8_t hi = bus.bus_read(addr + 1);
	return lo | (hi << 8);
}
bool CPU::InterruptCheck(uint16_t address, interrupt_type it) {
	if (bus.IF & it && bus.IE & it) {
		Push16(address);
		PC = address;
		bus.IF &= ~it;
		halted = false;
	    IME = false;
		temp_t_cycles += 20;

		return true;
	}

	return false;
}
void CPU::HandleInterrupt() {

	if (InterruptCheck(0x40, IT_VBLANK)) {

    } else if (InterruptCheck(0x48, IT_LCD_STAT)) {

    } else if (InterruptCheck(0x50, IT_TIMER)) {

    }  else if (InterruptCheck(0x58, IT_SERIAL)) {

    }  else if (InterruptCheck(0x60, IT_JOYPAD)) {

	}
	else
		return;
	printf("interrupt woked cpu up\n");
}
void CPU::RequestInterrupt(interrupt_type t) {

}

int imeAfterNextInsts = 0;
uint16_t tempPC = 0x100;
uint8_t tempA, tempF, tempIF, tempIE;
uint16_t tempBC, tempDE, tempHL, tempSP;

void CPU::ExecuteInstruction(uint8_t opcode) {
	uint8_t value, result, offset;
	uint16_t addr;
	if (imeAfterNextInsts == 1) {
		imeAfterNextInsts++;
	}
	else if (imeAfterNextInsts == 2) {
		IME = false;
		imeAfterNextInsts = 0;
	}
	if (imeAfterNextInsts == 4) {
		imeAfterNextInsts++;
	}
	else if (imeAfterNextInsts == 5) {
		IME = true;
		imeAfterNextInsts = 0;
	}
	size_t size = sizeof(cart.rom_data);
	//std::cout << "wram Used: " << size << "\n";
	if (size > 0x10000) {
		//std::cerr << "ERROR: wram Overflow!!" << "\n";
	}
	//printf("Executing instruction: %02X  PC: %04X\n", opcode, PC);

	temp_t_cycles = 0;
	temp_t_cycles += insts.instructionTicks[opcode];

	switch (opcode) {
	case 0x00: break; // NOP
	case 0x01: insts.ld_rr_d16(BC); break; // LD BC, nn
	case 0x02: insts.ld_addr_rr_a(BC, 0); break; // LD (BC), A
	case 0x03: BC.full++; break;
	case 0x04: insts.inc_r(insts.REG_B); break; // INC B
	case 0x05: insts.dec_r(insts.REG_B); break;// DEC B
	case 0x06: insts.ld_r_n(insts.REG_B, bus.bus_read(PC++)); break; // LD B, n
	case 0x07: insts.rlc_r(insts.REG_A); break; // RLCA
	case 0x08: insts.ld_a16_sp(); break; // LD (nn), SP
	case 0x09: insts.add_hl_rr(BC.full); break; // ADD HL, BC
	case 0x0A: insts.ld_a_addr_rr(BC, 0); break; // LD A, (BC)
	case 0x0B: BC.full--; break; // DEC BC
	case 0x0C: insts.inc_r(insts.REG_C); break; // INC C
	case 0x0D: insts.dec_r(insts.REG_C); break; // DEC C
	case 0x0E: insts.ld_r_n(insts.REG_C, bus.bus_read(PC++)); break; // LD C, n
	case 0x0F: insts.rrc(insts.REG_A); break; // RRCA



	case 0x10: break; //STOP
	case 0x11: insts.ld_rr_d16(DE); break; // LD DE, nn
	case 0x12: insts.ld_addr_rr_a(DE, 0); break; // LD (DE), A
	case 0x13: DE.full++; break; // INC DE
	case 0x14: insts.inc_r(insts.REG_D); break; // INC D
	case 0x15: insts.dec_r(insts.REG_D); break;// DEC D
	case 0x16: insts.ld_r_n(insts.REG_D, bus.bus_read(PC++)); break; // LD D, n
	case 0x17: insts.rla(); break; // RLA
	case 0x18: insts.jr_n(); break; // JR nn
	case 0x19: insts.add_hl_rr(DE.full); break; // ADD HL, DE
	case 0x1A: insts.ld_a_addr_rr(DE, 0); break; // LD A, (DE)
	case 0x1B: DE.full--; break; // DEC DE
	case 0x1C: insts.inc_r(insts.REG_E); break; // INC E
	case 0x1D: insts.dec_r(insts.REG_E); break; // DEC E
	case 0x1E: insts.ld_r_n(insts.REG_E, bus.bus_read(PC++)); break; // LD E, n
	case 0x1F: insts.rr_r(insts.REG_A); break; // RRA



	case 0x20: insts.jr_f(static_cast<int8_t>(bus.bus_read(PC + 1)), FLAG_Z, true); break; // JR NZ, *
	case 0x21: insts.ld_rr_d16(HL); break; // LD HL, nn
	case 0x22: insts.ld_addr_rr_a(HL, 1); break; // LD (HL+), A
	case 0x23: HL.full++; break; // INC HL
	case 0x24: insts.inc_r(insts.REG_H); break; // INC H
	case 0x25: insts.dec_r(insts.REG_H); break;// DEC H
	case 0x26: insts.ld_r_n(insts.REG_H, bus.bus_read(PC++)); break; // LD H, n
	case 0x27: insts.daa(); break; // DAA
	case 0x28: insts.jr_f(static_cast<int8_t>(bus.bus_read(PC + 1)), FLAG_Z, false); break; // JR Z, *
	case 0x29: insts.add_hl_rr(HL.full); break; // ADD HL, HL
	case 0x2A: insts.ld_a_addr_rr(HL, 1); break; // LD A, (HL+)
	case 0x2B: HL.full--; break; // DEC HL
	case 0x2C: insts.inc_r(insts.REG_L); break; // INC L
	case 0x2D: insts.dec_r(insts.REG_L); break; // DEC L
	case 0x2E: insts.ld_r_n(insts.REG_L, bus.bus_read(PC++)); break; // LD L, n
	case 0x2F: insts.cpl(); break; // CPL



	case 0x30: insts.jr_f(static_cast<int8_t>(bus.bus_read(PC + 1)), FLAG_C, true); break; // JR NC, *
	case 0x31: SP = Read16(PC); PC += 2; break; // LD SP, nn
	case 0x32: insts.ld_addr_rr_a(HL, -1); break; // LD (HL-), A
	case 0x33: SP++; break; // INC SP
	case 0x34: value = bus.bus_read(HL.full); result = value + 1; insts.setFlag(cpu.FLAG_Z, result == 0); insts.setFlag(cpu.FLAG_N, false); insts.setFlag(cpu.FLAG_H, (value & 0x0F) + 1 > 0x0F); bus.bus_write(HL.full, result); break; // INC (HL)
	case 0x35: value = bus.bus_read(HL.full); result = value - 1; insts.setFlag(cpu.FLAG_Z, result == 0); insts.setFlag(cpu.FLAG_N, true); insts.setFlag(cpu.FLAG_H, (value & 0x0F) == 0x00); bus.bus_write(HL.full, result); break;// DEC (HL)
	case 0x36: value = bus.bus_read(PC + 1); bus.bus_write(HL.full, value); PC += 2; break; // LD (HL), n
	case 0x37: insts.setFlag(FLAG_C, true); insts.setFlag(FLAG_N, false); insts.setFlag(FLAG_H, false); break; // SCF
	case 0x38: insts.jr_f(static_cast<int8_t>(bus.bus_read(PC + 1)), FLAG_C, false); break; // JR C, *
	case 0x39: insts.add_hl_rr(SP); break; // ADD HL, SP
	case 0x3A: insts.ld_a_addr_rr(HL, -1); break; // LD A, (HL-)
	case 0x3B: SP--; break; // DEC SP
	case 0x3C: insts.inc_r(insts.REG_A); break; // INC A
	case 0x3D: insts.dec_r(insts.REG_A); break; // DEC A
	case 0x3E: insts.ld_r_n(insts.REG_A, bus.bus_read(PC++)); break; // LD A, n
	case 0x3F: insts.setFlag(FLAG_N, false); insts.setFlag(FLAG_H, false); insts.setFlag(FLAG_C, !insts.getFlag(FLAG_C)); break; // CCF



	case 0x40: insts.ld_r_r(insts.REG_B, insts.REG_B); break;
	case 0x41: insts.ld_r_r(insts.REG_B, insts.REG_C); break;
	case 0x42: insts.ld_r_r(insts.REG_B, insts.REG_D); break;
	case 0x43: insts.ld_r_r(insts.REG_B, insts.REG_E); break;
	case 0x44: insts.ld_r_r(insts.REG_B, insts.REG_H); break;
	case 0x45: insts.ld_r_r(insts.REG_B, insts.REG_L); break;
	case 0x46: insts.ld_r_rr(insts.REG_B, HL); break;
	case 0x47: insts.ld_r_r(insts.REG_B, insts.REG_A); break;
	case 0x48: insts.ld_r_r(insts.REG_C, insts.REG_B); break;
	case 0x49: insts.ld_r_r(insts.REG_C, insts.REG_C); break;
	case 0x4A: insts.ld_r_r(insts.REG_C, insts.REG_D); break;
	case 0x4B: insts.ld_r_r(insts.REG_C, insts.REG_E); break;
	case 0x4C: insts.ld_r_r(insts.REG_C, insts.REG_H); break;
	case 0x4D: insts.ld_r_r(insts.REG_C, insts.REG_L); break;
	case 0x4E: insts.ld_r_rr(insts.REG_C, HL); break;
	case 0x4F: insts.ld_r_r(insts.REG_C, insts.REG_A); break;



	case 0x50: insts.ld_r_r(insts.REG_D, insts.REG_B); break;
	case 0x51: insts.ld_r_r(insts.REG_D, insts.REG_C); break;
	case 0x52: insts.ld_r_r(insts.REG_D, insts.REG_D); break;
	case 0x53: insts.ld_r_r(insts.REG_D, insts.REG_E); break;
	case 0x54: insts.ld_r_r(insts.REG_D, insts.REG_H); break;
	case 0x55: insts.ld_r_r(insts.REG_D, insts.REG_L); break;
	case 0x56: insts.ld_r_rr(insts.REG_D, HL); break;
	case 0x57: insts.ld_r_r(insts.REG_D, insts.REG_A); break;
	case 0x58: insts.ld_r_r(insts.REG_E, insts.REG_B); break;
	case 0x59: insts.ld_r_r(insts.REG_E, insts.REG_C); break;
	case 0x5A: insts.ld_r_r(insts.REG_E, insts.REG_D); break;
	case 0x5B: insts.ld_r_r(insts.REG_E, insts.REG_E); break;
	case 0x5C: insts.ld_r_r(insts.REG_E, insts.REG_H); break;
	case 0x5D: insts.ld_r_r(insts.REG_E, insts.REG_L); break;
	case 0x5E: insts.ld_r_rr(insts.REG_E, HL); break;
	case 0x5F: insts.ld_r_r(insts.REG_E, insts.REG_A); break;



	case 0x60: insts.ld_r_r(insts.REG_H, insts.REG_B); break;
	case 0x61: insts.ld_r_r(insts.REG_H, insts.REG_C); break;
	case 0x62: insts.ld_r_r(insts.REG_H, insts.REG_D); break;
	case 0x63: insts.ld_r_r(insts.REG_H, insts.REG_E); break;
	case 0x64: insts.ld_r_r(insts.REG_H, insts.REG_H); break;
	case 0x65: insts.ld_r_r(insts.REG_H, insts.REG_L); break;
	case 0x66: insts.ld_r_rr(insts.REG_H, HL); break;
	case 0x67: insts.ld_r_r(insts.REG_H, insts.REG_A); break;
	case 0x68: insts.ld_r_r(insts.REG_L, insts.REG_B); break;
	case 0x69: insts.ld_r_r(insts.REG_L, insts.REG_C); break;
	case 0x6A: insts.ld_r_r(insts.REG_L, insts.REG_D); break;
	case 0x6B: insts.ld_r_r(insts.REG_L, insts.REG_E); break;
	case 0x6C: insts.ld_r_r(insts.REG_L, insts.REG_H); break;
	case 0x6D: insts.ld_r_r(insts.REG_L, insts.REG_L); break;
	case 0x6E: insts.ld_r_rr(insts.REG_L, HL); break;
	case 0x6F: insts.ld_r_r(insts.REG_L, insts.REG_A); break;



	case 0x70: insts.ld_rr_r(HL, insts.REG_B); break;
	case 0x71: insts.ld_rr_r(HL, insts.REG_C); break;
	case 0x72: insts.ld_rr_r(HL, insts.REG_D); break;
	case 0x73: insts.ld_rr_r(HL, insts.REG_E); break;
	case 0x74: insts.ld_rr_r(HL, insts.REG_H); break;
	case 0x75: insts.ld_rr_r(HL, insts.REG_L); break;
	case 0x76: halt(); break;
	case 0x77: insts.ld_rr_r(HL, insts.REG_A); break;
	case 0x78: insts.ld_r_r(insts.REG_A, insts.REG_B); break;
	case 0x79: insts.ld_r_r(insts.REG_A, insts.REG_C); break;
	case 0x7A: insts.ld_r_r(insts.REG_A, insts.REG_D); break;
	case 0x7B: insts.ld_r_r(insts.REG_A, insts.REG_E); break;
	case 0x7C: insts.ld_r_r(insts.REG_A, insts.REG_H); break;
	case 0x7D: insts.ld_r_r(insts.REG_A, insts.REG_L); break;
	case 0x7E: insts.ld_r_rr(insts.REG_A, HL); break;
	case 0x7F: insts.ld_r_r(insts.REG_A, insts.REG_A); break;



	case 0x80: insts.add_r_r(insts.REG_A, insts.getReg(insts.REG_B)); break;
	case 0x81: insts.add_r_r(insts.REG_A, insts.getReg(insts.REG_C)); break;
	case 0x82: insts.add_r_r(insts.REG_A, insts.getReg(insts.REG_D)); break;
	case 0x83: insts.add_r_r(insts.REG_A, insts.getReg(insts.REG_E)); break;
	case 0x84: insts.add_r_r(insts.REG_A, insts.getReg(insts.REG_H)); break;
	case 0x85: insts.add_r_r(insts.REG_A, insts.getReg(insts.REG_L)); break;
	case 0x86: insts.add_r_rr(insts.REG_A, HL); break;
	case 0x87: insts.add_r_r(insts.REG_A, insts.getReg(insts.REG_A)); break;
	case 0x88: insts.adc_r_r(insts.REG_A, insts.getReg(insts.REG_B)); break;
	case 0x89: insts.adc_r_r(insts.REG_A, insts.getReg(insts.REG_C)); break;
	case 0x8A: insts.adc_r_r(insts.REG_A, insts.getReg(insts.REG_D)); break;
	case 0x8B: insts.adc_r_r(insts.REG_A, insts.getReg(insts.REG_E)); break;
	case 0x8C: insts.adc_r_r(insts.REG_A, insts.getReg(insts.REG_H)); break;
	case 0x8D: insts.adc_r_r(insts.REG_A, insts.getReg(insts.REG_L)); break;
	case 0x8E: insts.adc_r_rr(insts.REG_A, HL); break;
	case 0x8F: insts.adc_r_r(insts.REG_A, insts.getReg(insts.REG_A)); break;



	case 0x90: insts.sub_r_r(insts.REG_A, insts.getReg(insts.REG_B)); break;
	case 0x91: insts.sub_r_r(insts.REG_A, insts.getReg(insts.REG_C)); break;
	case 0x92: insts.sub_r_r(insts.REG_A, insts.getReg(insts.REG_D)); break;
	case 0x93: insts.sub_r_r(insts.REG_A, insts.getReg(insts.REG_E)); break;
	case 0x94: insts.sub_r_r(insts.REG_A, insts.getReg(insts.REG_H)); break;
	case 0x95: insts.sub_r_r(insts.REG_A, insts.getReg(insts.REG_L)); break;
	case 0x96: insts.sub_rr(HL); break;
	case 0x97: insts.sub_r_r(insts.REG_A, insts.getReg(insts.REG_A)); break;
	case 0x98: insts.sbc_r_r(insts.REG_A, insts.getReg(insts.REG_B)); break;
	case 0x99: insts.sbc_r_r(insts.REG_A, insts.getReg(insts.REG_C)); break;
	case 0x9A: insts.sbc_r_r(insts.REG_A, insts.getReg(insts.REG_D)); break;
	case 0x9B: insts.sbc_r_r(insts.REG_A, insts.getReg(insts.REG_E)); break;
	case 0x9C: insts.sbc_r_r(insts.REG_A, insts.getReg(insts.REG_H)); break;
	case 0x9D: insts.sbc_r_r(insts.REG_A, insts.getReg(insts.REG_L)); break;
	case 0x9E: insts.sbc_r_rr(insts.REG_A, HL); break;
	case 0x9F: insts.sbc_r_r(insts.REG_A, insts.getReg(insts.REG_A)); break;



	case 0xA0: insts.and_r_r(insts.REG_A, insts.getReg(insts.REG_B)); break;
	case 0xA1: insts.and_r_r(insts.REG_A, insts.getReg(insts.REG_C)); break;
	case 0xA2: insts.and_r_r(insts.REG_A, insts.getReg(insts.REG_D)); break;
	case 0xA3: insts.and_r_r(insts.REG_A, insts.getReg(insts.REG_E)); break;
	case 0xA4: insts.and_r_r(insts.REG_A, insts.getReg(insts.REG_H)); break;
	case 0xA5: insts.and_r_r(insts.REG_A, insts.getReg(insts.REG_L)); break;
	case 0xA6: insts.and_r_rr(insts.REG_A, HL); break;
	case 0xA7: insts.and_r_r(insts.REG_A, insts.getReg(insts.REG_A)); break;
	case 0xA8: insts.xor_r_r(insts.REG_A, insts.getReg(insts.REG_B)); break;
	case 0xA9: insts.xor_r_r(insts.REG_A, insts.getReg(insts.REG_C)); break;
	case 0xAA: insts.xor_r_r(insts.REG_A, insts.getReg(insts.REG_D)); break;
	case 0xAB: insts.xor_r_r(insts.REG_A, insts.getReg(insts.REG_E)); break;
	case 0xAC: insts.xor_r_r(insts.REG_A, insts.getReg(insts.REG_H)); break;
	case 0xAD: insts.xor_r_r(insts.REG_A, insts.getReg(insts.REG_L)); break;
	case 0xAE: insts.xor_r_rr(insts.REG_A, HL); break;
	case 0xAF: insts.xor_r_r(insts.REG_A, insts.getReg(insts.REG_A)); break;



	case 0xB0: insts.or_r_r(insts.REG_A, insts.getReg(insts.REG_B)); break;
	case 0xB1: insts.or_r_r(insts.REG_A, insts.getReg(insts.REG_C)); break;
	case 0xB2: insts.or_r_r(insts.REG_A, insts.getReg(insts.REG_D)); break;
	case 0xB3: insts.or_r_r(insts.REG_A, insts.getReg(insts.REG_E)); break;
	case 0xB4: insts.or_r_r(insts.REG_A, insts.getReg(insts.REG_H)); break;
	case 0xB5: insts.or_r_r(insts.REG_A, insts.getReg(insts.REG_L)); break;
	case 0xB6: insts.or_r_rr(insts.REG_A, HL); break;
	case 0xB7: insts.or_r_r(insts.REG_A, insts.getReg(insts.REG_A)); break;
	case 0xB8: insts.cp_r_r(insts.REG_A, insts.getReg(insts.REG_B)); break;
	case 0xB9: insts.cp_r_r(insts.REG_A, insts.getReg(insts.REG_C)); break;
	case 0xBA: insts.cp_r_r(insts.REG_A, insts.getReg(insts.REG_D)); break;
	case 0xBB: insts.cp_r_r(insts.REG_A, insts.getReg(insts.REG_E)); break;
	case 0xBC: insts.cp_r_r(insts.REG_A, insts.getReg(insts.REG_H)); break;
	case 0xBD: insts.cp_r_r(insts.REG_A, insts.getReg(insts.REG_L)); break;
	case 0xBE: insts.cp_r_rr(insts.REG_A, HL); break;
	case 0xBF: insts.cp_r_r(insts.REG_A, insts.getReg(insts.REG_A)); break;



	case 0xC0: insts.ret(FLAG_Z, true); break; // RET NZ
	case 0xC1: insts.pop_rr(BC); break; // POP BC
	case 0xC2: insts.jp_f(static_cast<int8_t>(bus.bus_read(PC + 1)), FLAG_Z, true); break; // JP NZ, a16
	case 0xC3: insts.jp_a16(); break; //JP a16
	case 0xC4: insts.call_f(static_cast<int8_t>(bus.bus_read(PC + 1)), FLAG_Z, true); break; //CALL NZ, a16
	case 0xC5: insts.push_rr(BC); break; // PUSH BC
	case 0xC6: insts.add_r_r(insts.REG_A, static_cast<int8_t>(bus.bus_read(PC++))); break; // ADD A, a8
	case 0xC7: insts.rst_addr(0x00); break;
	case 0xC8: insts.ret(FLAG_Z, false); break; // RET Z
	case 0xC9: insts.o_ret(); break; // RET
	case 0xCA: insts.jp_f(static_cast<int8_t>(bus.bus_read(PC + 1)), FLAG_Z, false); break; // JP Z, a16
	case 0xCB: insts.cb_prefix(); break; // CB PREFIX
	case 0xCC: insts.call_f(static_cast<int8_t>(bus.bus_read(PC + 1)), FLAG_Z, false); break; //CALL Z, a16
	case 0xCD: insts.call_a16(); break; // CALL a16
	case 0xCE: insts.adc_r_r(insts.REG_A, static_cast<int8_t>(bus.bus_read(PC++))); break;
	case 0xCF: insts.rst_addr(0x08); break;



	case 0xD0: insts.ret(FLAG_C, true); break; // RET NC
	case 0xD1: insts.pop_rr(DE); break; // POP DE
	case 0xD2: insts.jp_f(static_cast<int8_t>(bus.bus_read(PC + 1)), FLAG_C, true); break; // JP NC, a16
	case 0xD3: insts.jp_a16(); break; //JP a16
	case 0xD4: insts.call_f(static_cast<int8_t>(bus.bus_read(PC + 1)), FLAG_C, true); break; //CALL NC, a16
	case 0xD5: insts.push_rr(DE); break; // PUSH DE
	case 0xD6: insts.sub_r_r(insts.REG_A, static_cast<int8_t>(bus.bus_read(PC++))); break; // ADD A, a8
	case 0xD7: insts.rst_addr(0x10); break;
	case 0xD8: insts.ret(FLAG_C, false); break; // RET C
	case 0xD9: insts.o_ret(); cpu.IME = true; break; // RETI
	case 0xDA: insts.jp_f(static_cast<int8_t>(bus.bus_read(PC + 1)), FLAG_C, false); break; // JP C, a16
	case 0xDB: /*nothing*/ break; // nothing
	case 0xDC: insts.call_f(static_cast<int8_t>(bus.bus_read(PC + 1)), FLAG_C, false); break; //CALL C, a16
	case 0xDD: /*nothing*/ break; // nothing
	case 0xDE: insts.sbc_r_r(insts.REG_A, static_cast<int8_t>(bus.bus_read(PC++))); break;
	case 0xDF: insts.rst_addr(0x18); break;



	case 0xE0: addr = 0xFF00 + bus.bus_read(cpu.PC++); bus.bus_write(addr, cpu.AF.hi); break; // LDH (a8),A
	case 0xE1: insts.pop_rr(HL); break; // POP HL
	case 0xE2: addr = 0xFF00 + cpu.BC.lo; bus.bus_write(addr, cpu.AF.hi); break; // LD (C), A
	case 0xE3: /*nothing*/ break;
	case 0xE4: /*nothing*/ break;
	case 0xE5: insts.push_rr(HL); break; // PUSH HL
	case 0xE6: insts.and_r_r(insts.REG_A, static_cast<int8_t>(bus.bus_read(PC++))); break;
	case 0xE7: insts.rst_addr(0x20); break;// RST h20
	case 0xE8: insts.add_sp_r8(static_cast<int8_t>(bus.bus_read(PC++))); break; // ADD SP, a8
	case 0xE9: PC = HL.full; break; // JP a16
	case 0xEA: insts.ld_a16_a(); break; // LD a16, A
	case 0xEB: /*nothing*/ break; // nothing
	case 0xEC: /*nothing*/ break; // nothing
	case 0xED: /*nothing*/ break; // nothing
	case 0xEE: insts.xor_r_r(insts.REG_A, static_cast<int8_t>(bus.bus_read(PC++))); break;
	case 0xEF: insts.rst_addr(0x28); break;


	case 0xF0: offset = bus.bus_read(PC++); addr = 0xFF00 | offset; AF.hi = bus.bus_read(addr); break;
	case 0xF1: insts.pop_rr(AF); AF.lo &= 0xf0; break; // POP AF
	case 0xF2: addr = 0xFF00 + insts.getReg(insts.REG_C); AF.hi = bus.bus_read(addr); break; // LD A,(C)
	case 0xF3: imeAfterNextInsts = 1; /* disable IME after next instruction*/ break; // DI
	case 0xF4: /*nothing*/ break; // nothing
	case 0xF5: insts.push_rr(AF); break; // PUSH AF
	case 0xF6: insts.or_r_r(insts.REG_A, static_cast<int8_t>(bus.bus_read(PC++))); break;
	case 0xF7: insts.rst_addr(0x30); break; // RST h30
	case 0xF8: insts.ld_hl_sp_r8(); break;
	case 0xF9: SP = HL.full; break; // LD SP,HL
	case 0xFA: insts.ld_a_a16(); break; // LD A, a16
	case 0xFB: imeAfterNextInsts = 4; /* enable IME after next instruction*/ break; // EI
	case 0xFC: /*nothing*/ break; // nothing
	case 0xFD: /*nothing*/ break; // nothing
	case 0xFE: insts.cp_r_r(insts.REG_A, static_cast<int8_t>(bus.bus_read(PC++))); break;
	case 0xFF: insts.rst_addr(0x38); break;



	default:

		printf("[ERROR] Unknown opcode: 0x%02x at 0x%04x | ", opcode, tempPC);
		//printf("DIV: %d\n", mmu->timer.div);
		printf("Cycles: %d\n", t_cycles);
		return;
		break;
	}
	char flags[5] = { 0 }; // ZNHC + null terminator
	flags[0] = (tempF & (1 << 7)) ? 'Z' : '-';  // Bit 7
	flags[1] = (tempF & (1 << 6)) ? 'N' : '-';  // Bit 6
	flags[2] = (tempF & (1 << 5)) ? 'H' : '-';  // Bit 5
	flags[3] = (tempF & (1 << 4)) ? 'C' : '-';  // Bit 4
	flags[4] = '\0';
	
	printf("[INFO] PC: %04X | Executed 0x%02X (%02X %02X) | A: %02X F: %02X (b%s) %s BC: %04X DE: %04X HL: %04X SP: %04X IF: %02X IE: %02X\n",tempPC, opcode, bus.bus_read(tempPC + 1), bus.bus_read(tempPC + 2), tempA, tempF, std::bitset<8>(tempF).to_string().c_str(), flags, tempBC, tempDE, tempHL, tempSP, tempIF, tempIE );

}

void CPU::Cycle() {
	if (!halted) {

		uint8_t opcode = bus.bus_read(PC);
		currOpcode = opcode;
		tempPC = PC;
		tempA = AF.hi;
		tempF = AF.lo;
		tempBC = BC.full;
		tempDE = DE.full;
		tempHL = HL.full;
		tempIF = bus.IF;
		tempIE = bus.IE;
		tempSP = SP;

		PC++;





		/*printf("%04X: %-7s (%02X %02X %02X) A: %02X B: %02X C: %02X\n",
			PC, insts.inst_name(cur_inst->type), cur_opcode,
			bus.bus_read(PC + 1), bus.bus_read(PC + 2), getReg(REG_A), getReg(REG_B), getReg(REG_B));*/



		temp_t_cycles = 0; // Reset before instruction

		ExecuteInstruction(opcode);

		t_cycles += temp_t_cycles;
		m_cycles += temp_t_cycles / 4;
	 
		 
	}
	else {
		
		t_cycles += 4;
		if (haltedshow) {
			std::cout << "Halted!\n";
			haltedshow = false;
		}
		if ((IME && (bus.IE & bus.IF & 0x1F)) || (!IME && haltBug)) {
			halted = false;
			printf("CPU woken by %s\n",
				IME ? "interrupt" : "HALT bug");
		}
	}
	if (IME && (bus.IE & bus.IF & 0x1F)) {
		HandleInterrupt();
		imeAfterNextInsts = 0;
	}

	
	if (imeAfterNextInsts == 5) {
		IME = true;
		imeAfterNextInsts = 0;
	}
	ticks += 1;
	io.dbg_update();
	io.dbg_print();
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