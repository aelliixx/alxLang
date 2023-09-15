/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-09.
//

#include "../../libs/Utils.h"
#include "BlockGenerator.h"
#include "../../libs/Println.h"

namespace alx {

void BlockGenerator::generate_variables(const std::unique_ptr<ASTNode>& node)
{
	auto lhs = dynamic_cast<VariableDeclaration*>(node.get());
	auto lhs_type = lhs->Type();
	if (lhs_type == TokenType::T_VOID)
		error("Cannot declare variable of type 'void'");
	if (!lhs->Value())
	{
		if (!lhs->Ident().Assignable())
			error("Must initialise constant values");
		add_to_stack(lhs, 0);
		return;
	}
	if (lhs->Value()->class_name() == "NumberLiteral")
	{
		auto rhs = dynamic_cast<NumberLiteral*>(lhs->Value());
		add_to_stack(lhs); // Increment bp
		m_asm << mov(offset(bp, size_of(lhs_type)), size_of(lhs_type), rhs->AsInt(), size_of(lhs_type), is_unsigned(rhs->Type()));
		return;
	}
	else if (lhs->Value()->class_name() == "Identifier")
	{
		// - Get the underlying identifier of the rhs
		auto rhs = dynamic_cast<Identifier*>(lhs->Value());
		add_to_stack(lhs); // Increment bp
		generate_assignment_ident(rhs, lhs_type);
		return;
	}
	else if (lhs->Value()->class_name() == "BinaryExpression")
	{
		add_to_stack(lhs);
		auto lhs_ptr = m_stack[lhs->Name()].second;
		auto rhs = dynamic_cast<BinaryExpression*>(lhs->Value());
		if (rhs->Constexpr())
		{
			m_asm << mov(offset(bp, size_of(lhs_type)), size_of(lhs_type), rhs->Evaluate()->AsInt());
			return;
		}
		Context context = { .lhsSize = size_of(lhs_type) };
		generate_binary_expression(lhs->Value(), context);
		m_asm << mov(offset(lhs_ptr, size_of(lhs_type)), size_of(lhs_type), reg(Reg::rax, size_of(lhs_type)));
		return;
	} else if (lhs->Value()->class_name() == "UnaryExpression")
	{
		add_to_stack(lhs);
		generate_unary_expression(lhs->Value());
		auto lhs_ptr = m_stack[lhs->Name()].second;
		m_asm << mov(offset(lhs_ptr, size_of(lhs_type)), size_of(lhs_type), reg(Reg::rax, size_of(lhs_type)));
		return;
	}

	ASSERT_NOT_REACHABLE();
}

}