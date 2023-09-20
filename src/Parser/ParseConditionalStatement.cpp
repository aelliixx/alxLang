/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-12.
//

#include "Parser.h"
#include "../libs/ErrorHandler.h"

namespace alx {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
std::unique_ptr<IfStatement> Parser::parse_if_statement()
{
	std::unique_ptr<Expression> condition;

	must_consume(TokenType::T_IF);
	must_consume(TokenType::T_OPEN_PAREN);
	condition = parse_expression();
	must_consume(TokenType::T_CLOSE_PAREN);
	auto body = std::make_unique<BlockStatement>();
	auto curlyOpen = peek();
	if (!curlyOpen.has_value())
		m_error->Error(peek(-1).value().LineNumber,
					   peek(-1).value().ColumnNumber,
					   peek(-1).value().PosNumber, "Expected statement");
	if (curlyOpen.value().Type == TokenType::T_CURLY_OPEN)
	{
		must_consume(TokenType::T_CURLY_OPEN);
		while (peek().value().Type != TokenType::T_CURLY_CLOSE)
		{
			auto statement = parse_statement();
			consume_semicolon(statement);
			body->Append(std::move(statement));
		}
		must_consume(TokenType::T_CURLY_CLOSE);
	}
	else
	{
		auto statement = parse_statement();
		consume_semicolon(statement);
		body->Append(std::move(statement));
	}
	auto statement = std::make_unique<IfStatement>(std::move(condition), std::move(body));
	if (peek().has_value() && peek().value().Type == TokenType::T_ELSE)
	{
		must_consume(TokenType::T_ELSE);
		if (peek().has_value() && peek().value().Type == TokenType::T_IF)
		{
			statement->SetAlternate(parse_if_statement());
			return statement;
		}
		statement->SetAlternate(parse_else_statement());
	}
	return statement;
}
std::unique_ptr<BlockStatement> Parser::parse_else_statement()
{
	auto body = std::make_unique<BlockStatement>();
	if (must_consume(TokenType::T_CURLY_OPEN).Type == TokenType::T_CURLY_OPEN)
	{
		while (peek().value().Type != TokenType::T_CURLY_CLOSE)
		{
			auto statement = parse_statement();
			consume_semicolon(statement);
			body->Append(std::move(statement));
		}
		must_consume(TokenType::T_CURLY_CLOSE);
	}
	return body;
}

#pragma clang diagnostic pop

std::unique_ptr<WhileStatement> Parser::parse_while_statement()
{
	must_consume(TokenType::T_WHILE);
	must_consume(TokenType::T_OPEN_PAREN);
	std::unique_ptr<Expression> condition = parse_expression();
	must_consume(TokenType::T_CLOSE_PAREN);
	auto body = std::make_unique<BlockStatement>();
	auto curly_open = peek();
	if (!curly_open.has_value())
		m_error->Error(peek(-1).value().LineNumber,
					   peek(-1).value().ColumnNumber,
					   peek(-1).value().PosNumber,
					   "Expected statement, at line {}, position {}");
	if (curly_open.value().Type == TokenType::T_CURLY_OPEN)
	{
		must_consume(TokenType::T_CURLY_OPEN);
		while (peek().value().Type != TokenType::T_CURLY_CLOSE)
		{
			auto statement = parse_statement();
			consume_semicolon(statement);
			body->Append(std::move(statement));
		}
		must_consume(TokenType::T_CURLY_CLOSE);
	}
	else
	{
		auto statement = parse_statement();
		consume_semicolon(statement);
		body->Append(std::move(statement));
	}
	return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

}