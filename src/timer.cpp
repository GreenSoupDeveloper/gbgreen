#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cpu.h>
#include <timer.h>
#include <bus.h>
#include <interrupts.h>
void Timer::inc() {
   cpu.t_cycles += cpu.temp_t_cycles;
   cpu.t_cycles %= gameboy_ticks;

   temp_div += cpu.temp_t_cycles;

    while (temp_div >= 256) {
        temp_div -= 256;
        div++;
    }

    check();
}

void Timer::check() {
    if (tac & 0x04) {
        temp_tima += cpu.temp_t_cycles;

        int threshold = 0;
        switch (tac & 0x03) {
        case 0:
            threshold = 1024;
            break;
        case 1:
            threshold = 16;
            break;
        case 2:
            threshold = 64;
            break;
        case 3:
            threshold = 256;
            break;
        }
        while (temp_tima >= threshold) {
            temp_tima -= threshold;
            if (tima == 0xFF) {
                tima = bus.bus_read(0xFF06);
                interrupts.set_interrupt_flag(interrupts.IT_TIMER);
            }
            else {
                tima++;
            }
        }
    }
}