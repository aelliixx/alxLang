/*
 * Copyright (c) 2023 Donatas Mockus.
 */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-06.
//

#include "Parser.h"

namespace alx {

Parser::Parser(std::vector<Token> tokens)
	: m_tokens(std::move(tokens))
{
	m_binaryOpPrecedence[TokenType::T_LT] = 10;
	m_binaryOpPrecedence[TokenType::T_GT] = 10;
	m_binaryOpPrecedence[TokenType::T_PLUS] = 20;
	m_binaryOpPrecedence[TokenType::T_MINUS] = 20;
	m_binaryOpPrecedence[TokenType::T_STAR] = 30;
	m_binaryOpPrecedence[TokenType::T_FWD_SLASH] = 30;

	m_program = std::make_unique<Program>();
}

int Parser::m_get_binary_op_precedence(const Token& token)
{
	return m_binaryOpPrecedence.find(token.Type)->second;
}

std::unique_ptr<Program> Parser::Parse()
{
	while (peek().has_value())
	{
		auto type = peek().value().Type;
		m_program->Append(parse_statement());
	}
	return std::move(m_program);
}

std::optional<Token> Parser::peek(int ahead)
{
	if (m_index + ahead < m_tokens.size())
		return m_tokens.at(m_index + ahead);
	return {};
}
Token Parser::consume()
{
	auto token = m_tokens.at(m_index);
	++m_index;
	return token;
}

std::optional<Token> Parser::try_consume(TokenType type)
{
	if (peek().has_value() && peek().value().Type == type)
		return consume();
	return {};
}

std::unique_ptr<Expression> Parser::parse_expression()
{
	auto lhs = parse_term();
	if (!lhs)
		return nullptr;
	return lhs;
	// TODO: Binary expression
}

std::unique_ptr<Expression> Parser::parse_term()
{
	auto token = peek();
	if (!token.has_value())
		return nullptr;
	switch (token.value().Type)
	{
	case TokenType::T_INT_L:
	case TokenType::T_FLOAT_L:
	case TokenType::T_DOUBLE_L:
	case TokenType::T_CHAR_L:
	case TokenType::T_TRUE:
	case TokenType::T_FALSE:
		return parse_number_literal();
	case TokenType::T_STR_L:
		return parse_string_literal();
	case TokenType::T_IDENTIFIER:
		if (auto identifier = try_consume(TokenType::T_IDENTIFIER))
			return std::make_unique<Identifier>(identifier->Value.value());
	default:
		error("Unexpected token '{}'", token_to_string(token->Type));
	}
	exit(EXIT_FAILURE);
}
Token Parser::must_consume(TokenType token)
{
	if (auto tok = try_consume(token)) {
		return tok.value();
	}
	error("Expected token '{}'", token_to_string(token)); // TODO: Add line context
	exit(EXIT_FAILURE);
}

}