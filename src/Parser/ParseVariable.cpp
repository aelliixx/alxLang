/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include "Parser.h"
#include "../libs/ErrorHandler.h"

namespace alx {
std::unique_ptr<VariableDeclaration> Parser::parse_variable()
{
	auto typeToken = consume();
	std::variant<TokenType, std::unique_ptr<Identifier>> type;
	if (typeToken.Type == TokenType::T_IDENTIFIER)
		type = std::make_unique<Identifier>(typeToken.Value.value());
	else
		type = typeToken.Type;
	auto identToken = consume();
	auto identifier = std::make_unique<Identifier>(identToken.Value.value());
	if (std::find_if(m_variables.at(m_current_scope_name).begin(),
					 m_variables.at(m_current_scope_name).end(),
					 [&identifier](const VariableDeclaration* var)
					 {
					   return identifier->Name() == var->Name();
					 })
		!= m_variables.at(m_current_scope_name).end())
	{
		m_error->Error(
			identToken.LineNumber,
			identToken.ColumnNumber,
			identToken.PosNumber,
			"Redefinition of variable '{}'",
			identifier->Name());
	}
	// Direct assignment
	if (peek().has_value() && peek().value().Type == TokenType::T_EQ)
	{
		must_consume(TokenType::T_EQ); // Eat '='
		if (peek().has_value() && isNumberLiteral(peek().value().Type) &&
			peek(1).has_value() && peek(1).value().Type == TokenType::T_SEMI)
		{
			auto value = parse_number_literal();
			auto var = std::make_unique<VariableDeclaration>(std::move(type), std::move(identifier), std::move(value));
			add_variable(var.get());
			return std::move(var);
		}
		else if (peek().has_value() && peek().value().Type == TokenType::T_STR_L &&
			peek(1).has_value() && peek(1).value().Type == TokenType::T_SEMI)
		{
			auto string = parse_string_literal();
			auto var = std::make_unique<VariableDeclaration>(std::move(type), std::move(identifier), std::move(string));
			add_variable(var.get());
			return std::move(var);
		}
		else if (peek().has_value() && isUnaryOp(peek().value().Type))
		{
			auto unaryExpression = parse_expression();
			return std::make_unique<VariableDeclaration>(std::move(type),
														 std::move(identifier),
														 std::move(unaryExpression));
		}
		const Token expressionToken = peek().value();
		TokenType expressionType;
		auto expression = parse_expression();
		size_t expressionSize;
		if (expression->class_name() == "Identifier")
		{
			auto& ident = static_cast<Identifier&>(*expression);
			auto identIt = std::find_if(m_variables.at(m_current_scope_name).begin(),
										m_variables.at(m_current_scope_name).end(),
										[ident](VariableDeclaration* var)
										{
										  return var->Name() == ident.Name();
										});
			if ((*identIt)->TypeIndex() == 0)
			{
				expressionType = (*identIt)->TypeAsPrimitive();
				expressionSize = size_of(expressionType);
				if (expressionSize > size_of(typeToken.Type))
					m_error->Warning(expressionToken.LineNumber,
									 expressionToken.ColumnNumber + 2,
									 expressionToken.PosNumber + 2,
									 "Narrowing conversion from type '{}' to '{}'",
									 token_to_string(expressionType),
									 token_to_string(typeToken.Type));
			}
		}
		else if (expression->class_name() == "MemberExpression")
		{
			auto& memExpr = static_cast<MemberExpression&>(*expression);
			if (memExpr.TypeIndex() == 0)
			{
				expressionType = memExpr.TypeAsPrimitive();
				expressionSize = size_of(expressionType);
				if (expressionSize > size_of(typeToken.Type))
					m_error->Warning(expressionToken.LineNumber,
									 expressionToken.ColumnNumber + 2,
									 expressionToken.PosNumber + 2,
									 "Narrowing conversion from type '{}' to '{}'",
									 token_to_string(expressionType),
									 token_to_string(typeToken.Type));
			}
		}

		

		auto var = std::make_unique<VariableDeclaration>(std::move(type), std::move(identifier), std::move(expression));
		add_variable(var.get());
		return std::move(var);
	}
		// Declaration
	else if (peek().has_value() && peek().value().Type == TokenType::T_SEMI)
	{
		auto var = std::make_unique<VariableDeclaration>(std::move(type), std::move(identifier));
		add_variable(var.get());
		return std::move(var);
	}
	ASSERT_NOT_REACHABLE();
}
}
