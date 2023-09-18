/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-09.
//

#include "../../Utils/Utils.h"
#include "BlockGenerator.h"
#include "../../libs/Println.h"
#include "../../libs/Error.h"

namespace alx {

void BlockGenerator::generate_variables(const std::unique_ptr<ASTNode>& node)
{
	auto variable = static_cast<VariableDeclaration*>(node.get());
	auto type = variable->TypeAsPrimitive();
	auto size = size_of(type);
	auto value = variable->Value();
	
	if (type == TokenType::T_VOID)
		error("Cannot declare variable of variable 'void'");
	if (!value)
	{
		if (!variable->Ident().Assignable())
			error("Must initialise constant values");
		add_to_stack(variable, 0);
		return;
	}
	if (value->class_name() == "NumberLiteral")
	{
		auto rhs = static_cast<NumberLiteral*>(value);
		add_to_stack(variable); // Increment offset
		m_asm << mov(offset(bp_offset, size), size, rhs->AsInt(), size, is_unsigned(rhs->Type()));
		return;
	}
	else if (value->class_name() == "Identifier")
	{
		// - Get the underlying identifier of the rhs
		auto rhs = static_cast<Identifier*>(value);
		add_to_stack(variable); // Increment offset
		generate_assignment_ident(rhs, type);
		return;
	}
	else if (value->class_name() == "BinaryExpression")
	{
		add_to_stack(variable);
		auto lhs_ptr = m_stack[variable->Name()].second;
		auto rhs = static_cast<BinaryExpression*>(value);
		if (rhs->Constexpr())
		{
			m_asm << mov(offset(bp_offset, size), size, rhs->Evaluate()->AsInt());
			return;
		}
		Context context = { .lhsSize = size };
		generate_binary_expression(value, context);
		m_asm << mov(offset(lhs_ptr, size), size, reg(Reg::rax, size));
		return;
	} else if (value->class_name() == "UnaryExpression")
	{
		add_to_stack(variable);
		generate_unary_expression(variable->Value());
		auto lhs_ptr = m_stack[variable->Name()].second;
		m_asm << mov(offset(lhs_ptr, size), size, reg(Reg::rax, size));
		return;
	}

	ASSERT_NOT_REACHABLE();
}

}