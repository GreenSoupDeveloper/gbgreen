#include <cstring>
#include <fstream>
#include <iostream>

#include <cartridge.h>
#include <tools.h>
#include <iomanip>
#include <cpu.h>
#include <sstream>
#include <vector>
Cartridge::Cartridge() {

}
const char* ROM_TYPES[0x100] = {
   nullptr
};



bool Cartridge::LoadROM(std::string filename) {
	ROM_TYPES[0x00] = "ROM ONLY";
	ROM_TYPES[0x01] = "MBC1";
	ROM_TYPES[0x02] = "MBC1+RAM";
	ROM_TYPES[0x03] = "MBC1+RAM+BATTERY";
	ROM_TYPES[0x04] = "0x04 ???";
	ROM_TYPES[0x05] = "MBC2";
	ROM_TYPES[0x06] = "MBC2+BATTERY";
	ROM_TYPES[0x07] = "0x07 ???";
	ROM_TYPES[0x08] = "ROM+RAM 1";
	ROM_TYPES[0x09] = "ROM+RAM+BATTERY 1";
	ROM_TYPES[0x0A] = "0x0A ???";
	ROM_TYPES[0x0B] = "MMM01";
	ROM_TYPES[0x0C] = "MMM01+RAM";
	ROM_TYPES[0x0D] = "MMM01+RAM+BATTERY";
	ROM_TYPES[0x0E] = "0x0E ???";
	ROM_TYPES[0x0F] = "MBC3+TIMER+BATTERY";
	ROM_TYPES[0x10] = "MBC3+TIMER+RAM+BATTERY 2";
	ROM_TYPES[0x11] = "MBC3";
	ROM_TYPES[0x12] = "MBC3+RAM 2";
	ROM_TYPES[0x13] = "MBC3+RAM+BATTERY 2";
	ROM_TYPES[0x14] = "0x14 ???";
	ROM_TYPES[0x15] = "0x15 ???";
	ROM_TYPES[0x16] = "0x16 ???";
	ROM_TYPES[0x17] = "0x17 ???";
	ROM_TYPES[0x18] = "0x18 ???";
	ROM_TYPES[0x19] = "MBC5";
	ROM_TYPES[0x1A] = "MBC5+RAM";
	ROM_TYPES[0x1B] = "MBC5+RAM+BATTERY";
	ROM_TYPES[0x1C] = "MBC5+RUMBLE";
	ROM_TYPES[0x1D] = "MBC5+RUMBLE+RAM";
	ROM_TYPES[0x1E] = "MBC5+RUMBLE+RAM+BATTERY";
	ROM_TYPES[0x1F] = "0x1F ???";
	ROM_TYPES[0x20] = "MBC6";
	ROM_TYPES[0x21] = "0x21 ???";
	ROM_TYPES[0x22] = "MBC7+SENSOR+RUMBLE+RAM+BATTERY";
	ROM_TYPES[0xFC] = "POCKET CAMERA";
	ROM_TYPES[0xFD] = "BANDAI TAMA5";
	ROM_TYPES[0xFE] = "HuC3";
	ROM_TYPES[0xFF] = "HuC1 + RAM + BATTERY";

	tools.RenderToDisplay("assets/loading");
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{

		std::cout << "[INFO] Loading ROM..\n";
		// get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];




		// go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();
		if (size > 32768) {
			std::cout << "[INFO] ROM size is higher than 32768 bytes (32KB)! ROM will continue executing if it has MBC.\n";
			size = 0x8000;
		}
		cart.type = buffer[0x147];

		std::cout << "[INFO] ROM Type: " << ROM_TYPES[cart.type] << "\n";



		// load the rom into the gb's memory, since its going to rom_data and not main memory, it wont start at 0x0100 (256 bytes)
		int logocount = 0;
		int titlecount = 0;
		std::string titleting = "";
		bool logomatches = true;
		for (long i = 0; i < size; ++i)
		{
			if (i > 0x0103 && i < 0x0134) {
				logo[logocount] = buffer[i];
				if (logo[logocount] == cpu.nintendoLogo[logocount]) {

				}
				else {
					logomatches = false;
				}
				logocount++;


			}
			else if (i > 0x0133 && i < 0x0144) {
				title[titlecount] = buffer[i];
				titleting += title[titlecount];
				titlecount++;



			}
			
			rom_data[i] = buffer[i];
		}
		uint16_t x = 0;
		for (uint16_t i = 0x0134; i <= 0x014C; i++) {
			x = x - rom_data[i] - 1;
		}
		cart.checksum = rom_data[0x014D];
		printf("[INFO] Checksum : %2.2X (%s)\n", cart.checksum, (x & 0xFF) ? "PASSED" : "FAILED");
		if (logomatches) {
			printf("[INFO] Nintendo logo matches!\n\n");
		}
		else {
			printf("[INFO] Nintendo logo doesn't match! This means this rom is not official, or its corrupted.\n\n");

		}
		std::cout << "[INFO] ROM '" << titleting << "' Loaded!\n\n";


		// clean the buffer
		delete[] buffer;
		return true;
	}
	return false;

}

std::string Cartridge::toHexString(const uint8_t* data, size_t length) {
	std::ostringstream oss;
	for (size_t i = 0; i < length; ++i) {
		oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
		if (i < length - 1) oss << " ";
	}
	return oss.str();
}