/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-06.
//

#include "Ast.h"
#include "../libs/Println.h"

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
		println("{>}value: {}", indent + 2, std::get<int>(m_value));
		break;
	case TokenType::T_FLOAT_L:
		println("{>}type: {}", indent + 2, "float");
		println("{>}value: {}", indent + 2, std::get<float>(m_value));
		break;
	case TokenType::T_DOUBLE_L:
		println("{>}type: {}", indent + 2, "double");
		println("{>}value: {}", indent + 2, std::get<double>(m_value));
		break;
	case TokenType::T_CHAR_L:
		println("{>}type: {}", indent + 2, "char");
		println("{>}value: {}", indent + 2, std::get<char>(m_value));
		break;
	case TokenType::T_FALSE:
	case TokenType::T_TRUE:
		println("{>}type: {}", indent + 2, "bool");
		println("{>}value: {}", indent + 2, std::get<bool>(m_value));
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
	default:
		assert(false && "Invalid literal type");
	}
	m_identifier->PrintNode(indent + 2);
	println("{>}value:", indent + 2);
	m_value->PrintNode(indent + 4);
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
	println("{>}FunctionDeclaration: {\n{>}name: {}\n{>}body:", indent, indent + 2, m_identifier->Name(), indent + 2);
	m_body->PrintNode(indent + 4);
	println("{>}}", indent);
}
void BinaryExpression::PrintNode(int indent) const
{
	println("{>}BinaryExpression:", indent);
	println("{>}lhs:", indent + 2);
	m_lhs->PrintNode(indent + 2);
	print("{>}operator: ", indent);
	switch (m_binaryOp)
	{

	case TokenType::T_PLUS:
		println("+");
		break;
	case TokenType::T_MINUS:
		println("-");
		break;
	case TokenType::T_STAR:
		println("*");
		break;
	case TokenType::T_FWD_SLASH:
		println("/");
		break;
	case TokenType::T_POW:
		println("^");
		break;
	case TokenType::T_INT_DIV:
		println("\\");
		break;
	case TokenType::T_MOD:
		println("%");
		break;
	case TokenType::T_GT:
		println(">");
		break;
	case TokenType::T_LT:
		println("<");
		break;
	case TokenType::T_GTE:
		println(">=");
		break;
	case TokenType::T_LTE:
		println("<=");
		break;
	case TokenType::T_EQ:
		println("==");
		break;
	default:
		assert(false && "Invalid binary operator");
	}
	println("{>}rhs:", indent + 2);
	m_rhs->PrintNode(indent + 2);
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
}