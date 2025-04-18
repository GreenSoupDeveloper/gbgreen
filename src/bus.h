#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
class Bus {
public:
	// Memory (simplified, 64KB)
	uint8_t wram[0x2000];   // 8KB Work RAM (0xC000–0xDFFF)
	uint8_t vram[0x2000];   // 8KB Video RAM (0x8000–0x9FFF)
	uint8_t eram[0x2000];   // 8KB External RAM (0xA000–0xBFFF)
	uint8_t oam[0xA0];      // Object Attribute Memory (0xFE00–0xFE9F)
	uint8_t hram[0x7F];     // High RAM (0xFF80–0xFFFE)
	uint8_t io[0x80]; // I/O ports (0xFF00–0xFF7F)
	uint8_t IE = 0x00;    // 0xFFFF
	uint8_t IF = 0xE1;    // 0xFF0F
	uint8_t bus_read(uint16_t address);
	void bus_write(uint16_t address, uint8_t value);
};
extern Bus bus;