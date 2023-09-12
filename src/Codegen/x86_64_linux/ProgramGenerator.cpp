/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-12.
//

#include <algorithm>
#include "ProgramGenerator.h"
#include "BlockGenerator.h"
#include "../../libs/Println.h"
#include "../../libs/Utils.h"

namespace alx {

std::string ProgramGenerator::Generate()
{
	init();
	for (const auto& node : m_ast)
	{
		if (node->class_name() == "FunctionDeclaration")
		{
			const auto& func = dynamic_cast<FunctionDeclaration*>(node.get());
			m_asm << "\n";
			if (func->Name() == "main")
				m_asm << func->Name() << ":\n";
			else
				m_asm << generate_func_label(*func) << ":\n";
			// Set up the stack frame
			m_asm << "push rbp\n";
			m_asm << "mov rbp, rsp\n";
			BlockGenerator scope{ func->Body() };
			scope.GenerateBlock();
			m_asm << scope.Asm();
			if (func->Name() == "main" && m_implicit_return)
			{
				m_asm << "xor eax, eax\n";
				m_asm << "pop rbp\n";
				m_asm << "ret\n";
			}
			else
			{
				m_asm << "pop rbp\n";
				m_asm << "ret\n";
			}
		}
	}
	return m_asm.str();
}

void ProgramGenerator::init()
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
		auto name = node->class_name();
									return node->class_name() == "ReturnStatement";
								  });

	if (return_it == main->Body().Children().end())
		m_implicit_return = true;
}
std::string ProgramGenerator::generate_func_label(const FunctionDeclaration& func)
{
	const auto& args = func.Arguments();
	std::stringstream label;
	label << func.Name() << "(";
	const char* delim = "";
	for (const auto& arg : args)
	{
		label << delim << arg->TypeName();
		delim = ", ";
	}
	label << ")";
	auto label_str = label.str();
	if (std::find(m_labels.begin(), m_labels.end(), label.str()) == m_labels.end())
	{
		m_labels.push_back(label.str());
		return label.str();
	}
	ASSERT_NOT_REACHABLE(); // This should not be reached because the parser should catch these errors.
}
}
