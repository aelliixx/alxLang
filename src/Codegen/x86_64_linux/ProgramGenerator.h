/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-12.
//

#pragma once

#include <string>
#include <sstream>
#include "../../AST/Ast.h"


namespace alx {

enum class bitness
{
	x86_64,
	x86_32
};

struct Stack {
	std::string var_name;
	VariableDeclaration* var;
	size_t ptr;
};

class ProgramGenerator
{
	std::vector<Stack> m_stack;
	const std::vector<std::unique_ptr<ASTNode>>& m_ast{};
	std::stringstream m_asm;
	bitness m_bitness = bitness::x86_64;
	bool m_implicit_return = false;
	
	std::vector<std::string> m_labels;
	
public:
	explicit ProgramGenerator(const std::vector<std::unique_ptr<ASTNode>>& ast) : m_ast(ast) {}
	[[nodiscard]] std::string Asm() const { return m_asm.str(); }
	std::string Generate();
	
private:
	void init();
	[[nodiscard]] std::string generate_func_label(const FunctionDeclaration& name); 
	
	
	void format_asm(); // TODO
	
};

}
