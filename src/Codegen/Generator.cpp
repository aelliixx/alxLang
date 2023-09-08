/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include <algorithm>
#include <regex>
#include "../libs/Println.h"
#include "Generator.h"

namespace alx {

Code& operator<<(Code& out, std::string code)
{
	if (code.find_first_of(' ') <= code.length())
		code.replace(code.find_first_of(' '), 1, "\t");
	out.m_code << code;
	return out;
}

Code& operator<<(Code& out, int code)
{
	out.m_code << code;
	return out;
}

Code& operator<<(Code& out, size_t code)
{
	out.m_code << code;
	return out;
}


std::string Generator::Generate()
{
	init();
	for (const auto& node : m_ast)
	{
		if (node->class_name() == "FunctionDeclaration")
		{
			const auto& func = reinterpret_cast<FunctionDeclaration*>(node.get());
			m_asm << "\n";
			m_asm << func->Name() << ":\n";
			// Set up the stack frame
			m_asm << "push rbp\n";
			m_asm << "mov rbp, rsp\n";
			generate_block(func->Body());
		}
	}
	return m_asm.str();
}

void Generator::init()
{

	m_asm << "global _start\n";
	// Set up .bss section
	m_asm << "section .bss\n";
	// Set up .data section
	m_asm << "section .data\n";
	// Set up .text section
	m_asm << "section .text\n";
	m_asm << "\n";

	// Find main()
	auto main_it = std::find_if(m_ast.begin(), m_ast.end(), [&](const std::unique_ptr<ASTNode>& node)
	{
	  return node->class_name() == "FunctionDeclaration"
		  && dynamic_cast<FunctionDeclaration*>(node.get())->Name() == "main";
	});
	if (main_it == m_ast.end())
		error("No entry point 'main'");

	auto main = dynamic_cast<FunctionDeclaration*>(main_it->get());

	// Create _start
	m_asm << "_start:\n";
	m_asm << "xor ebp, ebp\n";
	// Call main
	m_asm << "call main\n";
	// Exit with main return
	m_asm << "mov rdi, rax\n";
	m_asm << "mov rax, 60\n";
	m_asm << "syscall\n";



	// Find return statement in main()
	auto return_it = std::find_if(main->Body().Children().begin(),
								  main->Body().Children().end(),
								  [&](const std::unique_ptr<ASTNode>& node)
								  {
									return node->class_name() == "ReturnStatement";
								  });

	if (return_it == main->Body().Children().end())
		m_implicit_return = true;
}
void Generator::generate_variables()
{

}

void Generator::generate_block(const ScopeNode& block)
{
	for (const auto& node : block.Children())
	{
		auto node_name = node->class_name();
		if (node_name == "VariableDeclaration")
		{
			auto var = reinterpret_cast<VariableDeclaration*>(node.get());
			if (var->Value()->class_name() == "NumberLiteral")
			{
				auto value = dynamic_cast<NumberLiteral*>(var->Value());
				switch (value->Type())
				{
				case TokenType::T_INT_L:
				{
					m_asm << "mov QWORD [rbp-4], " << std::get<int>(value->Value()) << "\n";
					m_stack[var->Ident().Name()] = 4;
					break;
				}
				case TokenType::T_FLOAT_L:
				case TokenType::T_DOUBLE_L:
				case TokenType::T_STR_L:
				case TokenType::T_CHAR_L:
				case TokenType::T_TRUE:
				case TokenType::T_FALSE:
				default:
					assert(false && "Not reachable");
				}
			}
		}
		if (node_name == "ReturnStatement")
		{
			auto ret = dynamic_cast<ReturnStatement*>(node.get());
			auto arg = ret->Argument();
			// FIXME get the actual class
			if (arg->class_name() == "Identifier")
			{
				const auto identifier = dynamic_cast<Identifier*>(arg);
				m_asm << "mov rax, QWORD [rbp-" << m_stack.at(identifier->Name()) << "]\n";
			}
			else if (arg->class_name() == "NumberLiteral")
			{
				const auto literal = dynamic_cast<NumberLiteral*>(arg);
				m_asm << "mov rax, ";
				switch (literal->Type())
				{
				case TokenType::T_INT_L:
					m_asm << std::get<int>(literal->Value()) << "\n";
					break;
				case TokenType::T_FLOAT_L:
				case TokenType::T_DOUBLE_L:
				case TokenType::T_CHAR_L:
				case TokenType::T_STR_L:
				case TokenType::T_TRUE:
				case TokenType::T_FALSE:
				default:
					assert(false && "Not reached");
				}
			}
			else {
				assert(false && "Not reached");
			}

			m_asm << "pop rbp\n";
			m_asm << "ret\n";
		}
	}
}
void Generator::format_asm()
{
}
} // alx