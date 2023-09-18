/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-12.
//

#include "BlockGenerator.h"

namespace alx {

void BlockGenerator::generate_unary_expression(const ASTNode* node)
{
	auto expression = static_cast<const UnaryExpression*>(node);
	auto op = expression->Operator();
	auto rhs = expression->Rhs();

	if (rhs->class_name() == "NumberLiteral")
	{
		auto num = static_cast<NumberLiteral*>(rhs);
		std::string value = std::to_string(!num->AsInt());
		m_asm << mov(Reg::rax, size_of(num->Type()), value);
		return;
	}
	else if (rhs->class_name() == "Identifier")
	{
		auto ident = static_cast<Identifier*>(rhs);
		auto rhs_ptr = m_stack[ident->Name()].second;
		auto rhs_size = size_of(m_stack[ident->Name()].first->TypeAsPrimitive());

		m_asm << "cmp " << bytes_to_data_size(rhs_size) << " " << offset(rhs_ptr, rhs_size) << ", 0\n";
		m_asm << "sete " << reg(Reg::rax, 1) << "\n";
		m_asm << mov(Reg::rax, 1, reg(Reg::rax, 1), rhs_size, true);
		return;
	}
	else if (rhs->class_name() == "BinaryExpression")
	{
		auto bin_expr = static_cast<BinaryExpression*>(rhs);
		if (bin_expr->Constexpr())
		{
			auto eval = bin_expr->Evaluate();
			std::string value = std::to_string(!eval->AsInt());
			m_asm << mov(Reg::rax, size_of(eval->Type()), value);
			return;
		}
		generate_binary_expression(bin_expr, {});
		// FIXME: Figure out the word size
		m_asm << "test " << reg(Reg::rax) << ", " << reg(Reg::rax) << "\n";
		m_asm << "sete " << reg(Reg::rax, 1) << "\n";
		m_asm << mov(Reg::rax, 1, reg(Reg::rax, 1), true);
		return;
	}
	else if (rhs->class_name() == "StringLiteral")
	{
		ASSERT_NOT_IMPLEMENTED();

	}
}

}