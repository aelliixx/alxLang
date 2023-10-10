/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-06.
//

#pragma once
#include <map>
#include <list>
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
	std::string m_current_scope_name;
	std::variant<TokenType, std::unique_ptr<Identifier>> m_current_return_type;
	std::map<std::string, std::vector<VariableDeclaration*>> m_variables;

	int get_binary_op_precedence(const Token& token);

	std::optional<Token> peek(int ahead = 0);
	Token consume();
	std::optional<Token> try_consume(TokenType);
	Token must_consume(TokenType token);
	std::shared_ptr<ErrorHandler> m_error;
	
public:
	Parser(std::vector<Token> tokens, const std::shared_ptr<ErrorHandler>& errorHandler);

	[[nodiscard]] std::unique_ptr<Program> Parse();
	[[nodiscard]] const Program& GetAst() { return *m_program;}

private:
	RefPtr<FunctionDeclaration> parse_function();
	RefPtr<VariableDeclaration> parse_variable();
	RefPtr<ASTNode> parse_statement();
	RefPtr<NumberLiteral> parse_number_literal();
	RefPtr<StringLiteral> parse_string_literal();
	std::optional<ValueExpression> parse_binary_operation(ValueExpression lhs, int precedence);
	RefPtr<ValueExpression> parse_expression();
	std::optional<ValueExpression> parse_term();
	RefPtr<ReturnStatement> parse_return_statement();
	RefPtr<IfStatement> parse_if_statement();
	RefPtr<WhileStatement> parse_while_statement();
	RefPtr<BlockStatement> parse_else_statement();
	RefPtr<UnaryExpression> parse_unary_expression();
	RefPtr<StructDeclaration> parse_struct_declaration();
	RefPtr<MemberExpression> parse_member_expression();
	void consume_semicolon(const RefPtr<ASTNode>& statement);
	void add_variable(VariableDeclaration*);
};
}
