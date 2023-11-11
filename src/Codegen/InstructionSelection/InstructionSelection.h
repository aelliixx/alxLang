/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-11-06.
//

#pragma once

#include "Target.h"
#include "../../IR/Ir.h"
namespace alx::ir {

struct InstructionInfo {
	virtual ~InstructionInfo() = default;
	virtual void GetInstruction(std::variant<BodyTypes, IdentifierInstruction>) = 0;
};


class InstructionSelection
{
	const std::vector<IRNodes>& m_ir;
	Target m_target{};

protected:
	virtual void PrintInstructions() const;
	InstructionSelection(const std::vector<IRNodes>& ir, const Target& target) : m_ir(ir), m_target(target) {}

public:
	virtual ~InstructionSelection() = default;
};


} // namespace alx::ir
