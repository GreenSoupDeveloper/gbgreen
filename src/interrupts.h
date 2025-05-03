#pragma once
#include <cstdint>
#include <string>
#include <chrono>
#include <random>


class Interrupt
{

public:
    typedef enum {
        IT_VBLANK = 1,
        IT_LCD_STAT = 2,
        IT_TIMER = 4,
        IT_SERIAL = 8,
        IT_JOYPAD = 16
    } interrupt_type; // interrupt types

    interrupt_type interrupts;

    void set_interrupt_flag(uint8_t flag);
    void unset_interrupt_flag(uint8_t flag);
    bool check();
    void trigger_interrupt(interrupt_type interrupt, uint8_t jump_pc);

};



extern Interrupt interrupts;