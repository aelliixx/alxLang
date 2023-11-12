//
// Created by aelliixx on 2023-11-11.
//

#pragma once

#include "../InstructionSelection.h"

namespace alx::ir::x86 {
class X86ISel final : public InstructionSelection
{
	BodyTypes m_currentInstruction{};
	void Select() override;
	void DoBlockSelection(const LogicalBlock&);

public:
	X86ISel(const std::vector<IRNodes>& ir, const Target& target) : alx::ir::InstructionSelection(ir, target) {}
	void DoInstructionSelection();
	void PrintInstructions() const override;
};

} // namespace alx::ir::x86