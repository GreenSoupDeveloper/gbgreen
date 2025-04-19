#include <bus.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cpu.h>
#include <cartridge.h>

// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x97FF : CHR RAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers
// 0xFF80 - 0xFFFE : Zero Page

uint8_t Bus::bus_read(uint16_t addr) {
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


void Bus::bus_write(uint16_t addr, uint8_t value) {
	if (addr <= 0x7FFF) {
		//cart.rom_data[addr]; // ROM region
		printf("[ERROR] You can't write to ROM dude. Opcode: 0x%02X | Address: 0x%04X | Value: 0x%02X (d%d)\n", cpu.currOpcode, addr, value, value);
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF) {
		vram[addr - 0x8000] = value;
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		eram[addr - 0xA000] = value;
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {
		wram[addr - 0xC000] = value;
	}
	else if (addr >= 0xE000 && addr <= 0xFDFF) {
		wram[addr - 0xE000] = value; // Echo of 0xC000–0xDDFF
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		oam[addr - 0xFE00] = value;
	}
	else if (addr >= 0xFF00 && addr <= 0xFF7F) {
		io[addr - 0xFF00] = value;
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFE) {
		hram[addr - 0xFF80] = value;
	}
	else {
		printf("[ERROR] Unknown address (Opcode: 0x%02X | Address: 0x%04X | Value: 0x%02X (d%d)).. is it you or is it me?\n", cpu.currOpcode, addr, value, value);
	}

    //more to do now
}