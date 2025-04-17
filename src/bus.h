#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
class Bus{
public:
uint8_t bus_read(uint16_t address);
void bus_write(uint16_t address, uint8_t value);
};
extern Bus bus;