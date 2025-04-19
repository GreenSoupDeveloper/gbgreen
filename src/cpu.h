#pragma once
#include <cstdint>
#include <string>
#include <chrono>
#include <random>
#include <cartridge.h>

class CPU
{

public:
    union RegisterPair {
        struct {
            uint8_t lo;
            uint8_t hi;
        };
        uint16_t full;

        RegisterPair() : full(0) {}
    };

    enum Flag {
        FLAG_Z = 1 << 7, // Zero
        FLAG_N = 1 << 6, // Subtract
        FLAG_H = 1 << 5, // Half Carry
        FLAG_C = 1 << 4  // Carry
    };

    RegisterPair AF, BC, DE, HL;
    uint16_t SP, PC;

    uint8_t currOpcode = 0x00;



    uint64_t temp_t_cycles;
    uint64_t t_cycles;
    uint64_t m_cycles;
    bool IME = false; // Interrupt Master Enable
    bool halted = false;
    bool haltBug = false;
    bool interrupts_enabled;

    const uint8_t nintendoLogo[0x30] = {
        0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
    };



   
    

    // Helpers

  

  

    uint8_t ReadByte(uint16_t addr);
    void Push16(uint16_t value);
    uint16_t Read16(uint16_t addr);
    void HandleInterrupt();
    void ExecuteInstruction(uint8_t opcode);
    
    

    void halt();


    CPU();
    void Cycle();
    
    void d_PrintState(); // Executes one instruction
    ~CPU();
};



extern CPU cpu;