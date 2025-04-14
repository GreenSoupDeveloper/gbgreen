#pragma once

#include <cstring>
#include <fstream>
#include <iostream>

enum class GBButtons {
    Up,
    Down,
    Left,
    Right,
    A,
    B,
    Select,
    Start,
};

class Input {
public:
    void button_pressed(GBButtons button);
    void button_released(GBButtons button);
    void write(uint8_t set);

    auto get_input() const->uint8_t;

private:
    void set_button(GBButtons button, bool set);

    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool a = false;
    bool b = false;
    bool select = false;
    bool start = false;

    bool button_switch = false;
    bool direction_switch = false;
};