/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

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
			auto argType = consume().Type;
			auto argName = consume().Value;
			args.emplace_back(
				std::make_unique<VariableDeclaration>(argType, std::make_unique<Identifier>(argName.value())));
			if (peek().value().Type == TokenType::T_COMMA)
				consume();
		}
		consume(); // Eat ')'
		auto body = std::make_unique<BlockStatement>();
		if (consume().Type == TokenType::T_CURLY_OPEN)
		{
			// Function body
			while (peek().value().Type != TokenType::T_CURLY_CLOSE)
			{
				body->Append(parse_statement());
			}
			consume(); // Eat '}'
		}
		return std::make_unique<FunctionDeclaration>(std::make_unique<Identifier>(name.value()), std::move(body));
	}
	assert(false && "Not reachable");
}
}