/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include <algorithm>
#include "Parser.h"
namespace alx {

std::unique_ptr<FunctionDeclaration> Parser::parse_function()
{
	auto returnType = consume().Type;
	if (returnType == TokenType::T_INT || returnType == TokenType::T_FLOAT || returnType == TokenType::T_DOUBLE ||
		returnType == TokenType::T_VOID || returnType == TokenType::T_STRING || returnType == TokenType::T_CHAR ||
		returnType == TokenType::T_BOOL)
	{
		auto name = consume().Value;
		if (consume().Type != TokenType::T_OPEN_PAREN)
			error("Expected '(' in function {} declaration", name.value());
		std::vector<std::unique_ptr<VariableDeclaration>> args{};
		while (peek().has_value() && peek().value().Type != TokenType::T_CLOSE_PAREN)
		{
			// Arguments
			auto argTypeToken = consume();
			auto argType = argTypeToken.Type;
			auto argNameToken = consume();

			if (!is_number_type(argType) && argType != TokenType::T_STRING) // FIXME: Allow class identifiers
				error("Unexpected token '{}' in variable declaration, at line: {}, position: {}", token_to_string(argType), argTypeToken.LineNumber, argTypeToken.ColumnNumber);
			if (argNameToken.Type != TokenType::T_IDENTIFIER)
				error("Unexpected token '{}' in variable declaration, at line: {}, position: {}", token_to_string(argNameToken.Type), argNameToken.LineNumber, argNameToken.ColumnNumber);

			auto argName = argNameToken.Value;

			if (peek().has_value() && peek().value().Type == TokenType::T_EQ) // Default arguments
			{
				must_consume(TokenType::T_EQ);
				if (peek().has_value() && is_number_literal(peek().value().Type))
					args.emplace_back(std::make_unique<VariableDeclaration>(argType,
																			std::make_unique<Identifier>(argName.value()),
																			std::move(parse_number_literal())));
				else if (peek().has_value() && peek().value().Type == TokenType::T_STR_L)
					args.emplace_back(std::make_unique<VariableDeclaration>(argType,
																			std::make_unique<Identifier>(argName.value()),
																			std::move(parse_string_literal())));
				if (peek().value().Type == TokenType::T_COMMA)
					consume();
				continue;
			}
			// It is definitely not a default argument, therefore no default arguments could have preceded it.
			if (std::find_if(args.begin(), args.end(), [](const std::unique_ptr<VariableDeclaration>& arg)
			{ return arg->Value(); }) != args.end())
				error("Missing default argument on {}", argName.value());

			args.emplace_back(
				std::make_unique<VariableDeclaration>(argType, std::make_unique<Identifier>(argName.value())));
			if (peek().value().Type == TokenType::T_COMMA)
				consume();
		}
		must_consume(TokenType::T_CLOSE_PAREN); // Eat ')'
		auto body = std::make_unique<BlockStatement>();
		if (consume().Type == TokenType::T_CURLY_OPEN)
		{
			// Function body
			while (peek().value().Type != TokenType::T_CURLY_CLOSE)
			{
				auto statement = parse_statement();
				consume_semicolon(statement);
				body->Append(std::move(statement));
			}
			must_consume(TokenType::T_CURLY_CLOSE); // Eat '}'
		}
		return std::make_unique<FunctionDeclaration>(returnType,
													 std::make_unique<Identifier>(name.value()),
													 std::move(body),
													 std::move(args));
	}
	assert(false && "Not reachable");
}
}