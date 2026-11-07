//
// Created by aelliixx on 2023-11-13.
//

#pragma once
#include "../../ISel.h"


namespace alx::ir::x86_64 {
class X86_64_ISel final : public ISel
{
	void SelectFunction(const Function& func);
	void SelectInstruction(const Instruction inst);

public:
	explicit X86_64_ISel(const std::vector<IRNodes>& nodes) : ISel(nodes) {}
	void SelectInstructions() override;
};
}