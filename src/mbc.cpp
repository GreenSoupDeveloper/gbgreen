#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <mbc.h>
#include <cartridge.h>

uint8_t MBC::read_mbc1(uint16_t addr) {
    if (addr < 0x4000) {
        // bank 0, always mapped to 0
        return cart.rom_data[addr];
    }
    else if (addr < 0x8000) {
        // switchable bank area
        uint32_t bank = currentBank;
        if (bank == 0) bank = 1;
        uint32_t offset = (bank * 0x4000) + (addr - 0x4000);
        return cart.rom_data[offset]; // wrap just in case
    }
    else if (addr >= 0xA000 && addr < 0xC000) {
        // ram bank (if enabled)
        if (!ram_enabled) return 0xFF;
        return ram_banks[current_ram_bank][addr - 0xA000];
    }

    return 0xFF; // invalid region for MBC
}

void MBC::write_mbc1(uint16_t addr, uint8_t val) {
    if (addr < 0x2000) {
        // ram enable/disable
        ram_enabled = ((val & 0x0F) == 0x0A);
    }
    else if (addr < 0x4000) {
        // rom bank number (lower 5 bits)
        currentBank = (currentBank & 0x60) | (val & 0x1F);
        if ((currentBank & 0x1F) == 0) currentBank |= 1; // never 0
    }
    else if (addr < 0x6000) {
        // ram bank number or upper rom bank bits (2 bits)
        if (banking_mode) {
            current_ram_bank = val & 0x03;
        }
        else {
            currentBank = (currentBank & 0x1F) | ((val & 0x03) << 5);
            if ((currentBank & 0x1F) == 0) currentBank |= 1;
        }
    }
    else if (addr < 0x8000) {
        // banking mode select
        banking_mode = val & 0x01;
    }
    else if (addr >= 0xA000 && addr < 0xC000) {
        // ram write (if enabled)
        if (ram_enabled) {
            ram_banks[current_ram_bank][addr - 0xA000] = val;
        }
    }
} 
//gonna make the otehrs later