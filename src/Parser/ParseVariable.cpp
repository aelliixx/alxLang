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
	auto type = consume().Type;
	if (type == TokenType::T_INT || type == TokenType::T_FLOAT || type == TokenType::T_DOUBLE
		|| type == TokenType::T_STRING || type == TokenType::T_CHAR || type == TokenType::T_BOOL)
	{
		auto name = std::make_unique<Identifier>(consume().Value.value());
		// Direct assignment
		if (peek().has_value() && peek().value().Type == TokenType::T_EQ)
		{
			consume(); // Eat '='
			// NumberLiteral assigment
			if (auto value = parse_number_literal())
			{
				if (!is_literal_assignable(type, value->Type()))
					error("Cannot assign {} to {}", token_to_string(value->Type()), token_to_string(type));
				consume(); // Eat ';'
				return std::make_unique<VariableDeclaration>(type, std::move(name), std::move(value));
			}
			// String literal assignment
			else if (auto string = parse_string_literal())
			{
				if (!is_literal_assignable(type, value->Type()))
					error("Cannot assign {} to {}", token_to_string(value->Type()), token_to_string(type));
				consume(); // Eat ';'
				return std::make_unique<VariableDeclaration>(type, std::move(name), std::move(string));
			}
		} // Declaration
		else {}
	}
	assert(false && "Not reachable");
}
}
