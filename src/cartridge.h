#pragma once
#include <SDL3/SDL.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
class Cartridge {

public:
	uint8_t entry[4];
	uint8_t logo[0x30];

	char title[16];
	uint16_t new_lic_code;
	uint8_t sgb_flag;
	uint8_t type;
	uint8_t rom_size;
	uint8_t ram_size;
	uint8_t dest_code;
	uint8_t lic_code;
	uint8_t version;
	uint8_t checksum;
	uint16_t global_checksum;

	char filename[1024];
	uint16_t rom_data[0x10000];

	bool LoadROM(std::string filename);
	Cartridge();
	~Cartridge();
	std::string toHexString(const uint8_t* data, size_t length);
};
extern Cartridge cart;