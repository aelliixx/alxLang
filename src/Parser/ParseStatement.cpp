/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include "Parser.h"
#include "../libs/Utils.h"

namespace alx {
std::unique_ptr<ASTNode> Parser::parse_statement()
{
	auto token = peek();
	if (!token.has_value())
		return nullptr;
	switch (token.value().Type)
	{
	case TokenType::T_INT:
	case TokenType::T_LONG:
	case TokenType::T_SHORT:
	case TokenType::T_FLOAT:
	case TokenType::T_DOUBLE:
	case TokenType::T_STRING:
	case TokenType::T_CHAR:
	case TokenType::T_BOOL:
		if (peek(1).has_value() && peek(1).value().Type == TokenType::T_IDENTIFIER)
		{
			if (peek(2).has_value() && peek(2).value().Type == TokenType::T_OPEN_PAREN)
				return parse_function();
			else if (peek(2).has_value() && peek(2).value().Type == TokenType::T_EQ ||
				peek(2).value().Type == TokenType::T_SEMI)
				return parse_variable();
		}
		else
			error("Expected identifier after type declaration");
	case TokenType::T_VOID:
		return parse_function();
	case TokenType::T_RET:
		return parse_return_statement();
	case TokenType::T_IF:
		return parse_if_statement();
	case TokenType::T_ELSE:
		error("An 'else' statement must succeed an 'if' statement");
	case TokenType::T_FOR:
	ASSERT_NOT_IMPLEMENTED();
	case TokenType::T_WHILE:
	ASSERT_NOT_IMPLEMENTED();
	default:
		return parse_expression();
	}
}

}