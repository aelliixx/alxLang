/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include <algorithm>
#include "Parser.h"
#include "../libs/ErrorHandler.h"
namespace alx {

std::unique_ptr<FunctionDeclaration> Parser::parse_function()
{
	auto returnType = consume().Type;
	m_current_return_type = returnType;
	if (returnType == TokenType::T_INT || returnType == TokenType::T_FLOAT || returnType == TokenType::T_DOUBLE ||
		returnType == TokenType::T_VOID || returnType == TokenType::T_STRING || returnType == TokenType::T_CHAR ||
		returnType == TokenType::T_BOOL)
	{
		auto name = consume().Value;
		m_variables[name.value()] = {};
		m_current_scope_name = name.value();
		auto paren = consume();
		if (paren.Type != TokenType::T_OPEN_PAREN)
			m_error->Error(paren.LineNumber,
						   paren.ColumnNumber,
						   paren.PosNumber,
						   "Expected '(' in function '{}' declaration", name.value());
		std::vector<std::unique_ptr<VariableDeclaration>> args{};
		while (peek().has_value() && peek().value().Type != TokenType::T_CLOSE_PAREN)
		{
			// Arguments
			auto argTypeToken = consume();
			auto argType = argTypeToken.Type;
			auto argNameToken = consume();

			if (!isNumberType(argType)
				&& argType != TokenType::T_STRING
				&& argType != TokenType::T_IDENTIFIER) // FIXME: Allow class identifiers
				m_error->Error(argTypeToken.LineNumber,
							   argTypeToken.ColumnNumber,
							   argTypeToken.PosNumber,
							   "Unexpected token '{}' in variable declaration",
							   token_to_string(argType));
			if (argNameToken.Type != TokenType::T_IDENTIFIER)
				m_error->Error(argTypeToken.LineNumber,
							   argTypeToken.ColumnNumber,
							   argTypeToken.PosNumber,
							   "Unexpected token '{}' in variable declaration",
							   token_to_string(argType));

			auto argName = argNameToken.Value;

			if (peek().has_value() && peek().value().Type == TokenType::T_EQ) // Default arguments
			{
				must_consume(TokenType::T_EQ);
				if (peek().has_value() && isNumberLiteral(peek().value().Type))
					args.emplace_back(
						std::make_unique<VariableDeclaration>(argType,
															  std::make_unique<Identifier>(
																  argName.value()),
															  std::move(parse_number_literal())));
				else if (peek().has_value() && peek().value().Type == TokenType::T_STR_L)
					args.emplace_back(
						std::make_unique<VariableDeclaration>(argType,
															  std::make_unique<Identifier>(
																  argName.value()),
															  std::move(parse_string_literal())));
				if (peek().value().Type == TokenType::T_COMMA)
					consume();
				continue;
			}
			// It is definitely not a default argument, therefore no default arguments could have preceded it.
			if (std::find_if(args.begin(), args.end(), [](const std::unique_ptr<VariableDeclaration>& arg)
			{ return arg->Value(); }) != args.end())
				m_error->Error(argNameToken.LineNumber, argNameToken.ColumnNumber,
							   argNameToken.PosNumber,
							   "Missing default argument on {}", argName.value());

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