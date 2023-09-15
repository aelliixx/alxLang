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

class ProgramGenerator
{
	const std::vector<std::unique_ptr<ASTNode>>& m_ast{};
	std::stringstream m_asm;
	std::string m_asm_str;
	bitness m_bitness = bitness::x86_64;
	bool m_implicit_return = false;
	
	std::vector<std::string> m_labels;
	
public:
	explicit ProgramGenerator(const std::vector<std::unique_ptr<ASTNode>>& ast) : m_ast(ast) {}
	[[nodiscard]] std::string Asm() const { return m_asm_str; }
	std::string Generate();
	static std::string FormatAsm(const std::string& assembly); // TODO
	
private:
	void init();
	[[nodiscard]] std::string generate_func_label(const FunctionDeclaration& name);
	
	// Merges labels which are next to each other into one
	void consolidate_labels();
	
	
	
};

}
