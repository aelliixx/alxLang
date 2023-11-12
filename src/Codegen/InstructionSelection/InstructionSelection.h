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
	Target m_target{};

protected:
	const std::vector<IRNodes>& m_ir;

	InstructionSelection(const std::vector<IRNodes>& ir, const Target& target) : m_target(target), m_ir(ir) {}

	virtual void Select();
	virtual void PrintInstructions() const;

public:
	virtual ~InstructionSelection() = default;
};


} // namespace alx::ir
