/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-11-09.
//

#pragma once

#include "../MachineInstruction.h"

namespace alx::ir::x86 {

struct MOV32mi final : MachineInstruction {
	std::string Name() override { return "MOV32mi"; }
};

}