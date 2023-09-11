/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#pragma once
#include <vector>
#include <string>
#include "../AST/Ast.h"

namespace alx {

class Code
{
	std::stringstream m_code{};
public:
	Code() = default;

	void operator<(const std::stringstream& code)
	{
		m_code << code.str();
	}

	[[nodiscard]] std::string str() const
	{
		return m_code.str();
	}

	friend Code& operator<<(Code& out, std::string code);
	friend Code& operator<<(Code& out, int code);
	friend Code& operator<<(Code& out, size_t code);

};

enum class bitness
{
	x86_64,
	x86_32
};

class Generator
{
	const std::vector<std::unique_ptr<ASTNode>>& m_ast;
	Code m_asm;

	std::map<std::string, size_t> m_stack;
	bitness m_bitness = bitness::x86_64;
	bool m_implicit_return = false;
	
	std::map<TokenType, uint> m_type_size;

public:
	explicit Generator(const std::vector<std::unique_ptr<ASTNode>>& ast) : m_ast(ast) {
		m_type_size[TokenType::T_INT] = 4;
		m_type_size[TokenType::T_FLOAT] = 4;
		m_type_size[TokenType::T_DOUBLE] = 8;
		m_type_size[TokenType::T_CHAR] = 1;
	}

	std::string Generate();
	std::string Asm() const { return m_asm.str(); }

private:
	void format_asm();
	
	
	void init();
	void generate_block(const ScopeNode&);
	void generate_variables(const std::unique_ptr<ASTNode>&);
	void generate_return_statement(const std::unique_ptr<ASTNode>&);

};

}

