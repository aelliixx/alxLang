#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-09.
//

#include "BlockGenerator.h"
#include "../../libs/Println.h"
#include "../../libs/Utils.h"

namespace alx {

void BlockGenerator::generate_binary_expression(const ASTNode* node, std::optional<Context> context)
{
	auto expr = dynamic_cast<const BinaryExpression*>(node);
	auto lhs = expr->Lhs();
	auto rhs = expr->Rhs();
	auto op = expr->Operator();
	if (op == TokenType::T_EQ)
	{
		generate_bin_eq(node);
		return;
	}

	if (lhs->class_name() == "Identifier")
	{
		auto lhs_id = dynamic_cast<Identifier*>(lhs);
		assert_ident_initialised(lhs_id);

		auto lhs_type = m_stack.at(lhs_id->Name()).first->Type();
		auto lhs_size = size_of(lhs_type);
		auto lhs_ptr = m_stack[lhs_id->Name()].second;

		if (rhs->class_name() == "Identifier") // a + b
		{
			auto rhs_id = dynamic_cast<Identifier*>(rhs);
			assert_ident_initialised(rhs_id);
			auto rhs_ptr = m_stack[rhs_id->Name()].second;
			m_asm << mov(Reg::rax, lhs_size, offset(lhs_ptr, lhs_size));

			auto src = reg(Reg::rdx, lhs_size);
			if (expr->OperandsMatch())
				src = reg(Reg::rax, lhs_size);
			else
				m_asm << mov(Reg::rdx, lhs_size, offset(rhs_ptr, lhs_size));

			switch (op)
			{
			case TokenType::T_PLUS:
				m_asm << "add " << reg(Reg::rax, lhs_size) << ", " << src << "\n";
				return;
			case TokenType::T_MINUS:
				m_asm << "sub " << reg(Reg::rax, lhs_size) << ", " << src << "\n";
				return;
			case TokenType::T_STAR:
				m_asm << "imul " << reg(Reg::rax, lhs_size) << ", " << src << "\n";
				return;
			default:
			ASSERT_NOT_IMPLEMENTED();
			}
		}
		else if (rhs->class_name() == "NumberLiteral") // a + 5
		{
			auto rhs_num = dynamic_cast<NumberLiteral*>(rhs);
			switch (op)
			{
			case TokenType::T_PLUS:
				m_asm << mov(reg(Reg::rax, lhs_size), lhs_size, offset(lhs_ptr, lhs_size));
				m_asm << "add ";
				break;
			case TokenType::T_ADD_EQ:
				m_asm << "add " << offset(lhs_ptr, lhs_size) << ", " << rhs_num->Value() << "\n";
				return;
			case TokenType::T_SUB_EQ:
				m_asm << "sub " << offset(lhs_ptr, lhs_size) << ", " << rhs_num->Value() << "\n";
				return;
			case TokenType::T_MINUS:
				m_asm << mov(reg(Reg::rax, lhs_size), lhs_size, offset(lhs_ptr, lhs_size));
				m_asm << "sub ";
				break;
			case TokenType::T_STAR:
				m_asm << mov(reg(Reg::rax, lhs_size), lhs_size, offset(lhs_ptr, lhs_size));
				m_asm << "imul ";
				break;
			case TokenType::T_GT:
			case TokenType::T_LTE:
			case TokenType::T_EQEQ:
			case TokenType::T_NOT_EQ:
				m_asm << mov(reg(Reg::rax, lhs_size), lhs_size, offset(lhs_ptr, lhs_size));
				m_asm << "cmp ";
				break;
			case TokenType::T_LT:
			case TokenType::T_GTE:
				m_asm << mov(reg(Reg::rax, lhs_size), lhs_size, offset(lhs_ptr, lhs_size));
				m_asm << "cmp " << reg(Reg::rax, lhs_size) << ", " << rhs_num->AsInt() - 1 << "\n";
				return;
			default:
			ASSERT_NOT_IMPLEMENTED();
			}
			m_asm << reg(Reg::rax, lhs_size) << ", " << rhs_num->Value() << "\n";
			return;
		}
		else if (rhs->class_name() == "BinaryExpression")
		{
			ASSERT_NOT_IMPLEMENTED();

		}
	}
	if (lhs->class_name() == "NumberLiteral")
	{
		auto lhs_num = dynamic_cast<NumberLiteral*>(lhs);
		auto lhs_size = size_of(lhs_num->Type());

		if (rhs->class_name() == "Identifier") // 5 + a
		{
			auto rhs_id = dynamic_cast<Identifier*>(rhs);
			auto rhs_size = size_of(m_stack[rhs_id->Name()].first->Type());
			assert_ident_initialised(rhs_id);
			auto rhs_ptr = m_stack[rhs_id->Name()].second;
			m_asm << mov(Reg::rax, rhs_size, lhs_num->Value());

			switch (op)
			{
			case TokenType::T_PLUS:
				m_asm << "add " << reg(Reg::rax, rhs_size) << ", " << offset(rhs_ptr, rhs_size) << "\n";
				return;
			case TokenType::T_MINUS:
				m_asm << "sub " << reg(Reg::rax, rhs_size) << ", " << offset(rhs_ptr, rhs_size) << "\n";
				return;
			case TokenType::T_STAR:
				m_asm << "imul " << reg(Reg::rax, rhs_size) << ", " << offset(rhs_ptr, rhs_size) << "\n";
				return;
			default:
			ASSERT_NOT_IMPLEMENTED();
			}
		}
		if (rhs->class_name() == "NumberLiteral") // 5 + 2
		{
			assert(expr->Constexpr() && "Expression with number literals on both sides should be constexpr");
			m_asm << mov(reg(Reg::rax), lhs_size, expr->Evaluate()->Value());
			return;
		}
		if (rhs->class_name() == "BinaryExpression") // 5 + 2 * 10 
		{
			generate_binary_expression(rhs);
			switch (op)
			{
			case TokenType::T_PLUS:
				m_asm << "add " << reg(Reg::rax, lhs_size) << ", " << lhs_num->Value() << "\n";
				return;
			case TokenType::T_MINUS:
				m_asm << "sub " << reg(Reg::rax, lhs_size) << ", " << lhs_num->Value() << "\n";
				return;
			case TokenType::T_STAR:
				m_asm << "imul " << reg(Reg::rax, lhs_size) << ", " << lhs_num->Value() << "\n";
				return;
			default:
			ASSERT_NOT_IMPLEMENTED();
			}
		}
	}
	if (lhs->class_name() == "BinaryExpression")
	{
		generate_binary_expression(lhs, context);
		assert(context.has_value() && "lhs context is missing");
		auto lhs_size = context.value().lhsSize;
		if (rhs->class_name() == "Identifier")
		{
			auto rhs_id = dynamic_cast<Identifier*>(rhs);
			assert_ident_initialised(rhs_id);
			auto rhs_ptr = m_stack[rhs_id->Name()].second;
			switch (op)
			{
			case TokenType::T_PLUS:
				m_asm << "add " << reg(Reg::rax, lhs_size) << ", " << offset(rhs_ptr, lhs_size) << "\n";
				return;
			case TokenType::T_MINUS:
				m_asm << "sub " << reg(Reg::rax, lhs_size) << ", " << offset(rhs_ptr, lhs_size) << "\n";
				return;
			case TokenType::T_STAR:
				m_asm << "imul " << reg(Reg::rax, lhs_size) << ", " << offset(rhs_ptr, lhs_size) << "\n";
				return;
			default:
			ASSERT_NOT_IMPLEMENTED();
			}
		}
		if (rhs->class_name() == "NumberLiteral")
		{
			auto rhs_num = dynamic_cast<NumberLiteral*>(rhs);
			switch (op)
			{
			case TokenType::T_PLUS:
				m_asm << "add " << reg(Reg::rax, lhs_size) << ", " << rhs_num->Value()
					  << "\n"; // FIXME: Figure out the reg size
				return;
			case TokenType::T_MINUS:
				m_asm << "sub " << reg(Reg::rax, lhs_size) << ", " << rhs_num->Value()
					  << "\n"; // FIXME: Figure out the reg size
				return;
			case TokenType::T_STAR:
				m_asm << "imul " << reg(Reg::rax, lhs_size) << ", " << rhs_num->Value()
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
void BlockGenerator::generate_assign_num_l(size_t lhs_size, const NumberLiteral* rhs_id)
{
	m_asm << "mov " << bytes_to_data_size(lhs_size) << " [rbp-" << bp << "], " << rhs_id->Value() << "\n";
}

void BlockGenerator::generate_assignment_ident(const Identifier* rhs_id,
											   TokenType lhs_type)
{
	// - Check if the rhs has been initialised
	if (!m_stack.at(rhs_id->Name()).first->Value())
		error("Cannot assign an uninitialised variable");
	// - Get underlying types, type sizes, whether the value we're trying to assign is unsigned (and therefore
	//	 we should extend the value)
	auto rhs_type = m_stack.at(rhs_id->Name()).first->Type();
	auto rhs_size = size_of(rhs_type);
	auto rhs_unsigned = is_unsigned(rhs_type);
	auto rhs_ptr_offset = m_stack.at(rhs_id->Name()).second;
	auto lhs_size = size_of(lhs_type);

	m_asm << mov(Reg::rax, rhs_size, offset(rhs_ptr_offset, lhs_size), lhs_size, rhs_unsigned);
	m_asm << mov(offset(bp, lhs_size), lhs_size, Reg::rax);

	if (size_of(lhs_type) < size_of(rhs_type))
		warning("Narrowing conversion from {} to {}", token_to_string(rhs_type), token_to_string(lhs_type));
}

void BlockGenerator::generate_bin_eq(const ASTNode* node)
{
	auto expr = dynamic_cast<const BinaryExpression*>(node);
	auto lhs = expr->Lhs();
	auto rhs = expr->Rhs();
	auto op = TokenType::T_EQ;

	if (lhs->class_name() == "Identifier")
	{
		auto lhs_id = dynamic_cast<Identifier*>(lhs);
		// Check if the identifier has been declared on the stack
		assert_ident_declared(lhs_id);

		auto lhs_type = m_stack.at(lhs_id->Name()).first->Type();
		auto lhs_size = size_of(lhs_type);
		add_to_stack(lhs_id->Name(), rhs);

		if (rhs->class_name() == "NumberLiteral")
		{
			auto rhs_num = dynamic_cast<NumberLiteral*>(rhs);
			generate_assign_num_l(lhs_size, rhs_num);
			return;
		}
		if (rhs->class_name() == "Identifier")
		{
			auto rhs_id = dynamic_cast<Identifier*>(rhs);
			generate_assignment_ident(rhs_id, lhs_type);
			return;
		}
		if (rhs->class_name() == "BinaryExpression")
		{
			auto rhs_bin = dynamic_cast<BinaryExpression*>(rhs);
			if (rhs_bin->Constexpr())
			{
				m_asm << mov(offset(bp, lhs_size), lhs_size, rhs_bin->Evaluate()->Value());
				return;
			}
			Context context{ .lhsSize = lhs_size };
			generate_binary_expression(rhs, context);
			m_asm << mov(offset(bp, lhs_size), lhs_size, reg(Reg::rax, lhs_size));
			return;
		}

	}
	if (lhs->class_name() == "BinaryExpression")
	{
		// TODO: Refactor this out
		auto lhs_bin_exp = dynamic_cast<BinaryExpression*>(lhs);
		if (lhs_bin_exp->Rhs()->class_name() == "NumberLiteral")
			throw_not_assignable(lhs_bin_exp->Rhs(), rhs, op);
		auto lhs_id = m_stack[dynamic_cast<Identifier*>(lhs_bin_exp->Rhs())->Name()].first;
		auto lhs_type = lhs_id->Type();
		auto lhs_size = size_of(lhs_type);

		if (rhs->class_name() == "NumberLiteral")
		{
			auto rhs_num = dynamic_cast<NumberLiteral*>(rhs);
			generate_assign_num_l(lhs_size, rhs_num);
		}
		else if (rhs->class_name() == "Identifier")
		{
			auto rhs_id = dynamic_cast<Identifier*>(rhs);
			generate_assignment_ident(rhs_id, lhs_type);
		}
		else if (rhs->class_name() == "BinaryExpression")
		{
			Context context = { .lhsSize = lhs_size };
			generate_binary_expression(lhs, context);
		}
		ASSERT_NOT_IMPLEMENTED();
	}
	if (lhs->class_name() == "NumberLiteral")
		throw_not_assignable(lhs, rhs, op);
}

}
#pragma clang diagnostic pop