#include <io.h>
#include <cpu.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <bus.h>

static char serial_data[2];
IO::IO() {

}

uint8_t IO::io_read(uint16_t address) {
    if (address == 0xFF01) {
        return serial_data[0];
    }

    if (address == 0xFF02) {
        return serial_data[1];
    }

  

    printf("UNSUPPORTED bus_read(%04X)\n", address);
    return 0;
}

void IO::io_write(uint16_t address, uint8_t value) {
    if (address == 0xFF01) {
        serial_data[0] = value;
        return;
    }

    if (address == 0xFF02) {
        serial_data[1] = value;
        return;
    }

 

    printf("UNSUPPORTED bus_write(%04X)\n", address);
}
static char dbg_msg[1024] = { 0 };
static int msg_size = 0;

void IO::dbg_update() {
    //printf("dbg %04X\n", bus.bus_read(0xFF02));
   // printf("DEBUG: %02x", bus.bus_read(0xFF02));

    if (bus.bus_read(0xFF02) == 0x81) {
        char c = bus.bus_read(0xFF01);
        //printf("DEBUG: %s", c);
        dbg_msg[msg_size++] = c;
        

        bus.bus_write(0xFF02, 0x00);
    }
   
}

void IO::dbg_print() {
    if (dbg_msg[0]) {
        printf("Serial Bus: %s\n", dbg_msg);
    }
}