#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cpu.h>

class Timer {
public:
    int gameboy_ticks = 4 * 1024 * 1024;  // 4194304

    uint16_t temp_div = 0;
    uint16_t div = 0;
    uint8_t temp_tima = 0;
    uint8_t tima = 0;
    uint8_t temp_tma = 0;
    uint8_t tma = 0;
    uint8_t temp_tac = 0;
    uint8_t tac = 0;
    void check();

    void inc();

  
};
extern Timer timer;
