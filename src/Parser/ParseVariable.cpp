/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include "Parser.h"

namespace alx {
std::unique_ptr<VariableDeclaration> Parser::parse_variable()
{
	auto type = consume().type;
	if (is_number_type(type) || type == TokenType::T_STRING)
	{
		auto name = std::make_unique<Identifier>(consume().value.value());
		// Direct assignment
		if (peek().has_value() && peek().value().type == TokenType::T_EQ)
		{
			must_consume(TokenType::T_EQ); // Eat '='
			if (peek().has_value() && is_number_literal(peek().value().type) &&
				peek(1).has_value() && peek(1).value().type == TokenType::T_SEMI)
			{
				auto value = parse_number_literal();
//				must_consume(TokenType::T_SEMI); // Eat ';'
				return std::make_unique<VariableDeclaration>(type, std::move(name), std::move(value));
			}
			else if (peek().has_value() && peek().value().type == TokenType::T_STR_L &&
				peek(1).has_value() && peek(1).value().type == TokenType::T_SEMI)
			{
				auto string = parse_string_literal();
//				must_consume(TokenType::T_SEMI); // Eat ';'
				return std::make_unique<VariableDeclaration>(type, std::move(name), std::move(string));
			}
			else if (peek().has_value() && is_unary_op(peek().value().type)) {
				auto unary_expression = parse_unary_expression();
				return std::make_unique<VariableDeclaration>(type, std::move(name), std::move(unary_expression));
			}
			auto expression = parse_expression();
//			must_consume(TokenType::T_SEMI);
			return std::make_unique<VariableDeclaration>(type, std::move(name), std::move(expression));
		}
		// Declaration
		else if (peek().has_value() && peek().value().type == TokenType::T_SEMI)
		{
//			must_consume(TokenType::T_SEMI);
			return std::make_unique<VariableDeclaration>(type, std::move(name));
		}
	}
	assert(false && "Not reachable");
}
}
