//
// Created by aelliixx on 2023-11-11.
//

#pragma once

#include "../InstructionSelection.h"

namespace alx::ir::x86 {
class X86ISel final : public InstructionSelection
{
public:
	X86ISel(const std::vector<IRNodes>& ir, const Target& target) : InstructionSelection(ir, target) {}

	void DoInstructionSelection();
	void PrintInstructions() const override;
};

} // namespace alx::ir::x86