#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <ppu.h>
#include <bus.h>

void PPU::ppu_init() {

}

void PPU::ppu_tick() {

}

PPU::Tile PPU::getTile(uint8_t tileIndex) {
    Tile tile;
    uint16_t addr = 0x8000 + (tileIndex * 16);  // 16 bytes per tile

    for (int i = 0; i < 16; i++) {
        tile.data[i] = bus.bus_read(addr + i);  // Convert to VRAM offset
        //printf("dataaa: %02X %04X", tile.data[i], addr);
    }

    return tile;
}
uint32_t PPU::getPixelColor(uint8_t pixelValue) {
    // Game Boy palette: 0=white, 3=black
    const uint32_t palette[4] = {
        0xFFFFFFFF,  // White
        0xAAAAAAFF,  // Light gray
        0x555555FF,  // Dark gray
        0x000000FF   // Black
    };
    return palette[pixelValue];
}
// Get a specific pixel from a tile
uint8_t PPU::getPixel(const Tile& tile, uint8_t x, uint8_t y) {
    // Each pixel = 2 bits (from 2 planes)
    uint8_t lowBit = (tile.data[y * 2] >> (7 - x)) & 1;
    uint8_t highBit = (tile.data[y * 2 + 1] >> (7 - x)) & 1;
    return (highBit << 1) | lowBit;  // Returns 0-3
}

void PPU::ppu_oam_write(uint16_t address, uint8_t value) {
    if (address >= 0xFE00) {
        address -= 0xFE00;
    }

  
    bus.oam[address] = value;
}

uint8_t PPU::ppu_oam_read(uint16_t address) {
    if (address >= 0xFE00) {
        address -= 0xFE00;
    }

 
    return bus.oam[address];
}