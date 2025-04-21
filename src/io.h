#pragma once

#include <cstring>
#include <fstream>
#include <iostream>
class IO {
public:
	IO();
	uint8_t io_read(uint16_t address);
	void io_write(uint16_t address, uint8_t value);
	void dbg_update();
	void dbg_print();
	~IO();
};
extern IO io;