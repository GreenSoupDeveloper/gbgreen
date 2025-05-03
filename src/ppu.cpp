#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <ppu.h>
#include <bus.h>
#include <emulator.h>

const int FB_SIZE = (256 * 256 * 3);
const int FB_SIZE_A = (256 * 256 * 4);

void PPU::ppu_init() {
    SCY = 0;
    SCX = 0;
    memset(bgmap, 0, FB_SIZE);
    memset(spritemap, 0x69, FB_SIZE);
    memset(winmap, 0x0, FB_SIZE);
    memset(bgmapA, 0, FB_SIZE_A);
    memset(spritemapA, 0, FB_SIZE_A);
    memset(winmapA, 0, FB_SIZE_A);

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
  
    return emu.palette[pixelValue];
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
void PPU::displayGraphics() {
    uint16_t currtile = 0x97FF;
    uint8_t currmapx = 0x00;
    uint8_t currmapy = 0x00;
    for (int ry = 0; ry < 144; ry += 8) {
        for (int rx = 0; rx < 160; rx += 8) {
            currtile++;
            if ((currtile & 0x00FF) == 0x0014 || (currtile & 0x00FF) == 0x0034 || (currtile & 0x00FF) == 0x0054 || (currtile & 0x00FF) == 0x0074 || (currtile & 0x00FF) == 0x0094 || (currtile & 0x00FF) == 0x00B4 || (currtile & 0x00FF) == 0x00D4 || (currtile & 0x00FF) == 0x00F4)
                currtile += 12;
            //printf("curtile %04x\n", currtile);

            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    emu.pixels[(ry + y) * 160 + (rx + x)] = ppu.getPixelColor(ppu.getPixel(ppu.getTile(bus.bus_read(currtile)), x, y));
                }
            }
        }
    }
    currtile = 0x97FF;
}