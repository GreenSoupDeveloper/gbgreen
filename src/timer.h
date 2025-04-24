#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cpu.h>

class Timer {
public:
    uint16_t div_counter = 0;   // Internal counter for DIV
    uint16_t tima_counter = 0;  // Internal counter for TIMA

    uint8_t DIV = 0xAC00;
    uint8_t TIMA = 0;
    uint8_t TMA = 0;
    uint8_t TAC = 0;

    void timer_init();
    void timer_tick();
    uint16_t getTimerThreshold();
    void timer_write(uint16_t address, uint8_t value);
    uint8_t timer_read(uint16_t address);

  
};
extern Timer timer;
