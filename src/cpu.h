//
// Created by sarbajit on 5/5/17.
//

#ifndef CHIP8_CHIP8EMULATOR_H
#define CHIP8_CHIP8EMULATOR_H


#include <cstdint>
#include <string>
#include <chrono>
#include <random>
class CPU
{

private:
    union RegisterPair {
        struct {
            uint8_t lo;
            uint8_t hi;
        };
        uint16_t full;

        RegisterPair() : full(0) {}
    };

    RegisterPair AF, BC, DE, HL;
    uint16_t SP, PC;

    // Memory (simplified, 64KB)
    uint8_t memory[0x10000];

    uint64_t cycles;

    // Helpers
  

    void ExecuteInstruction(uint8_t opcode);
    void add_to_a(uint8_t value, bool use_carry);
    void sub_from_a(uint8_t value, bool use_carry);
    void and_with_a(uint8_t value);
    void xor_with_a(uint8_t value);
    void or_with_a(uint8_t value);
    void cp_with_a(uint8_t value);
    uint16_t read16(uint16_t addr);
    void push(uint16_t val);
    uint16_t pop();

public:

    CPU();
    bool LoadROM(std::string filename);
    void Cycle();
    
    void d_PrintState(); // Executes one instruction
    ~CPU();
};


#endif