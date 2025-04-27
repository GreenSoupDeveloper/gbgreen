#include <bus.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cpu.h>
#include <cartridge.h>
#include <mbc.h>
#include <timer.h>

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
	if (addr < 0x8000)
		//return mbc.read_mbc1(addr);
		return cart.rom_data[addr];
	//test area
	else if (addr == 0xFF44) { // LY
		return 0x90; // hardcoded for testing
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
		return wram[addr - 0xE000]; // echo of 0xC000–0xDDFF
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		return oam[addr - 0xFE00];
	}
	else if (addr >= 0xFEA0 && addr <= 0xFEFF) {
		// technically unusable, but some games poke here
		printf("[INFO] ROM is trying to read the forbidden area. Address: 0x%04X\n", addr);
		return 0xFF;
	}
	else if (addr == 0xFF04 || addr == 0xFF05 || addr == 0xFF06 || addr == 0xFF07)
		return timer.timer_read(addr);
	else if (addr == 0xFF0F) {
		return IF;
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFE) {
		return hram[addr - 0xFF80]; // HRAM (unmapped, should persist)
	}
	else if (addr >= 0xFF00 && addr <= 0xFF7F) {
		return io[addr - 0xFF00];
	}
	
	else if (addr == 0xFFFF) {
		return IE;
	}


	else {
		printf("[ERROR] Unknown reading address (Opcode: 0x%02X | Address: 0x%04X).. is it you or is it me?\n", cpu.currOpcode, addr);
	}
	return 0xFF; // unusable or unimplemented area
}


void Bus::bus_write(uint16_t addr, uint8_t value) {

	
	if (addr < 0x8000 || (addr >= 0xA000 && addr < 0xC000))
		mbc.write_mbc1(addr, value);
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
	else if (addr >= 0xFEA0 && addr <= 0xFEFF) {
		// technically unusable, but some games poke here
		printf("[INFO] ROM is trying to write to the forbidden area. Address: 0x%04X\n", addr);
	}

	else if (addr == 0xFF04 || addr == 0xFF05 || addr == 0xFF06 || addr == 0xFF07)
		return timer.timer_write(addr, value);
	else if (addr >= 0xFF80 && addr <= 0xFFFE) {
		hram[addr - 0xFF80] = value;
	}
	else if (addr == 0xFF0F) {
		IF = value;
	}
	else if (addr >= 0xFF00 && addr <= 0xFF7F) {
		io[addr - 0xFF00] = value;
	}
	else if (addr == 0xFFFF) {
		IE = value;
	}





	else {
		printf("[ERROR] Unknown writing address (Opcode: 0x%02X | Address: 0x%04X | Value: 0x%02X (d%d)).. is it you or is it me?\n", cpu.currOpcode, addr, value, value);
	}


	// hi
}