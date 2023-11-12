/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-11-09.
//

#pragma once

#include "../MachineInstruction.h"

namespace alx::ir::x86 {

struct MOV final : public TwoOperandMI {
	MOV(MachineInstruction* op1, MachineInstruction* op2) : TwoOperandMI(op1, op2) {}
	std::string Name() override { return "MOV"; }
};

struct ADD final : public TwoOperandMI {
	ADD(MachineInstruction* op1, MachineInstruction* op2) : TwoOperandMI(op1, op2) {}
	std::string Name() override { return "ADD"; }
};

using X86MI = std::variant<MOV>;

}