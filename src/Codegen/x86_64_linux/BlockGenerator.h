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
	size_t lhsSize;
	bool immediateAssignment;
};

class BlockGenerator
{
	enum class Reg
	{
		rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15
	};

	const ScopeNode& m_block_ast;
	std::stringstream m_asm;

	std::list<std::pair<ASTNode*, std::string>> m_local_labels;
	size_t m_label_index{ 2 };

	size_t bp_offset{};
	size_t bp{};
	size_t sp{};

	bool m_early_returns = false;
	bool m_explicit_return = false;
	bool m_in_global_scope = false;
	const std::vector<std::unique_ptr<ASTNode>>& m_program_ast;
	std::map<std::string, std::pair<VariableDeclaration*, size_t>> m_stack;
public:
	BlockGenerator(BlockGenerator&& other) = delete;
	BlockGenerator(const ScopeNode& block,
				   const std::map<std::string, std::pair<VariableDeclaration*, size_t>>& stack,
				   size_t bpOffset,
				   size_t labelIndex,
				   std::list<std::pair<ASTNode*, std::string>>& labels,
				   const std::vector<std::unique_ptr<ASTNode>>& parent)
		: m_block_ast(block),
		  m_stack(stack),
		  bp_offset(bpOffset),
		  m_label_index(labelIndex),
		  m_local_labels(labels),
		  m_program_ast(parent) {}

	BlockGenerator(const ScopeNode& block, const std::vector<std::unique_ptr<ASTNode>>& node)
		: m_block_ast(block), m_program_ast(node)
	{
		m_in_global_scope = true;
	}

	void GenerateBlock();
	[[nodiscard]] std::string Asm() const { return m_asm.str(); }
	[[nodiscard]] bool Returns() const { return m_early_returns; }
	[[nodiscard]] size_t StackPointer() const { return sp; }
	[[nodiscard]] size_t BasePointer() const { return bp; }
	[[nodiscard]] size_t BpOffset() const { return bp_offset; }

protected:
	[[nodiscard]] size_t LabelIndex() const { return m_label_index; }
	[[nodiscard]] std::list<std::pair<ASTNode*, std::string>>& Labels() { return m_local_labels; }

private:
	static std::string reg(Reg reg, size_t bytes = 4);

	void add_to_stack(VariableDeclaration* const, size_t ptr = 4);
	void add_to_stack(const std::string& name, Expression* value);
	void push(const std::string&);
	void pop(const std::string&);
	std::string generate_local_label(ASTNode*);

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
	static std::string mov(Reg dest,
						   size_t src_size,
						   long src,
						   size_t dest_size = 0,
						   bool is_unsigned = false);
	static std::string mov(const std::string& dest,
						   size_t src_size,
						   long src,
						   size_t dest_size = 0,
						   bool is_unsigned = false);

	static std::string mov(Reg dest, Reg src, size_t src_size = 4, bool sign = true);

	static std::string offset(size_t offset, size_t data_size, Reg base = Reg::rbp, size_t reg_size = 8, bool positive = false);

	void generate_variables(const std::unique_ptr<ASTNode>&);
	void generate_return_statement(const std::unique_ptr<ASTNode>&);
	void generate_binary_expression(const ASTNode*, std::optional<Context> = {});
	void generate_unary_expression(const ASTNode*);
	void generate_structs(const ASTNode&);
	void generate_struct_variable(const ASTNode&);
	void generate_body(const BlockStatement& block);

	void generate_assign_num_l(size_t lhs_size, const NumberLiteral* rhs_id);
	void generate_assignment_ident(const Identifier* rhs_id,
								   TokenType lhs_type);
	void generate_if_statement(ASTNode*, const std::optional<std::string>& exitLabel);
	void generate_branch(Expression*,
						 const std::optional<IfStatement*>&,
						 const std::optional<std::string>&,
						 bool = false);
	void generate_while_statement(ASTNode*);

	static void throw_not_assignable(const Expression* lhs, const Expression* rhs, TokenType op);

	static std::string bytes_to_data_size(size_t);

	// Binary methods

	void generate_bin_eq(const ASTNode*);

	void align_stack(size_t offset);
	void assert_ident_initialised(const Identifier* lhs_id);
	void assert_ident_declared(const Identifier* lhs_id);
};

}

