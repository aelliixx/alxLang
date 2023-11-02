/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include "../Utils/Utils.h"
#include "../libs/ErrorHandler.h"
#include "Parser.h"

namespace alx {
std::unique_ptr<ASTNode> Parser::parse_statement()
{
	auto token = peek();
	if (!token.has_value())
		return nullptr;
	switch (token.value().Type) {
	case TokenType::T_CONST:
		if (peek(1).has_value() && isNumberType(peek(1).value().Type)) {
			if (peek(2).has_value() && peek(2).value().Type == TokenType::T_IDENTIFIER)
				return parse_variable();
			m_error->FatalError(token.value().LineNumber,
								token.value().ColumnNumber,
								token.value().PosNumber,
								"Expected identifier after type declaration");
		}
		m_error->FatalError(token.value().LineNumber,
							token.value().ColumnNumber,
							token.value().PosNumber,
							"Expected type after 'const' keyword");
		return nullptr;
	case TokenType::T_INT:
		[[fallthrough]];
	case TokenType::T_LONG:
		[[fallthrough]];
	case TokenType::T_SHORT:
		[[fallthrough]];
	case TokenType::T_FLOAT:
		[[fallthrough]];
	case TokenType::T_DOUBLE:
		[[fallthrough]];
	case TokenType::T_STRING:
		[[fallthrough]];
	case TokenType::T_CHAR:
		[[fallthrough]];
	case TokenType::T_BOOL:
		if (peek(1).has_value() && peek(1).value().Type == TokenType::T_IDENTIFIER) {
			if (peek(2).has_value() && peek(2).value().Type == TokenType::T_OPEN_PAREN)
				return parse_function();
			else if (peek(2).has_value() && peek(2).value().Type == TokenType::T_EQ
					 || peek(2).value().Type == TokenType::T_SEMI)
				return parse_variable();
		}
		else
			m_error->FatalError(token.value().LineNumber,
								token.value().ColumnNumber,
								token.value().PosNumber,
								"Expected identifier after type declaration");
		return nullptr;
	case TokenType::T_VOID:
		return parse_function();
	case TokenType::T_RET:
		return parse_return_statement();
	case TokenType::T_IF:
		return parse_if_statement();
	case TokenType::T_ELSE:
		m_error->Error(token.value().LineNumber,
					   token.value().ColumnNumber,
					   token.value().PosNumber,
					   "An 'else' statement must succeed an 'if' statement");
		return nullptr; // TODO: verify that this is appropriate
	case TokenType::T_FOR:
		ASSERT_NOT_IMPLEMENTED();
	case TokenType::T_WHILE:
		return parse_while_statement();
	case TokenType::T_STRUCT:
		return parse_struct_declaration();
	case TokenType::T_IDENTIFIER:
		if (peek(1).value().Type == TokenType::T_IDENTIFIER)
			return parse_variable();
		[[fallthrough]];
	default:
		return parse_expression();
	}
}

}
