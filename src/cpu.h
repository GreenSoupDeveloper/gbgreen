#pragma once
#include <cstdint>
#include <string>
#include <chrono>
#include <random>
#include <cartridge.h>

class CPU
{

public:
    // this is the zone where the registers are stored.
    union RegisterPair {
        struct {
            uint8_t lo;
            uint8_t hi;
        };
        uint16_t full;

        RegisterPair() : full(0) {}
    }; // this thing is for the register pairs, made to access the 16-bit regs and 8-bit regs

    enum Flag {
        FLAG_Z = 1 << 7, // Zero
        FLAG_N = 1 << 6, // Subtract
        FLAG_H = 1 << 5, // Half Carry
        FLAG_C = 1 << 4  // Carry
    }; // flag values based on the F register

    RegisterPair AF, BC, DE, HL; // registers
    uint16_t SP = 0xFFFE; // stack pointer
    uint16_t PC = 0x100; // pc, always starts at 0x100 (256 bytes)
    int ticks; // emulator ticks

    uint8_t currOpcode = 0x00; // current opcode



    uint64_t temp_t_cycles; // temp cycles used to add to the normal t_cycles
    uint64_t t_cycles; // t cycles
    uint64_t m_cycles; // m cycles
    bool IME = false; // interrupt master enable
    bool halted = false; // halt
    bool haltBug = false; // halt bug (this is not necessary, its just for some accuracy)
    

    const uint8_t nintendoLogo[0x30] = {
        0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
    }; // nintendo logo used to compare to the rom's one

    typedef enum {
        IT_VBLANK = 1,
        IT_LCD_STAT = 2,
        IT_TIMER = 4,
        IT_SERIAL = 8,
        IT_JOYPAD = 16
    } interrupt_type; // interrupt types

    interrupt_type interrupts;


   
    
    void initializeGameboy();
    void Push16(uint16_t value); // push 16-bytes to stack
    uint16_t Read16(uint16_t addr); // read 16-bit byte from memory
    bool InterruptCheck(uint16_t address, interrupt_type it); // to check interrupt things
    void HandleInterrupt(); // handle interrupts
    void RequestInterrupt(interrupt_type t); // request interrupts


    void ExecuteInstruction(uint8_t opcode); // execute cpu opcodes
    
    

    void halt(); // halt cpu


    CPU();
    void Cycle(); // cpu cycle
    ~CPU();
};



extern CPU cpu;