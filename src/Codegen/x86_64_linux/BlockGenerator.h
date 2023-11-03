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
#include <unordered_map>
#include "../../AST/Ast.h"
#include "../../Utils/Flags.h"

namespace alx {

struct Context
{
	size_t LhsSize{};
	bool AssignmentChain{};
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

	size_t m_bp_offset{};
	size_t m_bp{};
	size_t m_sp{};

	bool m_early_returns = false;
	bool m_explicit_return = false;
	bool m_in_global_scope = false;
	const std::vector<std::unique_ptr<ASTNode>>& m_program_ast;
	std::unordered_map<std::string, std::pair<size_t, size_t>> m_stack;
	std::unordered_map<std::string, TokenType> m_stack_types;
	TokenType m_return_type;

	Flags m_flags{};
public:
	BlockGenerator(BlockGenerator&& other) = delete;
	BlockGenerator(const ScopeNode& block,
				   const std::unordered_map<std::string, std::pair<size_t, size_t>>& stack,
				   size_t bpOffset,
				   size_t labelIndex,
				   std::list<std::pair<ASTNode*, std::string>>& labels,
				   const std::vector<std::unique_ptr<ASTNode>>& parent,
				   Flags flags)
		: m_block_ast(block),
		  m_local_labels(labels),
		  m_label_index(labelIndex),
		  m_bp_offset(bpOffset),
		  m_program_ast(parent),
		  m_stack(stack),
		  m_flags(flags) {}

	BlockGenerator(const ScopeNode& block,
				   const std::vector<std::unique_ptr<ASTNode>>& node,
				   Flags flags,
				   TokenType returnType)
		: m_block_ast(block), m_program_ast(node), m_return_type(returnType), m_flags(flags)
	{
		m_in_global_scope = true;
	}

	void GenerateBlock();
	[[nodiscard]] std::string Asm() const { return m_asm.str(); }
	[[nodiscard]] bool Returns() const { return m_early_returns; }
	[[nodiscard]] size_t StackPointer() const { return m_sp; }
	[[nodiscard]] size_t BasePointer() const { return m_bp; }
	[[nodiscard]] size_t BpOffset() const { return m_bp_offset; }

protected:
	[[nodiscard]] size_t label_index() const { return m_label_index; }
	[[nodiscard]] std::list<std::pair<ASTNode*, std::string>>& labels() { return m_local_labels; }

private:
	static std::string reg(Reg reg, size_t bytes = 4);

	void add_to_stack(const std::string&, size_t, TokenType);
	void push(const std::string&);
	void pop(const std::string&);
	std::string generate_local_label(ASTNode*);

	static std::string mov(Reg dest,
						   size_t srcSize,
						   const std::string& src,
						   size_t destSize = 0,
						   bool isUnsigned = false);
	static std::string mov(const std::string& dest,
						   size_t srcSize,
						   Reg src,
						   size_t destSize = 0);
	static std::string mov(const std::string& dest,
						   size_t srcSize,
						   const std::string& src,
						   size_t destSize = 0,
						   bool isUnsigned = false);
	static std::string mov(Reg dest,
						   size_t srcSize,
						   long src,
						   size_t destSize = 0,
						   bool isUnsigned = false);
	static std::string mov(const std::string& dest,
						   size_t srcSize,
						   long src,
						   size_t destSize = 0,
						   bool isUnsigned = false);

	static std::string mov(Reg dest, Reg src, size_t srcSize = 4);

	static std::string offset(size_t offset,
							  size_t dataSize,
							  Reg base = Reg::rbp,
							  size_t regSize = 8,
							  bool positive = false);

	void generate_variables(const std::unique_ptr<ASTNode>&);
	void generate_return_statement(const std::unique_ptr<ASTNode>&);
	void generate_binary_expression(const ASTNode*, std::optional<Context> = {});
	void generate_unary_expression(const ASTNode*);
	void generate_structs(const ASTNode&);
	void generate_struct_variable(const ASTNode&);
	void generate_body(const BlockStatement& block);

	void generate_assign_num_l(size_t lhsSize, const NumberLiteral* rhsId);
	void generate_assignment_ident(const Identifier& rhsId, size_t, bool);
	void generate_if_statement(ASTNode*, const std::optional<std::string>& exitLabel);
	void generate_branch(Expression*,
						 const std::optional<IfStatement*>&,
						 const std::optional<std::string>&,
						 bool = false);
	void generate_while_statement(ASTNode*);

	static void throw_not_assignable(const Expression* lhs, const Expression* rhs, TokenType op);

	static std::string bytes_to_data_size(size_t);

	// Binary methods

	void generate_bin_eq(const ASTNode*, std::optional<Context>);

	void align_stack(size_t offset);
	void assert_ident_initialised(const Identifier* lhsId);
	void assert_ident_declared(const Identifier* lhsId);
};

}

