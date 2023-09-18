/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include "Parser.h"
#include "../libs/Error.h"

namespace alx {
std::unique_ptr<VariableDeclaration> Parser::parse_variable()
{
	auto type_token = consume();
	std::variant<TokenType, std::unique_ptr<Identifier>> type;
	if (type_token.type == TokenType::T_IDENTIFIER)
		type = std::make_unique<Identifier>(type_token.value.value());
	else
		type = type_token.type;
	auto identifier = std::make_unique<Identifier>(consume().value.value());
	
	if (std::find_if(m_variables.at(m_current_scope_name).begin(),
				  m_variables.at(m_current_scope_name).end(),
				  [&identifier](const VariableDeclaration* var){
					return identifier->Name() == var->Name();
	})
		!= m_variables.at(m_current_scope_name).end())
	{
		error("Redefinition of variable '{}', at line: {}, position: {}", identifier->Name(), type_token.lineNumber, type_token.columnNumber);
	}
	// Direct assignment
	if (peek().has_value() && peek().value().type == TokenType::T_EQ)
	{
		must_consume(TokenType::T_EQ); // Eat '='
		if (peek().has_value() && is_number_literal(peek().value().type) &&
			peek(1).has_value() && peek(1).value().type == TokenType::T_SEMI)
		{
			auto value = parse_number_literal();
			auto var = std::make_unique<VariableDeclaration>(std::move(type), std::move(identifier), std::move(value));
			add_variable(var.get());
			return std::move(var);
		}
		else if (peek().has_value() && peek().value().type == TokenType::T_STR_L &&
			peek(1).has_value() && peek(1).value().type == TokenType::T_SEMI)
		{
			auto string = parse_string_literal();
			auto var = std::make_unique<VariableDeclaration>(std::move(type), std::move(identifier), std::move(string));
			add_variable(var.get());
			return std::move(var);
		}
		else if (peek().has_value() && is_unary_op(peek().value().type))
		{
			auto unary_expression = parse_unary_expression();
			return std::make_unique<VariableDeclaration>(std::move(type), std::move(identifier), std::move(unary_expression));
		}
		auto expression = parse_expression();
		auto var = std::make_unique<VariableDeclaration>(std::move(type), std::move(identifier), std::move(expression));
		add_variable(var.get());
		return std::move(var);
	}
		// Declaration
	else if (peek().has_value() && peek().value().type == TokenType::T_SEMI)
	{
		auto var = std::make_unique<VariableDeclaration>(std::move(type), std::move(identifier));
		add_variable(var.get());
		return std::move(var);
	}
	assert(false && "Not reachable");
}
}
