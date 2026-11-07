//
// Created by aelliixx on 2023-11-13.
//

#pragma once
#include "IR/ISel/ISel.h"

namespace alx::ir::x86_64 {

class MOV final : public MachineInstruction, public TwoOpInstruction
{
	size_t m_size;

public:
	MOV(const size_t size, const refptr<MachineInstruction>& op1, const refptr<MachineInstruction>& op2)
	  : TwoOpInstruction(op1, op2),
		m_size(size)
	{}
	const std::string& Name() override { return getFormatted("MOV{}", m_size); }
};

}