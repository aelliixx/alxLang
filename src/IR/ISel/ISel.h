//
// Created by aelliixx on 2023-11-13.
//

#pragma once
#include <utility>
#include <vector>
#include "../Ir.h"
#include "../../Utils/Utils.h"
namespace alx::ir {

class MachineInstruction
{
public:
	virtual ~MachineInstruction() = default;
	virtual const std::string& Name() = 0;
};

class TwoOpInstruction
{
	refptr<MachineInstruction> m_op1;
	refptr<MachineInstruction> m_op2;

public:
	TwoOpInstruction(refptr<MachineInstruction> op1, refptr<MachineInstruction> op2)
	  : m_op1(std::move(op1)),
		m_op2(std::move(op2))
	{}
};

using MachineInstructions = std::variant<TwoOpInstruction>;
class MachineBlock
{
	std::vector<MachineInstructions> m_nodes;
};

class ISel
{
protected:
	const std::vector<IRNodes>& m_ir;
	std::vector<MachineBlock> m_mir;

public:
	virtual ~ISel() = default;
	explicit ISel(const std::vector<IRNodes>& ir) : m_ir(ir) {}
	virtual void SelectInstructions();
	void Dump();
};
}
