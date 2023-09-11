/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-09.
//

#include "../../libs/Utils.h"
#include "Generator.h"
#include "../../libs/Println.h"

namespace alx {

void Generator::generate_variables(const std::unique_ptr<ASTNode>& node)
{
	auto lhs = reinterpret_cast<VariableDeclaration*>(node.get());
	auto lhs_type = lhs->Type();
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
		m_asm << mov(offset(bp), m_type_size[lhs_type], rhs->Value());
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
			m_asm << mov(offset(bp), m_type_size[lhs_type], rhs->Evaluate()->Value());
			return;
		}
		Context context = { .lhs_size = m_type_size[lhs_type] };
		generate_binary_expression(lhs->Value(), context);
		m_asm << mov(offset(lhs_ptr), m_type_size[lhs_type], reg(Reg::rax, m_type_size[lhs_type]));
		return;
	}

	ASSERT_NOT_REACHABLE();
}

}