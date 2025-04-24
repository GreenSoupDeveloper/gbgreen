#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cpu.h>
#include <timer.h>
#include <bus.h>

void Timer::timer_init() {
    DIV = 0xAC00;
}
void Timer::timer_tick() {
    uint16_t prev_div = DIV;

    DIV++;

    bool timer_update = false;

    switch (TAC & (0b11)) {
    case 0b00:
        timer_update = (prev_div & (1 << 9)) && (!(DIV & (1 << 9)));
        break;
    case 0b01:
        timer_update = (prev_div & (1 << 3)) && (!(DIV & (1 << 3)));
        break;
    case 0b10:
        timer_update = (prev_div & (1 << 5)) && (!(DIV & (1 << 5)));
        break;
    case 0b11:
        timer_update = (prev_div & (1 << 7)) && (!(DIV & (1 << 7)));
        break;
    }

    if (timer_update && TAC & (1 << 2)) {
        TIMA++;

        if (TIMA == 0xFF) {
            TIMA = TMA;

            cpu.RequestInterrupt(cpu.IT_TIMER);
        }
    }
}

uint16_t Timer::getTimerThreshold() {
    switch (TAC & 0x03) {
    case 0: return 1024; // 4096 Hz
    case 1: return 16;   // 262144 Hz
    case 2: return 64;   // 65536 Hz
    case 3: return 256;  // 16384 Hz
    }
    return 1024;
}

void Timer::timer_write(uint16_t address, uint8_t value) {
    switch (address) {
    case 0xFF04:
        //DIV
        DIV = 0;
        break;

    case 0xFF05:
        //TIMA
        TIMA = value;
        break;

    case 0xFF06:
        //TMA
        TMA = value;
        break;

    case 0xFF07:
        //TAC
        TAC = value;
        break;
    }
}

uint8_t Timer::timer_read(uint16_t address) {
    switch (address) {
    case 0xFF04:
        return DIV >> 8;
    case 0xFF05:
        return TIMA;
    case 0xFF06:
        return TMA;
    case 0xFF07:
        return TAC;
    }
}