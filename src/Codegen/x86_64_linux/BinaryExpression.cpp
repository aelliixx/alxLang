#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-09.
//

#include "BlockGenerator.h"

namespace alx {

void BlockGenerator::generate_binary_expression(const ASTNode* node, std::optional<Context> context)
{
	auto expr = static_cast<const BinaryExpression*>(node);
	auto lhs = expr->Lhs();
	auto rhs = expr->Rhs();
	auto op = expr->Operator();
	if (op == TokenType::T_EQ)
	{
		generate_bin_eq(node, context);
		return;
	}

	if (lhs->class_name() == "Identifier")
	{
		auto lhsId = static_cast<Identifier*>(lhs);

		auto lhsSize = m_stack[lhsId->Name()].second;
		auto lhsPtr = m_stack[lhsId->Name()].first;

		if (rhs->class_name() == "Identifier") // a + b
		{
			auto rhsId = static_cast<Identifier*>(rhs);
			assert_ident_initialised(rhsId);
			auto rhsPtr = m_stack[rhsId->Name()].first;
			m_asm << mov(Reg::rax, lhsSize, offset(lhsPtr, lhsSize));

			auto src = reg(Reg::rdx, lhsSize);
			if (expr->OperandsMatch())
				src = reg(Reg::rax, lhsSize);
			else
				m_asm << mov(Reg::rdx, lhsSize, offset(rhsPtr, lhsSize));

			switch (op)
			{
			case TokenType::T_PLUS:
				m_asm << "add " << reg(Reg::rax, lhsSize) << ", " << src << "\n";
				return;
			case TokenType::T_MINUS:
				m_asm << "sub " << reg(Reg::rax, lhsSize) << ", " << src << "\n";
				return;
			case TokenType::T_STAR:
				m_asm << "imul " << reg(Reg::rax, lhsSize) << ", " << src << "\n";
				return;
			default:
				error("Token {} not implemented", token_to_string(op));
				ASSERT_NOT_IMPLEMENTED();
			}
		}
		else if (rhs->class_name() == "NumberLiteral") // a + 5
		{
			auto rhs_num = static_cast<NumberLiteral*>(rhs);
			switch (op)
			{
			case TokenType::T_PLUS:
				m_asm << mov(Reg::rax, lhsSize, offset(lhsPtr, lhsSize));
				m_asm << "add ";
				break;
			case TokenType::T_ADD_EQ:
				m_asm << "add " << offset(lhsPtr, lhsSize) << ", " << rhs_num->Value() << "\n";
				return;
			case TokenType::T_SUB_EQ:
				m_asm << "sub " << offset(lhsPtr, lhsSize) << ", " << rhs_num->Value() << "\n";
				return;
			case TokenType::T_MINUS:
				m_asm << mov(Reg::rax, lhsSize, offset(lhsPtr, lhsSize));
				m_asm << "sub ";
				break;
			case TokenType::T_STAR:
				m_asm << mov(Reg::rax, lhsSize, offset(lhsPtr, lhsSize));
				m_asm << "imul ";
				break;
			case TokenType::T_GT:
			case TokenType::T_LTE:
			case TokenType::T_EQEQ:
			case TokenType::T_NOT_EQ:
				m_asm << mov(Reg::rax, lhsSize, offset(lhsPtr, lhsSize));
				m_asm << "cmp ";
				break;
			case TokenType::T_LT:
			case TokenType::T_GTE:
				m_asm << mov(Reg::rax, lhsSize, offset(lhsPtr, lhsSize));
				m_asm << "cmp " << reg(Reg::rax, lhsSize) << ", " << rhs_num->AsInt() - 1 << "\n";
				return;
			default:
			ASSERT_NOT_IMPLEMENTED();
			}
			m_asm << reg(Reg::rax, lhsSize) << ", " << rhs_num->Value() << "\n";
			return;
		}
		else if (rhs->class_name() == "BinaryExpression")
		{
			ASSERT_NOT_IMPLEMENTED();

		}
	}
	if (lhs->class_name() == "NumberLiteral")
	{
		auto lhsNum = static_cast<NumberLiteral*>(lhs);
		auto lhsSize = size_of(lhsNum->Type());

		if (rhs->class_name() == "Identifier") // 5 + a
		{
			auto rhsId = static_cast<Identifier*>(rhs);
			auto rhsSize = m_stack[rhsId->Name()].second;
			auto rhsPtr = m_stack[rhsId->Name()].first;
			m_asm << mov(Reg::rax, rhsSize, lhsNum->Value());

			switch (op)
			{
			case TokenType::T_PLUS:
				m_asm << "add " << reg(Reg::rax, rhsSize) << ", " << offset(rhsPtr, rhsSize) << "\n";
				return;
			case TokenType::T_MINUS:
				m_asm << "sub " << reg(Reg::rax, rhsSize) << ", " << offset(rhsPtr, rhsSize) << "\n";
				return;
			case TokenType::T_STAR:
				m_asm << "imul " << reg(Reg::rax, rhsSize) << ", " << offset(rhsPtr, rhsSize) << "\n";
				return;
			default:
			ASSERT_NOT_IMPLEMENTED();
			}
		}
		if (rhs->class_name() == "NumberLiteral") // 5 + 2
		{
			MUST(expr->Constexpr() && "Expression with number literals on both sides should be constexpr");
			m_asm << mov(Reg::rax, lhsSize, expr->Evaluate()->Value());
			return;
		}
		if (rhs->class_name() == "BinaryExpression") // 5 + 2 * 10 
		{
			generate_binary_expression(rhs);
			switch (op)
			{
			case TokenType::T_PLUS:
				m_asm << "add " << reg(Reg::rax, lhsSize) << ", " << lhsNum->Value() << "\n";
				return;
			case TokenType::T_MINUS:
				m_asm << "sub " << reg(Reg::rax, lhsSize) << ", " << lhsNum->Value() << "\n";
				return;
			case TokenType::T_STAR:
				m_asm << "imul " << reg(Reg::rax, lhsSize) << ", " << lhsNum->Value() << "\n";
				return;
			default:
			ASSERT_NOT_IMPLEMENTED();
			}
		}
	}
	if (lhs->class_name() == "BinaryExpression")
	{
		generate_binary_expression(lhs, context);
		MUST(context.has_value() && "lhs context is missing");
		auto lhsSize = context.value().LhsSize;
		if (rhs->class_name() == "Identifier")
		{
			auto rhsId = static_cast<Identifier*>(rhs);
			assert_ident_initialised(rhsId);
			auto rhsPtr = m_stack[rhsId->Name()].first;
			switch (op)
			{
			case TokenType::T_PLUS:
				m_asm << "add " << reg(Reg::rax, lhsSize) << ", " << offset(rhsPtr, lhsSize) << "\n";
				return;
			case TokenType::T_MINUS:
				m_asm << "sub " << reg(Reg::rax, lhsSize) << ", " << offset(rhsPtr, lhsSize) << "\n";
				return;
			case TokenType::T_STAR:
				m_asm << "imul " << reg(Reg::rax, lhsSize) << ", " << offset(rhsPtr, lhsSize) << "\n";
				return;
			default:
			ASSERT_NOT_IMPLEMENTED();
			}
		}
		if (rhs->class_name() == "NumberLiteral")
		{
			auto rhsNum = static_cast<NumberLiteral*>(rhs);
			switch (op)
			{
			case TokenType::T_PLUS:
				m_asm << "add " << reg(Reg::rax, lhsSize) << ", " << rhsNum->Value()
					  << "\n"; // FIXME: Figure out the reg size
				return;
			case TokenType::T_MINUS:
				m_asm << "sub " << reg(Reg::rax, lhsSize) << ", " << rhsNum->Value()
					  << "\n"; // FIXME: Figure out the reg size
				return;
			case TokenType::T_STAR:
				m_asm << "imul " << reg(Reg::rax, lhsSize) << ", " << rhsNum->Value()
					  << "\n"; // FIXME: Figure out the reg size
				return;
			default:
			ASSERT_NOT_IMPLEMENTED();
			}
		}
		if (rhs->class_name() == "BinaryExpression")
		{
			generate_binary_expression(rhs, context);
			return;
		}
	}
	ASSERT_NOT_REACHABLE();
}
void BlockGenerator::generate_assign_num_l(size_t lhsSize, const NumberLiteral* rhsId)
{
	m_asm << "mov " << bytes_to_data_size(lhsSize) << " [rbp-" << m_bp_offset << "], " << rhsId->Value() << "\n";
}

void BlockGenerator::generate_assignment_ident(const Identifier& rhsId, size_t lhsSize, bool isUnsigned)
{
	// - Get underlying types, type sizes, whether the value we're trying to assign is unsigned (and therefore
	//	 we should extend the value)
	auto rhsSize = m_stack.at(rhsId.Name()).second;
	auto rhsPtrOffset = m_stack.at(rhsId.Name()).first;

	if (rhsSize <= 2)
		m_asm << mov(Reg::rax, rhsSize, offset(rhsPtrOffset, rhsSize), lhsSize, isUnsigned); // FIXME: Add back signs
	else
		m_asm << mov(Reg::rax, rhsSize, offset(rhsPtrOffset, rhsSize), rhsSize, isUnsigned); // FIXME: Add back signs
	m_asm << mov(offset(m_bp_offset, lhsSize), rhsSize, Reg::rax, lhsSize, isUnsigned);

	// FIXME: Narrowing conversions should emit warnings in parsing stage
}

void BlockGenerator::generate_bin_eq(const ASTNode* node, std::optional<Context> context)
{
	auto expr = static_cast<const BinaryExpression*>(node);
	auto lhs = expr->Lhs();
	auto rhs = expr->Rhs();
	auto op = TokenType::T_EQ;
	if (lhs->class_name() == "Identifier")
	{
		auto lhsId = static_cast<Identifier&>(*lhs);
		auto lhsPtr = m_stack[lhsId.Name()].first;
		auto lhsSize = m_stack[lhsId.Name()].second;

//		add_to_stack(lhsId.Name(), lhsSize, TokenType::T_VOID);

		if (rhs->class_name() == "NumberLiteral")
		{
			auto rhsNum = static_cast<NumberLiteral*>(rhs);
			m_asm << "mov " << bytes_to_data_size(lhsSize) << " [rbp-" << lhsPtr << "], " << rhsNum->Value() << "\n";
			if (context.has_value() && context.value().AssignmentChain)
				m_asm << mov(Reg::rax, lhsSize, offset(lhsPtr, lhsSize));
//			generate_assign_num_l(lhs_size, rhs_num);
			return;
		}
		if (rhs->class_name() == "Identifier")
		{
			auto rhsId = static_cast<Identifier&>(*rhs);
			generate_assignment_ident(rhsId, m_stack[lhsId.Name()].second, false);
			return;
		}
		if (rhs->class_name() == "BinaryExpression")
		{
			auto rhsBin = static_cast<BinaryExpression*>(rhs);
			if (rhsBin->Constexpr())
			{
				m_asm << mov(offset(m_bp_offset, lhsSize), lhsSize, rhsBin->Evaluate()->Value());
				return;
			}
			Context newContext = { .LhsSize = lhsSize };
			generate_binary_expression(rhs, newContext);
			m_asm << mov(offset(m_bp_offset, lhsSize), lhsSize, reg(Reg::rax, lhsSize));
			return;
		}

	}
	if (lhs->class_name() == "BinaryExpression")
	{
		// TODO: Refactor this out
		auto lhsBinExp = static_cast<BinaryExpression*>(lhs);
		if (lhsBinExp->Rhs()->class_name() == "NumberLiteral")
			throw_not_assignable(lhsBinExp->Rhs(), rhs, op);
		size_t lhsSize = 4; // FIXME

		if (rhs->class_name() == "NumberLiteral")
		{
			auto rhsNum = static_cast<NumberLiteral*>(rhs);
			generate_assign_num_l(lhsSize, rhsNum);
			return;
		}
		else if (rhs->class_name() == "Identifier")
		{
			auto rhsId = static_cast<Identifier&>(*rhs);
			generate_assignment_ident(rhsId, 2, false); // FIXME: fix the sign
			return;
		}
		else if (rhs->class_name() == "BinaryExpression")
		{
			Context newContext = { .LhsSize = lhsSize };
			generate_binary_expression(lhs, newContext);
			return;
		}
		ASSERT_NOT_IMPLEMENTED();
	}
	if (lhs->class_name() == "NumberLiteral")
		throw_not_assignable(lhs, rhs, op);
}

}
#pragma clang diagnostic pop