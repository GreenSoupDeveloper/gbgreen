#include <cstring>
#include <fstream>
#include <iostream>
#include <cpu.h>
#include <cartridge.h>
#include <tools.h>
#include <iomanip>
Cartridge::Cartridge() {

}

bool Cartridge::LoadROM(std::string filename) {
	tools.RenderToDisplay("assets/loading");
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{

		std::cout << "INFO: Loading ROM..\n";
		// get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];




		// go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();
		if (size == 256) {
			std::cout << "INFO: GameBoy Bootroom loaded.\n\n";
		}
		if (size > 32768) {
			std::cout << "INFO: ROM size is higher than 32768 bytes (32KB)! ROM will continue executing if it has MBC.\n";
			size = 0x8000;
		}

		if (buffer[0x147] == 0x00) {
			std::cout << "INFO: ROM doesn't have MBC. ROM Size should be 32KB then.\n\n";
		}
		else if (buffer[0x147] == 0x01 || buffer[0x147] == 0x02 || buffer[0x147] == 0x03) {
			std::cout << "INFO: ROM has MBC1. ROM Size should be 64KB min, and 2MB max then.\n\n";
		}
		else if (buffer[0x147] == 0x05 || buffer[0x147] == 0x06) {
			std::cout << "INFO: ROM has MBC2. ROM Size should be 64KB min, and 256KB max then.\n\n";
		}
		else if (buffer[0x147] == 0x0F || buffer[0x147] == 0x10 || buffer[0x147] == 0x11 || buffer[0x147] == 0x12 || buffer[0x147] == 0x13) {
			std::cout << "INFO: ROM has MBC3. ROM Size should be 64KB min, and 2MB max then.\n\n";
		}
		else if (buffer[0x147] == 0x19 || buffer[0x147] == 0x1A || buffer[0x147] == 0x1B || buffer[0x147] == 0x1C || buffer[0x147] == 0x1D || buffer[0x147] == 0x1E) {
			std::cout << "INFO: ROM has MBC5. ROM Size should be 64KB min, and 8MB max then.\n\n";
		}
		else if (buffer[0x147] == 0x20) {
			std::cout << "INFO: ROM has MBC6. ROM Size should be 64KB min, and 1MB max then.\n\n";
		}
		else if (buffer[0x147] == 0x22) {
			std::cout << "INFO: ROM has MBC7. ROM Size should be 64KB min, and 8MB max then.\n\n";
		}

		// load the rom into the gb's memory, since its going to rom_data and not main memory, it wont start at 0x0100 (256 bytes)
		for (long i = 0; i < size; ++i)
		{
			if (i > 0x0103 && i < 0x0134) {
				logo[i] = buffer[i];
				
			}
			else
			rom_data[i] = buffer[i];
		}

		// clean the buffer
		delete[] buffer;
		return true;
	}
	return false;

}