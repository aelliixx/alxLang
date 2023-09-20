/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-16.
//


#include "Parser.h"

namespace alx {

std::unique_ptr<StructDeclaration> Parser::parse_struct_declaration()
{
	auto oldScopeName = m_current_scope_name;
	consume(); // Consumes 'struct' or 'class'
	// TODO: Parse inheritance
	auto name = must_consume(TokenType::T_IDENTIFIER);
	must_consume(TokenType::T_CURLY_OPEN);
	std::vector<std::unique_ptr<ASTNode>> members;
	std::vector<std::unique_ptr<ASTNode>> methods;
	m_current_scope_name = name.Value.value();
	m_variables[m_current_scope_name] = {};
	while (peek().has_value() && peek().value().Type != TokenType::T_CURLY_CLOSE)
	{
		auto statement = parse_statement();
		must_consume(TokenType::T_SEMI);
		if (statement->class_name() == "VariableDeclaration")
			members.push_back(std::move(statement));
		else if (statement->class_name() == "FunctionDeclaration")
			methods.push_back(std::move(statement));
	}
	must_consume(TokenType::T_CURLY_CLOSE);
	m_current_scope_name = oldScopeName;
	return std::make_unique<StructDeclaration>(name.Value.value(), std::move(members), std::move(methods));
}

}