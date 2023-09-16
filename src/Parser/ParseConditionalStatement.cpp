/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-12.
//

#include "Parser.h"

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
	auto curly_open = peek();
	if (!curly_open.has_value())
		error("Expected statement, at line {}, position {}",
			  peek(-1).value().lineNumber,
			  peek(-1).value().columnNumber);
	if (curly_open.value().type == TokenType::T_CURLY_OPEN)
	{
		must_consume(TokenType::T_CURLY_OPEN);
		while (peek().value().type != TokenType::T_CURLY_CLOSE)
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
	if (peek().has_value() && peek().value().type == TokenType::T_ELSE)
	{
		must_consume(TokenType::T_ELSE);
		if (peek().has_value() && peek().value().type == TokenType::T_IF)
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
	if (must_consume(TokenType::T_CURLY_OPEN).type == TokenType::T_CURLY_OPEN)
	{
		while (peek().value().type != TokenType::T_CURLY_CLOSE)
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
		error("Expected statement, at line {}, position {}",
			  peek(-1).value().lineNumber,
			  peek(-1).value().columnNumber);
	if (curly_open.value().type == TokenType::T_CURLY_OPEN)
	{
		must_consume(TokenType::T_CURLY_OPEN);
		while (peek().value().type != TokenType::T_CURLY_CLOSE)
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