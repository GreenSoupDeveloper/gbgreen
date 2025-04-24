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