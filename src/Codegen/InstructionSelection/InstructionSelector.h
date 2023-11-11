/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-11-06.
//

#pragma once

#include "../../Ir.h"
namespace alx::ir {

struct InstructionInfo {
	virtual void GetInstruction(std::variant<BodyTypes, IdentifierInstruction>) = 0;
};


class InstructionSelector
{
public:
	enum class Target
	{
		X86_64
	};

private:
	std::vector<IRNodes>& m_ir;
	Target m_target = Target::X86_64;

public:
	InstructionSelector(std::vector<IRNodes>& ir, Target target) : m_ir(ir), m_target(target) {}
	void SelectInstructions();
};


} // namespace alx::ir
