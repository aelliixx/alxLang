/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-06.
//

#include <cmath>
#include "Ast.h"
#include "../libs/Println.h"
#include "../libs/Utils.h"

namespace alx {

void ScopeNode::dump_nodes(const std::vector<std::unique_ptr<ASTNode>>& Children, int indent)
{
	for (const auto& child : Children)
	{
		child->PrintNode(indent);
	}
}
void ScopeNode::PrintNode(int indent) const
{
	dump_nodes(m_children, indent);
}

void Program::PrintNode(int indent) const
{
	print(Colour::LightPurple, "{>}Program", indent);
	println(": {");
	dump_nodes(Children(), 2);
	println("}");
}

void Identifier::PrintNode(int indent) const
{
	println("{>}Identifier: {}", indent, m_name);
}
void NumberLiteral::PrintNode(int indent) const
{
	println("{>}NumberLiteral: ", indent);
	switch (m_type)
	{
	case TokenType::T_INT_L:
		println("{>}type: {}", indent + 2, "int");
		println("{>}value: {}", indent + 2, m_value);
		break;
	case TokenType::T_FLOAT_L:
		println("{>}type: {}", indent + 2, "float");
		println("{>}value: {}", indent + 2, m_value);
		break;
	case TokenType::T_DOUBLE_L:
		println("{>}type: {}", indent + 2, "double");
		println("{>}value: {}", indent + 2, m_value);
		break;
	case TokenType::T_CHAR_L:
		println("{>}type: {}", indent + 2, "char");
		println("{>}value: {}", indent + 2, m_value);
		break;
	case TokenType::T_FALSE:
	case TokenType::T_TRUE:
		println("{>}type: {}", indent + 2, "bool");
		println("{>}value: {}", indent + 2, AsBool());
		break;
	default:
		assert(false && "Invalid literal type");
	}
}
void VariableDeclaration::PrintNode(int indent) const
{
	println("{>}VariableDeclaration: {", indent);
	switch (m_type)
	{
	case TokenType::T_INT:
		println("{>}type: {}", indent + 2, "int");
		break;
	case TokenType::T_FLOAT:
		println("{>}type: {}", indent + 2, "float");
		break;
	case TokenType::T_DOUBLE:
		println("{>}type: {}", indent + 2, "double");
		break;
	case TokenType::T_CHAR:
		println("{>}type: {}", indent + 2, "char");
		break;
	case TokenType::T_BOOL:
		println("{>}type: {}", indent + 2, "bool");
		break;
	case TokenType::T_STRING:
		println("{>}type: {}", indent + 2, "string");
		break;
	case TokenType::T_LONG:
		println("{>}type: {}", indent + 2, "long");
		break;
	case TokenType::T_SHORT:
		println("{>}type: {}", indent + 2, "short");
		break;
	default:
		assert(false && "Invalid literal type");
	}
	println("{>}identifier: {}", indent + 2, m_identifier->Name());
	println("{>}value:", indent + 2);
	if (m_value)
		m_value->PrintNode(indent + 4);
	else
		println("{>}null", indent + 4);
	println("{>}}", indent);
}
void BlockStatement::PrintNode(int indent) const
{
	println("{>}BlockStatement: {", indent);
	dump_nodes(Children(), indent + 2);
	println("{>}}", indent);
}

void FunctionDeclaration::PrintNode(int indent) const
{
	println("{>}FunctionDeclaration: {", indent);
	println("{>}name: {}", indent + 2, Name());
	switch (m_return_type)
	{
	case TokenType::T_INT:
		println("{>}return_type: {}", indent + 2, "int");
		break;
	case TokenType::T_FLOAT:
		println("{>}return_type: {}", indent + 2, "float");
		break;
	case TokenType::T_DOUBLE:
		println("{>}return_type: {}", indent + 2, "double");
		break;
	case TokenType::T_CHAR:
		println("{>}return_type: {}", indent + 2, "char");
		break;
	case TokenType::T_BOOL:
		println("{>}return_type: {}", indent + 2, "bool");
		break;
	case TokenType::T_STRING:
		println("{>}return_type: {}", indent + 2, "string");
		break;
	case TokenType::T_LONG:
		println("{>}type: {}", indent + 2, "long");
		break;
	case TokenType::T_SHORT:
		println("{>}type: {}", indent + 2, "short");
		break;
	default:
		assert(false && "Invalid literal type");
	}
	println("{>}body:", indent + 2);
	m_body->PrintNode(indent + 4);
	println("{>}}", indent);
}
void BinaryExpression::PrintNode(int indent) const
{
	println("{>}BinaryExpression: {", indent);
	println("{>}constexpr: {}", indent + 2, m_constexpr);
	println("{>}lhs:", indent + 2);
	m_lhs->PrintNode(indent + 4);
	println("{>}operator: {}", indent + 2, token_to_string(m_binary_op));
	println("{>}rhs:", indent + 2);
	m_rhs->PrintNode(indent + 4);
	println("{>}}", indent);
}

void StringLiteral::PrintNode(int indent) const
{
	println("{>}ReturnStatement: {\n{>}value: {}\n{>}length: ", indent, indent + 2, m_value, indent + 2, Lenght());
	println("{>}}", indent);
}
void ReturnStatement::PrintNode(int indent) const
{
	println("{>}ReturnStatement: {\n{>}argument:", indent, indent + 2);
	m_argument->PrintNode(indent + 4);
	println("{>}}", indent);
}
void Expression::PrintNode(int indent) const
{

}

void IfStatement::PrintNode(int indent) const
{
	println("{>}IfStatement: {", indent);
	println("{>}condition:", indent + 2);
	m_condition->PrintNode(indent + 4);
	println("{>}body:", indent + 2);
	m_body->PrintNode(indent + 4);
	if (HasAlternate())
	{
		println("{>}alternate:", indent + 2);
		m_alternate.value()->PrintNode(indent + 4);
	}
	println("{>}}", indent);

}

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
	  case TokenType::T_INT_DIV:
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
		auto lhs = dynamic_cast<BinaryExpression*>(m_lhs.get())->Evaluate();
		auto rhs = dynamic_cast<NumberLiteral*>(m_rhs.get());
		auto lhs_val = lhs->Value();
		auto rhs_val = rhs->Value();
		return std::make_unique<NumberLiteral>(lhs->Type(), add(lhs->Type(), lhs_val, rhs_val));
	}

	assert(m_rhs->class_name() == "NumberLiteral");
	assert(m_lhs->class_name() == "NumberLiteral");
	auto* lhs = dynamic_cast<NumberLiteral*>(m_lhs.get());
	auto* rhs = dynamic_cast<NumberLiteral*>(m_rhs.get());

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
		m_constexpr = dynamic_cast<BinaryExpression*>(m_lhs.get())->m_constexpr;

	if (m_lhs->class_name() == "Identifier" && m_rhs->class_name() == "Identifier")
	{
		auto lhs_id = dynamic_cast<Identifier*>(m_lhs.get());
		auto rhs_id = dynamic_cast<Identifier*>(m_rhs.get());
		
		m_operands_match = lhs_id->Name() == rhs_id->Name();
	}

}

void UnaryExpression::PrintNode(int indent) const
{
	println("{>}UnaryExpression: {", indent);
	println("{>}operator: {}", indent + 2, token_to_string(m_unary_op));
	println("{>}rhs:", indent + 2);
	m_rhs->PrintNode(indent + 4);
	println("{>}}", indent);
}
void WhileStatement::PrintNode(int indent) const
{
	println("{>}WhileStatement: {", indent);
	println("{>}condition:", indent + 2);
	m_condition->PrintNode(indent + 4);
	println("{>}body:", indent + 2);
	m_body->PrintNode(indent + 4);
	println("{>}}", indent);
}
}