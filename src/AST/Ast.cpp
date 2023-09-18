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



#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
std::unique_ptr<NumberLiteral> BinaryExpression::Evaluate() const
{
	assert(m_constexpr && "Cannot evaluate non constant binary expressions");
	auto add = [this](TokenType lhs, const std::string& lhs_val, const std::string& rhs_val)
	{
	  // FIXME: Add evaluation for doubles and floats
	  std::string res;
	  switch (m_binary_op)
	  {
	  case TokenType::T_PLUS:
		  res = std::to_string(std::stol(lhs_val) + std::stol(rhs_val));
		  break;
	  case TokenType::T_MINUS:
		  res = std::to_string(std::stol(lhs_val) - std::stol(rhs_val));
		  break;
	  case TokenType::T_STAR:
		  res = std::to_string(std::stol(lhs_val) * std::stol(rhs_val));
		  break;
	  case TokenType::T_FWD_SLASH:
		  res = std::to_string(std::stol(lhs_val) / std::stol(rhs_val));
		  break;
	  case TokenType::T_POW:
		  res = std::to_string((long)(std::pow(std::stol(lhs_val), std::stol(rhs_val))));
		  break;
	  case TokenType::T_LT:
		  res = std::to_string(std::stol(lhs_val) < std::stol(rhs_val));
		  break;
	  case TokenType::T_GT:
		  res = std::to_string(std::stol(lhs_val) > std::stol(rhs_val));
		  break;
	  case TokenType::T_LTE:
		  res = std::to_string(std::stol(lhs_val) <= std::stol(rhs_val));
		  break;
	  case TokenType::T_GTE:
		  res = std::to_string(std::stol(lhs_val) >= std::stol(rhs_val));
		  break;
	  case TokenType::T_MOD:
		  res = std::to_string(std::stol(lhs_val) % std::stol(rhs_val));
		  break;
	  case TokenType::T_COLON:
		  res = std::to_string(std::stol(lhs_val) / std::stol(rhs_val));
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
	  return res;
	};

	BinaryExpression* binaryExpression;

	if (m_lhs->class_name() == "BinaryExpression")
	{
		auto lhs = static_cast<BinaryExpression*>(m_lhs.get())->Evaluate();
		auto rhs = static_cast<NumberLiteral*>(m_rhs.get());
		auto lhs_val = lhs->Value();
		auto rhs_val = rhs->Value();
		return std::make_unique<NumberLiteral>(lhs->Type(), add(lhs->Type(), lhs_val, rhs_val));
	}

	assert(m_rhs->class_name() == "NumberLiteral");
	assert(m_lhs->class_name() == "NumberLiteral");
	auto* lhs = static_cast<NumberLiteral*>(m_lhs.get());
	auto* rhs = static_cast<NumberLiteral*>(m_rhs.get());

	auto lhs_val = lhs->Value();
	auto rhs_val = rhs->Value();

	return std::make_unique<NumberLiteral>(lhs->Type(), add(lhs->Type(), lhs_val, rhs_val));
}
#pragma clang diagnostic pop

BinaryExpression::BinaryExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, TokenType binaryOp)
	: m_lhs(std::move(lhs)),
	  m_rhs(std::move(rhs)),
	  m_binary_op(binaryOp)
{
	assert(is_binary_op(m_binary_op) && "Invalid binary operator");

	// Check if both sides are numbers or binary expressions and if they are constant
	if (m_lhs->class_name() == "NumberLiteral" && m_rhs->class_name() == "NumberLiteral")
		m_constexpr = true;
	else if (m_lhs->class_name() == "BinaryExpression" && m_rhs->class_name() == "NumberLiteral")
		m_constexpr = static_cast<BinaryExpression*>(m_lhs.get())->m_constexpr;

	if (m_lhs->class_name() == "Identifier" && m_rhs->class_name() == "Identifier")
	{
		auto lhs_id = static_cast<Identifier*>(m_lhs.get());
		auto rhs_id = static_cast<Identifier*>(m_rhs.get());
		
		m_operands_match = lhs_id->Name() == rhs_id->Name();
	}

}

}