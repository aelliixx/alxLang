/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include "Parser.h"
#include "../AST/Types.h"
namespace alx {

std::unique_ptr<NumberLiteral> Parser::parse_number_literal()
{
	if (peek().has_value() && peek().value().Type == TokenType::T_INT_L
		|| peek().value().Type == TokenType::T_FLOAT_L || peek().value().Type == TokenType::T_DOUBLE_L)
	{
		auto valueToken = consume();
		std::variant<int, float, double, char, bool> value;
		switch (valueToken.Type)
		{
		case TokenType::T_INT_L:
			value = std::stoi(valueToken.Value.value());
			break;
		case TokenType::T_FLOAT_L:
			value = std::stof(valueToken.Value.value());
			break;
		case TokenType::T_DOUBLE_L:
			value = std::stod(valueToken.Value.value());
			break;
		case TokenType::T_CHAR_L:
			value = valueToken.Value.value()[0];
			break;
		case TokenType::T_TRUE:
			value = true;
			break;
		case TokenType::T_FALSE:
			value = false;
			break;
		default:
			assert(false && "Not reachable");
		}
		return std::make_unique<NumberLiteral>(valueToken.Type, value);
	}
	return nullptr;
}

std::unique_ptr<StringLiteral> Parser::parse_string_literal()
{
	if (peek().has_value() && peek().value().Type == TokenType::T_STR_L)
	{
		
	return std::unique_ptr<StringLiteral>();
	}
	return nullptr;
}


}