#include <cstring>
#include <fstream>
#include <iostream>
#include <cpu.h>
#include <tools.h>
#include <cartridge.h>
#include <bus.h>
#include <iomanip>
#include <instructions.h>



CPU::CPU()
{
	//if its not bootrom
	PC = 0x100; //is it 0x100 or 0x0ff?
	AF.hi = 0x01;
}
//helper stuff here..
void CPU::halt() {
	/*if (!IME && (IE & IF) != 0) {
		// HALT bug triggers
		haltBug = true;
	}
	else {
		halted = true;
	}*/
	temp_t_cycles += 4;
	std::cout << "HALTEDDDDDDDD!!!!\n";
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
void CPU::HandleInterrupt() {
	/*uint8_t triggered = IE & IF;
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
	} coming soon*/
}


void CPU::ExecuteInstruction(uint8_t opcode) {
	uint8_t value, result;
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
	case 0x31: SP = Read16(PC + 1); PC += 3; break; // LD SP, nn
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
	case 0x3C: insts.inc_r(insts.REG_A); break; // INC L
	case 0x3D: insts.dec_r(insts.REG_A); break; // DEC L
	case 0x3E: insts.ld_r_n(insts.REG_A, bus.bus_read(PC++)); break; // LD L, n
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



	case 0x80: insts.add_r_r(insts.REG_A, insts.REG_B); break;
	case 0x81: insts.add_r_r(insts.REG_A, insts.REG_C); break;
	case 0x82: insts.add_r_r(insts.REG_A, insts.REG_D); break;
	case 0x83: insts.add_r_r(insts.REG_A, insts.REG_E); break;
	case 0x84: insts.add_r_r(insts.REG_A, insts.REG_H); break;
	case 0x85: insts.add_r_r(insts.REG_A, insts.REG_L); break;
	case 0x86: insts.add_r_rr(insts.REG_A, HL); break;
	case 0x87: insts.add_r_r(insts.REG_A, insts.REG_A); break;
	case 0x88: insts.adc_r_r(insts.REG_A, insts.REG_B); break;
	case 0x89: insts.adc_r_r(insts.REG_A, insts.REG_C); break;
	case 0x8A: insts.adc_r_r(insts.REG_A, insts.REG_D); break;
	case 0x8B: insts.adc_r_r(insts.REG_A, insts.REG_E); break;
	case 0x8C: insts.adc_r_r(insts.REG_A, insts.REG_H); break;
	case 0x8D: insts.adc_r_r(insts.REG_A, insts.REG_L); break;
	case 0x8E: insts.adc_r_rr(insts.REG_A, HL); break;
	case 0x8F: insts.adc_r_r(insts.REG_A, insts.REG_A); break;



	case 0x90: insts.sub_r(insts.REG_B); break;
	case 0x91: insts.sub_r(insts.REG_C); break;
	case 0x92: insts.sub_r(insts.REG_D); break;
	case 0x93: insts.sub_r(insts.REG_E); break;
	case 0x94: insts.sub_r(insts.REG_H); break;
	case 0x95: insts.sub_r(insts.REG_L); break;
	case 0x96: insts.sub_rr(HL); break;
	case 0x97: insts.sub_r(insts.REG_A); break;
	case 0x98: insts.sbc_r_r(insts.REG_A, insts.REG_B); break;
	case 0x99: insts.sbc_r_r(insts.REG_A, insts.REG_C); break;
	case 0x9A: insts.sbc_r_r(insts.REG_A, insts.REG_D); break;
	case 0x9B: insts.sbc_r_r(insts.REG_A, insts.REG_E); break;
	case 0x9C: insts.sbc_r_r(insts.REG_A, insts.REG_H); break;
	case 0x9D: insts.sbc_r_r(insts.REG_A, insts.REG_L); break;
	case 0x9E: insts.sbc_r_rr(insts.REG_A, HL); break;
	case 0x9F: insts.sbc_r_r(insts.REG_A, insts.REG_A); break;




		/*
		
		
		case 0xA0:  // AND B
			and_(registers->b);
			break;
		case 0xA1:  // AND C
			and_(registers->c);
			break;
		case 0xA2:  // AND D
			and_(registers->d);
			break;
		case 0xA3:  // AND E
			and_(registers->e);
			break;
		case 0xA4:  // AND H
			and_(registers->h);
			break;
		case 0xA5:  // AND l
			and_(registers->l);
			break;
		case 0xA6:  // AND (HL)
			and_(mmu->read_byte(registers->hl));
			break;
		case 0xA7:  // AND A
			and_(registers->a);
			break;
		case 0xA8:  // XOR B
			xor_(registers->b);
			break;
		case 0xA9:  // XOR C
			xor_(registers->c);
			break;
		case 0xAA:  // XOR D
			xor_(registers->d);
			break;
		case 0xAB:  // XOR E
			xor_(registers->e);
			break;
		case 0xAC:  // XOR H
			xor_(registers->h);
			break;
		case 0xAD:  // XOR L
			xor_(registers->l);
			break;
		case 0xAE:  // XOR (HL)
			xor_(mmu->read_byte(registers->hl));
			break;
		case 0xAF:  // XOR A
			xor_(registers->a);
			break;
		case 0xB0:  // OR B
			or_(registers->b);
			break;
		case 0xB1:  // OR C
			or_(registers->c);
			break;
		case 0xB2:  // OR D
			or_(registers->d);
			break;
		case 0xB3:  // OR E
			or_(registers->e);
			break;
		case 0xB4:  // OR H
			or_(registers->h);
			break;
		case 0xB5:  // OR L
			or_(registers->l);
			break;
		case 0xB6:  // OR (HL)
			or_(mmu->read_byte(registers->hl));
			break;
		case 0xB7:  // OR A
			or_(registers->a);
			break;
		case 0xB8:  // CP B
			cp(registers->b);
			break;
		case 0xB9:  // CP C
			cp(registers->c);
			break;
		case 0xBA:  // CP D
			cp(registers->d);
			break;
		case 0xBB:  // CP E
			cp(registers->e);
			break;
		case 0xBC:  // CP H
			cp(registers->h);
			break;
		case 0xBD:  // CP L
			cp(registers->l);
			break;
		case 0xBE:  // CP (HL)
			cp(mmu->read_byte(registers->hl));
			break;
		case 0xBF:  // CP A
			cp(registers->a);
			break;
		case 0xC0:  // RET NZ
			ret(!registers->is_flag_set(FLAG_ZERO));
			break;
		case 0xC1:  // POP BC
			registers->bc = mmu->read_short_stack(&registers->sp);
			break;
		case 0xC2:  // JP NZ, nn
			jump(!registers->is_flag_set(FLAG_ZERO));
			break;
		case 0xC3:  // JP nn
			registers->pc = mmu->read_short(registers->pc);
			break;
		case 0xC4:  // CALL NZ, nn
			call(!registers->is_flag_set(FLAG_ZERO));
			break;
		case 0xC5:  // PUSH BC
			mmu->write_short_stack(&registers->sp, registers->bc);
			break;
		case 0xC6:  // ADD A, n
			add(&registers->a, mmu->read_byte(registers->pc++));
			break;
		case 0xC7:  // RST $00
			mmu->write_short_stack(&registers->sp, registers->pc++);
			registers->pc = 0x0000;
			break;
		case 0xC8:  // RET Z
			ret(registers->is_flag_set(FLAG_ZERO));
			break;
		case 0xC9:  // RET
			registers->pc = mmu->read_short_stack(&registers->sp);
			break;
		case 0xCA:  // JP Z, nn
			jump(registers->is_flag_set(FLAG_ZERO));
			break;
		case 0xCB:
			extended_execute(mmu->read_byte(registers->pc++));
			// registers->pc++;
			break;
		case 0xCC:  // CALL Z, nn
			call(registers->is_flag_set(FLAG_ZERO));
			break;

		case 0xCD:  // CALL nn
		{
			uint16_t operand = mmu->read_short(registers->pc);
			registers->pc += 2;
			mmu->write_short_stack(&registers->sp, registers->pc);
			registers->pc = operand;
		} break;
		case 0xCE:  // ADC A, n
			adc(mmu->read_byte(registers->pc++));
			break;
		case 0xCF:  // RST $08
			mmu->write_short_stack(&registers->sp, registers->pc++);
			registers->pc = 0x0008;
			break;
		case 0xD0:  // RET NC
			ret(!registers->is_flag_set(FLAG_CARRY));
			break;
		case 0xD1:  // POP DE
			registers->de = mmu->read_short_stack(&registers->sp);
			break;
		case 0xD2:  // JP NC, nn
			jump(!registers->is_flag_set(FLAG_CARRY));
			break;
		case 0xD4:  // CALL NC, nn
			call(!registers->is_flag_set(FLAG_CARRY));
			break;
		case 0xD5:  // PUSH DE
			mmu->write_short_stack(&registers->sp, registers->de);
			break;
		case 0xD6:  // SUB n
			sub(mmu->read_byte(registers->pc++));
			break;
		case 0xD7:  // RST $10
			mmu->write_short_stack(&registers->sp, registers->pc++);
			registers->pc = 0x0010;
			break;
		case 0xD8:  // RET C
			ret(registers->is_flag_set(FLAG_CARRY));
			break;
		case 0xD9:  // RETI
			interrupts->set_master_flag(true);
			registers->pc = mmu->read_short_stack(&registers->sp);
			break;
		case 0xDA:  // JP C, nn
			jump(registers->is_flag_set(FLAG_CARRY));
			break;
		case 0xDC:  // CALL C, nn
			call(registers->is_flag_set(FLAG_CARRY));
			break;
		case 0xDE:  // SUB n
			sbc(mmu->read_byte(registers->pc++));
			break;
		case 0xDF:  // RST $18
			mmu->write_short_stack(&registers->sp, registers->pc++);
			registers->pc = 0x0018;
			break;
		case 0xE0:  // LD ($FF00+n), A
			mmu->write_byte(0xff00 + mmu->read_byte(registers->pc++), registers->a);
			break;
		case 0xE1:  // POP HL
			registers->hl = mmu->read_short_stack(&registers->sp);
			break;
		case 0xE2:  // LD ($FF00+C), A
			mmu->write_byte(0xff00 + registers->c, registers->a);
			break;
		case 0xE5:  // PUSH HL
			mmu->write_short_stack(&registers->sp, registers->hl);
			break;
		case 0xE6:  // AND n
			and_(mmu->read_byte(registers->pc++));
			break;
		case 0xE7:  // RST $20
			mmu->write_short_stack(&registers->sp, registers->pc++);
			registers->pc = 0x0020;
			break;
		case 0xE8:  // ADD SP, n
			add(&registers->sp, (int8_t)mmu->read_byte(registers->pc++));
			break;
		case 0xE9:  // JP HL
			registers->pc = registers->hl;
			break;
		case 0xEA:  // LD (nn), A
			mmu->write_byte(mmu->read_short(registers->pc), registers->a);
			registers->pc += 2;
			break;
		case 0xEE:  // XOR n
			xor_(mmu->read_byte(registers->pc++));
			break;
		case 0xEF:  // RST $28
			mmu->write_short_stack(&registers->sp, registers->pc++);
			registers->pc = 0x0028;
			break;
		case 0xF0:  // LD A, ($FF00+n)
			registers->a = mmu->read_byte(0xff00 + mmu->read_byte(registers->pc++));
			break;
		case 0xF1:  // POP AF
			registers->af = mmu->read_short_stack(&registers->sp);
			registers->f &= 0xf0;  // Reset the 4 unused bits
			break;
		case 0xF2:  // LD A, (C)
			registers->a = mmu->read_byte(0xff00 + registers->c);
			break;
		case 0xF3:  // DI
			interrupts->set_master_flag(false);
			break;
		case 0xF5:  // PUSH AF
			mmu->write_short_stack(&registers->sp, registers->af);
			break;
		case 0xF6:  // OR n
			or_(mmu->read_byte(registers->pc++));
			break;
		case 0xF7:  // RST $30
			mmu->write_short_stack(&registers->sp, registers->pc++);
			registers->pc = 0x0030;
			break;
		case 0xF8:  // LDHL SP, n
			ldhl(mmu->read_byte(registers->pc++));
			break;
		case 0xF9:  // LD SP, HL
			registers->sp = registers->hl;
			break;
		case 0xFA:  // LD A, (nn)
			registers->a = mmu->read_byte(mmu->read_short(registers->pc));
			registers->pc += 2;
			break;
		case 0xFB:  // NI
			interrupts->set_master_flag(true);
			break;
		case 0xFE:  // CP n
			cp_n(mmu->read_byte(registers->pc++));
			break;
		case 0xFF:  // RST $38
			mmu->write_short_stack(&registers->sp, registers->pc++);
			registers->pc = 0x0038;
			break;*/
	default:

		//printf("[ERROR] Unknown opcode: 0x%02x at 0x%04x | ", opcode, PC);
		//printf("DIV: %d\n", mmu->timer.div);
		//printf("Cycles: %d\n", t_cycles);
		return;
		break;
	}
	printf("[INFO] Opcode 0x%02x at 0x%04x executed\n", opcode, PC);

}
void CPU::Cycle() {
	if (!halted) {

		uint8_t opcode = bus.bus_read(PC++);
		currOpcode = opcode;



		/*printf("%04X: %-7s (%02X %02X %02X) A: %02X B: %02X C: %02X\n",
			PC, insts.inst_name(cur_inst->type), cur_opcode,
			bus.bus_read(PC + 1), bus.bus_read(PC + 2), getReg(REG_A), getReg(REG_B), getReg(REG_B));*/



		temp_t_cycles = 0; // Reset before instruction
		ExecuteInstruction(opcode);
		t_cycles += temp_t_cycles;
		m_cycles += temp_t_cycles / 4;
	}
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