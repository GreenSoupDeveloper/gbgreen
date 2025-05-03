#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <interrupts.h>
#include <bus.h>
#include <cpu.h>




void Interrupt::set_interrupt_flag(uint8_t flag) {
	uint8_t IF_value = bus.bus_read(0xFF0F);
	IF_value |= flag;
	return bus.bus_write(0xFF0F, IF_value);
}

void Interrupt::unset_interrupt_flag(uint8_t flag) {
	uint8_t IF_value = bus.bus_read(0xFF0F);
	IF_value &= ~flag;
	return bus.bus_write(0xFF0F, IF_value);
}
bool Interrupt::check() {
	if (bus.bus_read(0xFFFF) & bus.bus_read(0xFF0F) & 0x0F)
		cpu.halted = false;

	if (!cpu.IME)
		return false;

	// Check if VBLANK
	if (bus.bus_read(0xFFFF) & IT_VBLANK && bus.bus_read(0xFF0F) & IT_VBLANK) {
		this->trigger_interrupt(IT_VBLANK, 0x40);
		return true;
	}

	// Check if LCD
	if (bus.bus_read(0xFFFF) & IT_LCD_STAT && bus.bus_read(0xFF0F) & IT_LCD_STAT) {
		this->trigger_interrupt(IT_LCD_STAT, 0x48);
		return true;
	}

	// Check if TIMER
	if (bus.bus_read(0xFFFF) & IT_TIMER && bus.bus_read(0xFF0F) & IT_TIMER) {
		this->trigger_interrupt(IT_TIMER, 0x50);
		return true;
	}

	// Check if JOYPAD
	
	if (bus.bus_read(0xFFFF) & IT_JOYPAD && bus.bus_read(0xFF0F) & IT_JOYPAD) {
		this->trigger_interrupt(IT_JOYPAD, 0x60);
		return true;
	}

	return false;
}

void Interrupt::trigger_interrupt(interrupt_type interrupt, uint8_t jump_pc) {
	
	cpu.Push16(cpu.PC);
	cpu.PC = jump_pc;
	cpu.IME = false;
	this->unset_interrupt_flag(interrupt);
	cpu.halted = false;

	cpu.temp_t_cycles += 20;
}
