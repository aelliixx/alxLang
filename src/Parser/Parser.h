/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-06.
//

#pragma once
#include <map>
#include "../AST/Ast.h"
#include "../libs/Println.h"
#include "../Tokeniser/Tokeniser.h"

namespace alx {

class Parser
{
	std::map<TokenType, int> m_binary_op_precedence;
	std::vector<Token> m_tokens;
	std::unique_ptr<Program> m_program;
	size_t m_index{};

	int get_binary_op_precedence(const Token& token);

	std::optional<Token> peek(int ahead = 0);
	Token consume();
	std::optional<Token> try_consume(TokenType);
	Token must_consume(TokenType token);
	
public:
	explicit Parser(std::vector<Token> tokens);

	[[nodiscard]] std::unique_ptr<Program> Parse();

private:
	std::unique_ptr<FunctionDeclaration> parse_function();
	std::unique_ptr<VariableDeclaration> parse_variable();
	std::unique_ptr<ASTNode> parse_statement();
	std::unique_ptr<NumberLiteral> parse_number_literal();
	std::unique_ptr<StringLiteral> parse_string_literal();
	std::unique_ptr<Expression> parse_binary_operation(std::unique_ptr<Expression> lhs, int precedence);
	std::unique_ptr<Expression> parse_expression();
	std::unique_ptr<Expression> parse_term();
	std::unique_ptr<ReturnStatement> parse_return_statement();
	std::unique_ptr<IfStatement> parse_if_statement();
	std::unique_ptr<WhileStatement> parse_while_statement();
	std::unique_ptr<BlockStatement> parse_else_statement();
	std::unique_ptr<UnaryExpression> parse_unary_expression();
	void consume_semicolon(const std::unique_ptr<ASTNode>& statement);
};
}
