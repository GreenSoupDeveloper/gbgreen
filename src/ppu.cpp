#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <ppu.h>
#include <bus.h>
#include <emulator.h>
#include <cpu.h>
#include <interrupts.h>



void PPU::ppu_init() {
    SCY = 0;
    SCX = 0;
   

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

    return ppu.palette[pixelValue];
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
    //uint8_t tscy = SCY;
  
    for (int ry = 0; ry < 144; ry += 8) {
        for (int rx = 0; rx < 160; rx += 8) {
            currtile++;
          
            if ((currtile & 0x00FF) == 0x0014 || (currtile & 0x00FF) == 0x0034 || (currtile & 0x00FF) == 0x0054 || (currtile & 0x00FF) == 0x0074 || (currtile & 0x00FF) == 0x0094 || (currtile & 0x00FF) == 0x00B4 || (currtile & 0x00FF) == 0x00D4 || (currtile & 0x00FF) == 0x00F4)
                currtile += 12;
            //printf("scy %d\n", SCY);
            

            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    uint8_t tileIndex = bus.bus_read(currtile) & 0xFF;  // Wrap tile index
                    Tile tile = ppu.getTile(tileIndex);

                    // Calculate pixel with bounds checking
                    uint8_t pixel = ppu.getPixel(tile, x % 8, y % 8);  // Wrap within tile
                    uint32_t color = ppu.getPixelColor(pixel);

                    // Calculate screen position with wrapping
                    uint16_t screenX = ((rx + x) + ppu.SCX) % 160;
                    uint16_t screenY = ((ry + y) + (ppu.SCY)) % 144;  // GB screen is 160x144
                  

                    ppu.frameBuffer[screenY * 160 + screenX] = color;
                }
            }
        }
    }
    currtile = 0x97FF;
}
/*void PPU::renderPixel(uint8_t rx, uint8_t ry, uint8_t x, uint8_t y) {
    // 1. Get tile index from tilemap (with wrapping)
    uint16_t tileMapAddr = 0x9800 + ((ry + y + ppu.SCY) / 8) * 32 + ((rx + x + ppu.SCX) / 8);
    uint8_t tileIndex = bus.bus_read(tileMapAddr);

    // 2. Adjust for signed tiles (if LCDC bit 4=0)
    if ((ppu.LCDC.value & 0x10) == 0) {
        tileIndex = (int8_t)tileIndex + 128;  // Convert to unsigned
    }

    // 3. Get tile data
    Tile tile = ppu.getTile(tileIndex);

    // 4. Get pixel within tile (0-7)
    uint8_t tileX = (rx + x + ppu.SCX) % 8;
    uint8_t tileY = (ry + y + ppu.SCY) % 8;
    uint8_t pixel = ppu.getPixel(tile, tileX, tileY);

    // 5. Apply palette and render
    uint16_t screenX = (rx + x) % 160;
    uint16_t screenY = (ry + y) % 144;
    ppu.frameBuffer[screenY * 160 + screenX] = getPixelColor(pixel);
}*/