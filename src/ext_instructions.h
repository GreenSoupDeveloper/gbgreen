#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cpu.h>

class ExtInstruction {

public:
	const uint8_t extendedInstructionTicks[256] = {
		8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0x0_
		8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0x1_
		8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0x2_
		8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0x3_
		8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,  // 0x4_
		8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,  // 0x5_
		8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,  // 0x6_
		8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,  // 0x7_
		8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0x8_
		8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0x9_
		8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0xa_
		8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0xb_
		8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0xc_
		8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0xd_
		8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0xe_
		8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8   // 0xf_
	};
	




	ExtInstruction();

	void ExecuteExtInstruction(uint8_t opcode);



	void rlc_r(Instruction::Register8 reg);
	void rlc_rr(CPU::RegisterPair pair);
	void rrc_r(Instruction::Register8 reg);
	void rrc_rr(CPU::RegisterPair pair);
	void rl_r(Instruction::Register8 reg);
	void rl_rr(CPU::RegisterPair pair);
	void rr_r(Instruction::Register8 reg);
	void rr_rr(CPU::RegisterPair pair);
	void sla_r(Instruction::Register8 reg);
	void sla_rr(CPU::RegisterPair pair);
	void sra_r(Instruction::Register8 reg);
	void sra_rr(CPU::RegisterPair pair);
	void swap_r(Instruction::Register8 reg);
	void swap_rr(CPU::RegisterPair pair);
	void srl_r(Instruction::Register8 reg);
	void srl_rr(CPU::RegisterPair pair);
	void bit_n_r(uint8_t bit, Instruction::Register8 reg);
	void bit_n_rr(uint8_t bit, CPU::RegisterPair reg);
	void res_n_r(uint8_t bit, Instruction::Register8 reg);
	void res_n_rr(uint8_t bit, CPU::RegisterPair reg);
	void set_n_r(uint8_t bit, Instruction::Register8 reg);
	void set_n_rr(uint8_t bit, CPU::RegisterPair reg);


	~ExtInstruction();



};
extern ExtInstruction extInsts;
