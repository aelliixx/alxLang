//
// Created by aelliixx on 2023-11-11.
//

#include "X86ISel.h"
#include "X86InstructionMatcher.h"

namespace alx::ir::x86 {

void X86ISel::PrintInstructions() const { InstructionSelection::PrintInstructions(); }

void X86ISel::DoBlockSelection(const LogicalBlock& block)
{
	for (const auto& inst : block.Body) {
		m_currentInstruction = inst;
		Select();
	}
}


void X86ISel::DoInstructionSelection()
{
	for (const auto& node : m_ir) {
		if (std::holds_alternative<std::unique_ptr<Function>>(node))
			for (const auto& block : std::get<std::unique_ptr<Function>>(node)->Blocks) DoBlockSelection(block);
	}
}

void X86ISel::Select() { std::visit(InstructionMatcher(), m_currentInstruction); }


}