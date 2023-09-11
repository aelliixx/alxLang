/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#pragma once

#include <vector>
#include <sstream>
#include "../../AST/Ast.h"

namespace alx {

enum class bitness
{
	x86_64,
	x86_32
};

struct Context {
	size_t lhs_size, rhs_size;
};

class Generator
{
	enum class Reg
	{
		rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15
	};

	const std::vector<std::unique_ptr<ASTNode>>& m_ast;
	std::stringstream m_asm;

	size_t bp{};
	size_t sp{};
	std::map<std::string, std::pair<VariableDeclaration*, size_t>> m_stack;
	bitness m_bitness = bitness::x86_64;
	bool m_implicit_return = false;

	std::map<TokenType, size_t> m_type_size;

public:
	explicit Generator(const std::vector<std::unique_ptr<ASTNode>>& ast) : m_ast(ast)
	{
		m_type_size[TokenType::T_CHAR] = 1;
		m_type_size[TokenType::T_CHAR_L] = 1;
		m_type_size[TokenType::T_BOOL] = 1;
		m_type_size[TokenType::T_SHORT] = 2;
		m_type_size[TokenType::T_INT] = 4;
		m_type_size[TokenType::T_INT_L] = 4;
		m_type_size[TokenType::T_LONG] = 8;
		m_type_size[TokenType::T_FLOAT] = 4;
		m_type_size[TokenType::T_FLOAT_L] = 8;
		m_type_size[TokenType::T_DOUBLE] = 8;
		m_type_size[TokenType::T_DOUBLE_L] = 8;
	}

	std::string Generate();
	std::string Asm() const { return m_asm.str(); }

private:
	static std::string reg(Reg reg, size_t bytes = 4);

	void add_to_stack(VariableDeclaration*, size_t ptr = 4);
	void add_to_stack(const std::string& name, Expression* value);
	void push(const std::string&);
	void pop(const std::string&);

	static std::string mov(Reg dest,
						   size_t src_size,
						   const std::string& src,
						   size_t dest_size = 0,
						   bool is_unsigned = false);
	static std::string mov(const std::string& dest,
						   size_t src_size,
						   Reg src,
						   size_t dest_size = 0,
						   bool is_unsigned = false);
	static std::string mov(const std::string& dest,
						   size_t src_size,
						   const std::string& src,
						   size_t dest_size = 0,
						   bool is_unsigned = false);
	static std::string mov(Reg dest, Reg src, size_t src_size = 4, bool sign = true);

	static std::string offset(size_t offset, Reg base = Reg::rbp, size_t reg_size = 8, bool positive = false);

	void init();
	void generate_block(const ScopeNode&);
	void generate_variables(const std::unique_ptr<ASTNode>&);
	void generate_return_statement(const std::unique_ptr<ASTNode>&);
	void generate_binary_expression(const ASTNode*, std::optional<Context> = {});

	void generate_assign_num_l(size_t lhs_size, const NumberLiteral* rhs_id);
	void generate_assignment_ident(const Identifier* rhs_id,
								   TokenType lhs_type);

	static void throw_not_assignable(const Expression* lhs, const Expression* rhs, TokenType op);

	static std::string bytes_to_data_size(size_t);

	// Binary methods

	void generate_bin_eq(const ASTNode*);

	void format_asm(); // TODO
	void align_stack(size_t offset);
	void assert_ident_declared(const Identifier* lhs_id);
};

}

