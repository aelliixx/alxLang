/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-06.
//

#include <cmath>
#include "Ast.h"
#include "../libs/Println.h"
#include "../Utils/Utils.h"

namespace alx {

#ifdef __clang__
#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
#endif
std::unique_ptr<NumberLiteral> BinaryExpression::Evaluate() const
{
	MUST(m_constexpr && "Cannot evaluate non constant binary expressions");
	auto add = [this](const std::string& lhsVal, const std::string& rhsVal) {
		// FIXME: Add evaluation for doubles and floats
		std::string res;
		switch (m_binary_op) {
		case TokenType::T_PLUS:
			res = std::to_string(std::stol(lhsVal) + std::stol(rhsVal));
			break;
		case TokenType::T_MINUS:
			res = std::to_string(std::stol(lhsVal) - std::stol(rhsVal));
			break;
		case TokenType::T_STAR:
			res = std::to_string(std::stol(lhsVal) * std::stol(rhsVal));
			break;
		case TokenType::T_FWD_SLASH:
			res = std::to_string(std::stol(lhsVal) / std::stol(rhsVal));
			break;
		case TokenType::T_POW:
			res = std::to_string((long)(std::pow(std::stol(lhsVal), std::stol(rhsVal))));
			break;
		case TokenType::T_LT:
			res = std::to_string(std::stol(lhsVal) < std::stol(rhsVal));
			break;
		case TokenType::T_GT:
			res = std::to_string(std::stol(lhsVal) > std::stol(rhsVal));
			break;
		case TokenType::T_LTE:
			res = std::to_string(std::stol(lhsVal) <= std::stol(rhsVal));
			break;
		case TokenType::T_GTE:
			res = std::to_string(std::stol(lhsVal) >= std::stol(rhsVal));
			break;
		case TokenType::T_MOD:
			res = std::to_string(std::stol(lhsVal) % std::stol(rhsVal));
			break;
		case TokenType::T_COLON:
			res = std::to_string(std::stol(lhsVal) / std::stol(rhsVal));
			break;
		case TokenType::T_EQEQ:
			[[fallthrough]];
		case TokenType::T_SUB:
			[[fallthrough]];
		case TokenType::T_ADD:
			[[fallthrough]];
		case TokenType::T_ADD_EQ:
			[[fallthrough]];
		case TokenType::T_SUB_EQ:
			[[fallthrough]];
		case TokenType::T_MULT_EQ:
			[[fallthrough]];
		case TokenType::T_DIV_EQ:
			[[fallthrough]];
		case TokenType::T_MOD_EQ:
			[[fallthrough]];
		case TokenType::T_POW_EQ:
			ASSERT_NOT_IMPLEMENTED();
		default:
			ASSERT_NOT_REACHABLE();
		}
		return res;
	};

	if (m_lhs->class_name() == "BinaryExpression") {
		auto lhs = static_cast<BinaryExpression*>(m_lhs.get())->Evaluate();
		auto rhs = static_cast<NumberLiteral*>(m_rhs.get());
		auto lhsVal = lhs->Value();
		auto rhsVal = rhs->Value();
		return std::make_unique<NumberLiteral>(lhs->Type(), add(lhsVal, rhsVal));
	}

	MUST(m_rhs->class_name() == "NumberLiteral");
	MUST(m_lhs->class_name() == "NumberLiteral");
	auto* lhs = static_cast<NumberLiteral*>(m_lhs.get());
	auto* rhs = static_cast<NumberLiteral*>(m_rhs.get());

	auto lhs_val = lhs->Value();
	auto rhs_val = rhs->Value();

	return std::make_unique<NumberLiteral>(lhs->Type(), add(lhs_val, rhs_val));
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif

BinaryExpression::BinaryExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, TokenType binaryOp)
  : m_lhs(std::move(lhs)),
	m_rhs(std::move(rhs)),
	m_binary_op(binaryOp)
{
	MUST(isBinaryOp(m_binary_op) && "Invalid binary operator");

	// Check if both sides are numbers or binary expressions and if they are constant
	if (m_lhs->class_name() == "NumberLiteral" && m_rhs->class_name() == "NumberLiteral")
		m_constexpr = true;
	else if (m_lhs->class_name() == "BinaryExpression" && m_rhs->class_name() == "NumberLiteral")
		m_constexpr = static_cast<BinaryExpression*>(m_lhs.get())->m_constexpr;

	if (m_lhs->class_name() == "Identifier" && m_rhs->class_name() == "Identifier") {
		auto lhsId = static_cast<Identifier*>(m_lhs.get());
		auto rhsId = static_cast<Identifier*>(m_rhs.get());

		m_operands_match = lhsId->Name() == rhsId->Name();
	}
}

StructDeclaration::StructDeclaration(std::string name,
									 std::vector<std::unique_ptr<ASTNode>> members,
									 std::vector<std::unique_ptr<ASTNode>> methods)
  : m_name(std::move(name)),
	m_members(std::move(members)),
	m_methods(std::move(methods))
{
	for (const auto& member : m_members) {
		auto& var = static_cast<VariableDeclaration&>(*member);
		if (var.TypeIndex() == 0) {
			m_size += size_of(var.TypeAsPrimitive());
		}
	}
}

}