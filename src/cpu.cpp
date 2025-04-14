#include <cstring>
#include <fstream>
#include <iostream>
#include <cpu.h>
CPU::CPU()
{

}

bool CPU::LoadROM(std::string filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
   
        if (file.is_open())
        {
            std::cout << "Loading ROM..\n";
            // get size of file and allocate a buffer to hold the contents
            std::streampos size = file.tellg();
            char* buffer = new char[size];
          
           
                

            // go back to the beginning of the file and fill the buffer
            file.seekg(0, std::ios::beg);
            file.read(buffer, size);
            file.close();
            if (size > 0x7fff)
                std::cout << "careful, rom size is higher than 0x7fff! gonna implement mbc later.";

            // load the rom into the gb's memory, starting at 0x0100 (256 bytes)
            for (long i = 0; i < size; ++i)
            {
                memory[0x0100 + i] = buffer[i];
            }

            // clean the buffer
            delete[] buffer;
            return true;
        }
        return false;
   
}
void CPU::add_to_a(uint8_t value, bool use_carry = false) {
    uint16_t a = AF.hi;
    uint16_t c = (use_carry && (AF.lo & 0x10)) ? 1 : 0;
    uint16_t result = a + value + c;

    // Set flags
    AF.lo = 0;
    if ((result & 0xFF) == 0) AF.lo |= 0x80;             // Z
    if (use_carry == false) AF.lo &= ~0x40;              // N = 0
    if (((a & 0xF) + (value & 0xF) + c) > 0xF) AF.lo |= 0x20; // H
    if (result > 0xFF) AF.lo |= 0x10;                    // C

    AF.hi = static_cast<uint8_t>(result);
}
void CPU::sub_from_a(uint8_t value, bool use_carry = false) {
    uint16_t a = AF.hi;
    uint16_t c = (use_carry && (AF.lo & 0x10)) ? 1 : 0;
    uint16_t result = a - value - c;

    // Reset all flags
    AF.lo = 0;

    if ((result & 0xFF) == 0) AF.lo |= 0x80;             // Z
    AF.lo |= 0x40;                                       // N is always set
    if (((a & 0xF) - (value & 0xF) - c) & 0x10) AF.lo |= 0x20; // H
    if (result > 0xFF) AF.lo |= 0x10;                    // C (borrow)

    AF.hi = static_cast<uint8_t>(result);
}
void CPU::and_with_a(uint8_t value) {
    AF.hi &= value;

    // Flags: Z 0 1 0
    AF.lo = 0;
    if (AF.hi == 0) AF.lo |= 0x80; // Z
    AF.lo |= 0x20;                 // H
}
void CPU::xor_with_a(uint8_t value) {
    AF.hi ^= value;

    // Flags: Z 0 0 0
    AF.lo = 0;
    if (AF.hi == 0) AF.lo |= 0x80; // Z
}
void CPU::or_with_a(uint8_t value) {
    AF.hi |= value;

    // Flags: Z 0 0 0
    AF.lo = 0;
    if (AF.hi == 0) AF.lo |= 0x80; // Z
}
void CPU::cp_with_a(uint8_t value) {
    uint8_t result = AF.hi - value;

    // Flags: Z 1 H C
    AF.lo = 0;
    if (result == 0) AF.lo |= 0x80;        // Z
    AF.lo |= 0x40;                         // N
    if ((AF.hi & 0x0F) < (value & 0x0F)) AF.lo |= 0x20; // H
    if (AF.hi < value) AF.lo |= 0x10;     // C
}
uint16_t CPU::read16(uint16_t addr) {
    return memory[addr] | (memory[addr + 1] << 8);
}



void CPU::push(uint16_t val) {
    SP -= 2;
    memory[SP] = val & 0xFF;
    memory[SP + 1] = val >> 8;
}



uint16_t CPU::pop() {
    uint16_t val = memory[SP] | (memory[SP + 1] << 8);
    SP += 2;
    return val;
}
void CPU::Cycle() {
    uint8_t opcode = memory[PC++];
    switch (opcode) {
    case 0x00: // NOP
        break;
    default:
        std::cerr << "Unknown opcode: 0x" << std::hex << (int)opcode << "\n";
        break;
    }
}

void CPU::d_PrintState() {
    std::cout << "A: " << std::hex << (int)AF.hi
        << " B: " << (int)BC.hi
        << " C: " << (int)BC.lo
        << " D: " << (int)DE.hi
        << " E: " << (int)DE.lo
        << " F: " << (int)AF.lo
        << " H: " << (int)HL.hi
        << " L: " << (int)HL.lo
        << " SP: " << (int)SP
        << " PC: " << PC << "\n";
}