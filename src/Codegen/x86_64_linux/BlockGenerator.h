/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#pragma once

#include <utility>
#include <vector>
#include <sstream>
#include <list>
#include "../../AST/Ast.h"

namespace alx {

struct Context
{
	size_t lhs_size;
};

class BlockGenerator
{
	enum class Reg
	{
		rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15
	};

	const ScopeNode& m_block_ast;
	std::stringstream m_asm;

	std::list<std::pair<ScopeNode*, std::string>> m_local_labels;
	size_t m_label_index{ 2 };

	size_t bp{};
	size_t sp{};

	bool m_returns = false;
	bool m_in_global_scope = false;
	std::optional<ScopeNode*> m_parent_body;
	std::map<std::string, std::pair<VariableDeclaration*, size_t>> m_stack;
public:
	BlockGenerator(BlockGenerator&& other) = delete;
	BlockGenerator(const ScopeNode& block,
				   const std::map<std::string, std::pair<VariableDeclaration*, size_t>>& stack,
				   size_t basePointer,
				   size_t labelIndex,
				   std::list<std::pair<ScopeNode*, std::string>>& labels,
				   ScopeNode* parent)
		: m_block_ast(block),
		  m_stack(stack),
		  bp(basePointer),
		  m_label_index(labelIndex),
		  m_local_labels(labels),
		  m_parent_body(parent) {}

	BlockGenerator(const ScopeNode& block, ScopeNode* node)
		: m_block_ast(block), m_parent_body(node)
	{
		m_in_global_scope = true;
	}

	void GenerateBlock();
	[[nodiscard]] std::string Asm() const { return m_asm.str(); }
	[[nodiscard]] bool Returns() const { return m_returns; }

protected:
	[[nodiscard]] size_t LabelIndex() const { return m_label_index; }
	[[nodiscard]] size_t BasePointer() const { return bp; }
	[[nodiscard]] std::list<std::pair<ScopeNode*, std::string>>& Labels() { return m_local_labels; }

private:
	static std::string reg(Reg reg, size_t bytes = 4);
	

	void add_to_stack(VariableDeclaration*, size_t ptr = 4);
	void add_to_stack(const std::string& name, Expression* value);
	void push(const std::string&);
	void pop(const std::string&);
	std::string generate_local_label(ScopeNode*);

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

	void generate_variables(const std::unique_ptr<ASTNode>&);
	void generate_return_statement(const std::unique_ptr<ASTNode>&);
	void generate_binary_expression(const ASTNode*, std::optional<Context> = {});
	void generate_unary_expression(const ASTNode*);
	void generate_body(const BlockStatement& block);

	void generate_assign_num_l(size_t lhs_size, const NumberLiteral* rhs_id);
	void generate_assignment_ident(const Identifier* rhs_id,
								   TokenType lhs_type);
	void generate_if_statement(ASTNode*, const std::optional<std::string>& exit_label);

	static void throw_not_assignable(const Expression* lhs, const Expression* rhs, TokenType op);

	static std::string bytes_to_data_size(size_t);

	// Binary methods

	void generate_bin_eq(const ASTNode*);

	void align_stack(size_t offset);
	void assert_ident_declared(const Identifier* lhs_id);
};

}

