/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-09.
//

#include "Generator.h"
#include "../../libs/Println.h"
#include "../../libs/Utils.h"

namespace alx {

void Generator::generate_binary_expression(const std::unique_ptr<ASTNode>& node)
{
	auto expr = reinterpret_cast<BinaryExpression*>(node.get());
	auto lhs = expr->LHS();
	auto rhs = expr->RHS();
	auto op = expr->Operator();

	switch (op)
	{
	case TokenType::T_EQ:
	{
		if (lhs->class_name() == "Identifier")
		{
			auto lhs_id = dynamic_cast<Identifier*>(lhs);
			// Check if the identifier has been declared on the stack
			if (m_stack.find(lhs_id->Name()) == m_stack.end())
				error("Use of undeclared identifier '{}'", lhs_id->Name());
			
			auto rhs_ptr_offset = m_stack.at(lhs_id->Name()).second;
			auto lhs_type = m_stack.at(lhs_id->Name()).first->Type();
			auto lhs_size = m_type_size[lhs_type];
			
			if (rhs->class_name() == "NumberLiteral") {
				auto rhs_id = dynamic_cast<NumberLiteral*>(rhs);
				m_asm << "mov " << bytes_to_data_size(lhs_size) << " [rbp-" << bp << "], " << rhs_id->Value() << "\n";
			}
			if (rhs->class_name() == "Identifier") {
				auto rhs_id = dynamic_cast<Identifier*>(rhs);
				
				m_asm << "mov " << bytes_to_data_size(lhs_size) << " [rbp-" << bp << "], " << rhs_id->Value() << "\n";

			}
			
//			m_asm << "mov " << bytes_to_data_size(m_type_size[lhs_type]) << " [rbp-" << bp << "], " << rhs->Value() << "\n";
			
		}
		return;
	}
	case TokenType::T_PLUS:
	case TokenType::T_MINUS:
	case TokenType::T_STAR:
	case TokenType::T_FWD_SLASH:
	case TokenType::T_POW:
	case TokenType::T_LT:
	case TokenType::T_GT:
	case TokenType::T_LTE:
	case TokenType::T_GTE:
	case TokenType::T_MOD:
	case TokenType::T_INT_DIV:
	case TokenType::T_EQEQ:
	case TokenType::T_SUB:
	case TokenType::T_ADD:
	case TokenType::T_ADD_EQ:
	case TokenType::T_SUB_EQ:
	case TokenType::T_MULT_EQ:
	case TokenType::T_DIV_EQ:
	case TokenType::T_MOD_EQ:
	case TokenType::T_POW_EQ:
		ASSERT_NOT_IMPLEMENTED();
	default:
		ASSERT_NOT_REACHABLE();
	}
}

}