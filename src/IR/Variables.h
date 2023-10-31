/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-29.
//

#pragma once

#include "Instructions.h"

namespace alx::ir {
class IR;
struct Variable
{
	std::string Name;
	VisibilityAttribute Visibility = VisibilityAttribute::Local;
	std::vector<ParameterAttributes> Attributes;
	IdentifierInstruction Allocation;
	bool IsTemporary = false;
	void PrintNode(IR& ir) const;
	// Size in bytes
	[[nodiscard]] size_t Size() const
	{
		struct ValueVisitor
		{
			size_t operator()(const Constant& constant) const {
				return constant.Size();
			}
			size_t operator()(const std::shared_ptr<Variable>& var) const
			{
				return var->Size();
			}
		};
		struct SizeVisitor
		{
			size_t operator()(const AllocaInst& inst) const
			{
				return inst.Size();
			}
			size_t operator()(const LoadInst& inst) const
			{
				return inst.Alignment.Alignment;
			}
			size_t operator()(const AddInst& inst) const
			{
				return std::visit(ValueVisitor{}, inst.Lhs);
			}
			size_t operator()(const SubInst& inst) const
			{
				return std::visit(ValueVisitor{}, inst.Lhs);
			}
			size_t operator()(const MulInst& inst) const
			{
				return std::visit(ValueVisitor{}, inst.Lhs);
			}
			size_t operator()(const SDivInst& inst) const
			{
				return std::visit(ValueVisitor{}, inst.Lhs);
			}
		} visitor;

		return std::visit(visitor, Allocation);
	}
};
}