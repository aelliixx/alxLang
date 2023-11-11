/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-11-06.
//

#include "InstructionSelector.h"
#include "X86/X86Info.h"

namespace alx::ir {


void InstructionSelector::SelectInstructions() {
	for (auto& node : m_ir) {
		if (m_target == Target::X86_64)
		{
			X86InstructionInfo instructionInfo;
			if (std::holds_alternative<std::unique_ptr<Function>>(node)) {
				auto& function = std::get<std::unique_ptr<Function>>(node);
				for (auto& block : function->Blocks)
					for (auto& inst : block.Body)
					{
						instructionInfo.GetInstruction(inst);
					}
			}
		}
		
	}
	
	
}
}