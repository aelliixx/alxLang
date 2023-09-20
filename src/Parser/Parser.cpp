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

#include <utility>
#include "../Utils/Utils.h"
#include "../libs/ErrorHandler.h"

namespace alx {

Parser::Parser(std::vector<Token> tokens, const std::shared_ptr<ErrorHandler>& errorHandler)
	: m_tokens(std::move(tokens)),
	  m_error(errorHandler)
{
	m_binary_op_precedence[TokenType::T_LT] = 10;
	m_binary_op_precedence[TokenType::T_GT] = 10;
	m_binary_op_precedence[TokenType::T_PLUS] = 20;
	m_binary_op_precedence[TokenType::T_MINUS] = 20;
	m_binary_op_precedence[TokenType::T_STAR] = 30;
	m_binary_op_precedence[TokenType::T_FWD_SLASH] = 30;

	m_program = std::make_unique<Program>();
}

int Parser::get_binary_op_precedence(const Token& token)
{
	return m_binary_op_precedence.find(token.Type)->second;
}

std::unique_ptr<Program> Parser::Parse()
{
	try
	{
		while (peek().has_value())
		{
			auto type = peek().value().Type;
			m_program->Append(parse_statement());
		}
	}
	catch (std::runtime_error& err)
	{
		println(Colour::LightRed, "Something went wrong when building AST. Current AST:");
		m_program->PrintNode(0);
		m_error->EmitErrorCount();
		exit(1);
	}
	return std::move(m_program);
}

std::unique_ptr<Expression> Parser::parse_expression()
{
	auto lhs = parse_term();
	if (!lhs)
		return nullptr;

	auto bin = parse_binary_operation(std::move(lhs), 0);
	return bin;
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
		if (peek().has_value() && peek().value().Type == TokenType::T_IDENTIFIER)
		{
			if (peek(1).has_value()
				&& (peek(1).value().Type == TokenType::T_DOT || peek(1).value().Type == TokenType::T_ARROW
					|| peek(1).value().Type == TokenType::T_COLON_COLON))
				return parse_member_expression();
			auto identifier = must_consume(TokenType::T_IDENTIFIER);

			if (std::find_if(m_variables[m_current_scope_name].begin(),
							 m_variables[m_current_scope_name].end(),
							 [identifier](VariableDeclaration* var)
							 {
							   return var->Name() == identifier.Value.value();
							 }) == m_variables[m_current_scope_name].end())
				m_error->Error(identifier.LineNumber, identifier.ColumnNumber, identifier.PosNumber,
							   "Use of undeclared identifier '{}'", identifier.Value.value());
			
			return std::make_unique<Identifier>(identifier.Value.value());
		}
	case TokenType::T_OPEN_PAREN:
	{
		consume();
		auto expr = parse_expression();
		must_consume(TokenType::T_CLOSE_PAREN);
		return expr;
	}
	case TokenType::T_NOT:
		return parse_unary_expression();
	default:
		m_error->Error(token->LineNumber,
					   token->ColumnNumber,
					   token->PosNumber,
					   "Unexpected token '{}'",
					   token_to_string(token->Type));
	}
	ASSERT_NOT_REACHABLE();
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

Token Parser::must_consume(TokenType token)
{
	if (auto tok = try_consume(token))
	{
		return tok.value();
	}
	m_error->Error(peek(-1).value().LineNumber,
				   peek(-1).value().ColumnNumber,
				   peek(-1).value().PosNumber,
				   "Expected token '{}' after '{}'",
				   token_to_string(token),
				   token_to_string(peek(-1).value().Type));
	exit(EXIT_FAILURE);
}
void Parser::consume_semicolon(const std::unique_ptr<ASTNode>& statement)
{
	if (statement->class_name() != "IfStatement" && statement->class_name() != "WhileStatement"
		&& statement->class_name() != "StructDeclaration")
		must_consume(TokenType::T_SEMI);
}
void Parser::add_variable(VariableDeclaration* var)
{
	m_variables.at(m_current_scope_name).push_back(var);
}

}