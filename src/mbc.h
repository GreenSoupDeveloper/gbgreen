#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
class MBC {
public:
	

	uint8_t ram_banks[4][0x2000];
	uint8_t current_ram_bank = 0;
	bool ram_enabled = false;
	bool banking_mode = false; // false = ROM mode, true = RAM mode

	uint8_t rom_bank0[0x4000];
	uint8_t rom_bank1[0x4000];
	uint8_t currentBank = 1;
	

	void loadROM(const std::string& filename);
	uint8_t read_mbc1(uint16_t addr);
	void write_mbc1(uint16_t addr, uint8_t val);
};
extern MBC mbc;