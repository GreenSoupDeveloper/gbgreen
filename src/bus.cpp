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
		return cpu.vram[addr - 0x8000];
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		return cpu.eram[addr - 0xA000];
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {
		return cpu.wram[addr - 0xC000];
	}
	else if (addr >= 0xE000 && addr <= 0xFDFF) {
		return cpu.wram[addr - 0xE000]; // Echo of 0xC000–0xDDFF
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		return cpu.oam[addr - 0xFE00];
	}
	else if (addr >= 0xFF00 && addr <= 0xFF7F) {
		return cpu.io[addr - 0xFF00];
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFE) {
		return cpu.hram[addr - 0xFF80];
	}
	/*else if (addr == 0xFFFF) {
		return interrupt_enable;
	}*/
	return 0xFF; // Unusable or unimplemented area
}


void bus_write(uint16_t address, uint8_t value) {
    if (address < 0x8000) {
        //ROM Data
        //cart_write(address, value);
        return;
    }

    //more to do now
}