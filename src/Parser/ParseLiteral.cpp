/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include "Parser.h"
#include "../Utils/Types.h"
#include "../Utils/Utils.h"
namespace alx {

std::unique_ptr<NumberLiteral> Parser::parse_number_literal()
{
	if (peek().has_value() && peek().value().Type == TokenType::T_INT_L
		|| peek().value().Type == TokenType::T_FLOAT_L || peek().value().Type == TokenType::T_DOUBLE_L ||
		peek().value().Type == TokenType::T_TRUE || peek().value().Type == TokenType::T_FALSE)
	{
		auto valueToken = consume();
		return std::make_unique<NumberLiteral>(valueToken.Type, valueToken.Value.value());
	}
	return nullptr;
}

std::unique_ptr<StringLiteral> Parser::parse_string_literal()
{
	ASSERT_NOT_IMPLEMENTED();
	if (peek().has_value() && peek().value().Type == TokenType::T_STR_L)
	{
		
	return std::unique_ptr<StringLiteral>();
	}
	return nullptr;
}


}