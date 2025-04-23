#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
class MBC {
public:
	uint8_t currentBank = 1;
	uint8_t rom_bank0[0x4000];
	uint8_t rom_bank1[0x4000];
};
extern MBC mbc;