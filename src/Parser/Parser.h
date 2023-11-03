/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-06.
//

#pragma once
#include <map>
#include <list>
#include <unordered_map>
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
	
	// FIXME: It is not unreasonable that a project may contain tens of thousands of variables.
	//     	  Therefore we might want to split this hashmap by scope e.g. 
	//     	  std::unordered_map<std::string, std::unordered_map<std::string, VariableDeclaration*>> m_variables;
	//                                ^ scope name                        	^ variable name
	std::unordered_map<std::string, VariableDeclaration*> m_variables;

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
	std::unique_ptr<StructDeclaration> parse_struct_declaration();
	std::unique_ptr<MemberExpression> parse_member_expression();
	void consume_semicolon(const std::unique_ptr<ASTNode>& statement);
	void add_variable(VariableDeclaration*);
	
	bool find_variable_by_name(const std::string& qualifiedName);
	static std::string get_fully_qualified_name(const std::string& name, const std::string& scope);
};
}
