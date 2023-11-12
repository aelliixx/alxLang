//
// Created by aelliixx on 2023-11-11.
//

#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>


namespace alx::ir {


class MachineInstruction
{
public:
	virtual ~MachineInstruction() = default;
	virtual std::string Name() = 0;
};

class TwoOperandMI : MachineInstruction
{
	std::shared_ptr<MachineInstruction> m_op1{};
	std::shared_ptr<MachineInstruction> m_op2{};

protected:
	TwoOperandMI(MachineInstruction* op1, MachineInstruction* op2) : m_op1(op1), m_op2(op2) {}

public:
	[[nodiscard]] const std::shared_ptr<MachineInstruction>& GetOperand1() const { return m_op1; }
	[[nodiscard]] const std::shared_ptr<MachineInstruction>& GetOperand2() const { return m_op2; }
	std::string Name() override = 0;
};

class BasicMachineBlock
{
public:
	std::string Label{};
	std::vector<MachineInstruction> Nodes{};
};

class MachineFunction
{
public:
	std::string Name{};
	std::vector<BasicMachineBlock> Blocks{};
};

using MIR = std::vector<std::variant<MachineFunction>>;


} // namespace alx::ir
