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
	auto assignable = true;
	auto deprecated = false;
	if (peek().has_value() && peek().value().Type == TokenType::T_CONST) {
		assignable = false;
		must_consume(TokenType::T_CONST); // Eat 'const'
	}

	if (peek().has_value() && peek().value().Type == TokenType::T_DEPRECATED) {
		deprecated = true;
		must_consume(TokenType::T_DEPRECATED); // Eat 'const'
	}
	auto typeToken = consume();
	std::variant<TokenType, std::unique_ptr<Identifier>> type;
	if (typeToken.Type == TokenType::T_IDENTIFIER)
		type = std::make_unique<Identifier>(typeToken.Value.value());
	else
		type = typeToken.Type;
	auto identToken = consume();
	auto identifier = std::make_unique<Identifier>(identToken.Value.value(), assignable);
	if (deprecated) {
		identifier->SetDeprecated();
		m_error->Warning(identToken.LineNumber,
						 identToken.ColumnNumber,
						 identToken.PosNumber,
						 "Initialisation of deprecated variable '{}'",
						 identifier->Name());
	}
	auto fullyQualifiedIdentifier = Parser::get_fully_qualified_name(identifier->Name(), m_current_scope_name);
	if (find_variable_by_name(fullyQualifiedIdentifier)) {
		m_error->Error(identToken.LineNumber,
					   identToken.ColumnNumber,
					   identToken.PosNumber,
					   "Redefinition of variable '{}'",
					   identifier->Name());
	}
	// Direct assignment
	if (peek().has_value() && peek().value().Type == TokenType::T_EQ) {
		must_consume(TokenType::T_EQ); // Eat '='
		if (peek().has_value() && isNumberLiteral(peek().value().Type) && peek(1).has_value()
			&& peek(1).value().Type == TokenType::T_SEMI)
		{
			auto value = parse_number_literal();
			auto var = std::make_unique<VariableDeclaration>(
				std::move(type), std::move(identifier), std::move(value), fullyQualifiedIdentifier);
			add_variable(var.get());
			return var;
		}
		else if (peek().has_value() && peek().value().Type == TokenType::T_STR_L && peek(1).has_value()
				 && peek(1).value().Type == TokenType::T_SEMI)
		{
			auto string = parse_string_literal();
			auto var = std::make_unique<VariableDeclaration>(
				std::move(type), std::move(identifier), std::move(string), fullyQualifiedIdentifier);
			add_variable(var.get());
			return var;
		}
		else if (peek().has_value() && isUnaryOp(peek().value().Type)) {
			auto unaryExpression = parse_expression();
			return std::make_unique<VariableDeclaration>(
				std::move(type), std::move(identifier), std::move(unaryExpression), fullyQualifiedIdentifier);
		}
		const Token expressionToken = peek().value();
		TokenType expressionType;
		auto expression = parse_expression();
		size_t expressionSize;
		if (expression->class_name() == "Identifier") {
			auto& ident = static_cast<Identifier&>(*expression);
			//			auto identIt = std::find_if(m_variables.at(m_current_scope_name).begin(),
			//										m_variables.at(m_current_scope_name).end(),
			//										[ident](VariableDeclaration* var) { return var->Name() ==
			// ident.Name();
			//});

			auto identIt = m_variables.find(Parser::get_fully_qualified_name(ident.Name(), m_current_scope_name));
			if (identIt != m_variables.end() && identIt->second->Ident().Deprecated())
				m_error->Warning(expressionToken.LineNumber,
								 expressionToken.ColumnNumber,
								 expressionToken.PosNumber,
								 "Use of deprecated identifier '{}'",
								 ident.Name());
			if ((*identIt).second->TypeIndex() == 0) {
				expressionType = (*identIt).second->TypeAsPrimitive();
				expressionSize = size_of(expressionType);
				if (expressionSize > size_of(typeToken.Type))
					m_error->Warning(typeToken.LineNumber,
									 typeToken.ColumnNumber + 2,
									 typeToken.PosNumber + 2,
									 "Narrowing conversion from type '{}' to '{}'",
									 token_to_string(expressionType),
									 token_to_string(typeToken.Type));
			}
		}
		else if (expression->class_name() == "MemberExpression") {
			auto& memExpr = static_cast<MemberExpression&>(*expression);
			if (memExpr.TypeIndex() == 0) {
				expressionType = memExpr.TypeAsPrimitive();
				expressionSize = size_of(expressionType);
				if (expressionSize > size_of(typeToken.Type))
					m_error->Warning(typeToken.LineNumber,
									 typeToken.ColumnNumber + 2,
									 typeToken.PosNumber + 2,
									 "Narrowing conversion from type '{}' to '{}'",
									 token_to_string(expressionType),
									 token_to_string(typeToken.Type));
			}
		}


		auto var = std::make_unique<VariableDeclaration>(
			std::move(type), std::move(identifier), std::move(expression), fullyQualifiedIdentifier);
		add_variable(var.get());
		return var;
	}
	// Declaration
	else if (peek().has_value() && peek().value().Type == TokenType::T_SEMI) {
		auto var =
			std::make_unique<VariableDeclaration>(std::move(type), std::move(identifier), fullyQualifiedIdentifier);
		add_variable(var.get());
		return var;
	}
	ASSERT_NOT_REACHABLE();
}
}
