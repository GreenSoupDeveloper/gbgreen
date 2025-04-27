#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <windows.h>
#include <cpu.h>
#include <sstream>
#include <tools.h>
#include <emulator.h>


void Tools::RenderToDisplay(std::string filepath) {
	std::fstream file(filepath + ".gbp", std::ios::in);  // Explicitly open for reading
	
	if (!file.is_open()) {  // Fixed typo: is.open() -> is_open()
		std::cerr << "Failed to open '" << filepath << ".gbp' file!" << std::endl;  // Fixed: test -> cerr
		// Changed from SRL_PPL_PRILINE to standard error code
	}

	// Read the entire file into a vector (dynamic array)
	std::vector<char> charArray;
	char ch;  // Fixed variable name: pp -> ch
	while (file.get(ch)) {
		charArray.push_back(ch);
	}
	charArray.erase(std::remove(charArray.begin(), charArray.end(), '\n'), charArray.end());

	// If you need a raw char array instead of vector:
	char* rawArray = new char[charArray.size()];  // Fixed syntax: char: -> char*, new char( -> new char[
	std::copy(charArray.begin(), charArray.end(), rawArray);

	// Don't forget to delete[] rawArray when done!




	for (int i = 0; i < (160 * 144); i++) {





		if (rawArray[i] == '0') {
			emu.pixels[i] = emu.palette[0];
		}
		else if (rawArray[i] == '1') {
			emu.pixels[i] = emu.palette[1];
		}
		else if (rawArray[i] == '2') {
			emu.pixels[i] = emu.palette[2];
		}
		else {
			emu.pixels[i] = emu.palette[3];
		}


	}
	delete[] rawArray;
	
}
uint32_t Tools::hexStringToUint32(const std::string& hexStr) {
	std::stringstream ss;
	ss << std::hex << hexStr;
	uint32_t result;
	
	ss >> result;
	//std::cerr << hexStr << "";
	return result;
}
void Tools::readPaletteFile(const std::string& filename) {
	std::ifstream file(filename);
	std::string line;
	int i = 0;

	if (!file.is_open()) {
		std::cerr << "Failed to open palette file: " << filename << std::endl;
		return;
	}

	while (std::getline(file, line) && i < 4) {
		// Clean up line (remove \r if present on Windows)
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}

		if (!line.empty()) {
			emu.palette[i] = hexStringToUint32(line);
			++i;
		}
	}

	file.close();
}
void Tools::saveConfig() {
	std::ofstream configFile("config.cfg");

	if (!configFile.is_open()) {
		std::cerr << "Failed to open config.cfg for writing!" << std::endl;
		return;
	}

	configFile << "selectedColorPalette=" << emu.selectedColorPallete << std::endl;

	configFile.close();
}
void Tools::getConfig() {
	std::ifstream configFile("config.cfg");
	std::string line;

	if (!configFile.is_open()) {
		std::cerr << "Failed to open config.cfg for reading!" << std::endl;
		 // Default palette if file doesn't exist or fails
	}

	while (std::getline(configFile, line)) {
		if (line.rfind("selectedColorPalette=", 0) == 0) { // starts with
			std::string valueStr = line.substr(21);
			std::stringstream ss(valueStr);
			int palette;
			if (ss >> palette) {
				emu.selectedColorPallete = palette;
			}
		}
	}
}



