/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-12.
//

#include <algorithm>
#include <array>
#include "ProgramGenerator.h"
#include "BlockGenerator.h"
#include "../../libs/Error.h"

namespace alx {

std::string ProgramGenerator::Generate()
{
	init();
	for (const auto& node : m_ast)
	{
		if (node->class_name() == "FunctionDeclaration")
		{
			auto func = static_cast<FunctionDeclaration*>(node.get());
			m_asm << "\n";
			if (func->Name() == "main")
				m_asm << func->Name() << ":\n";
			else
				m_asm << generate_func_label(*func) << ":\n";
			// Set up the stack frame
			m_asm << "push rbp\n";
			m_asm << "mov rbp, rsp\n";

			BlockGenerator scope{ func->Body(), m_ast };
			scope.GenerateBlock();
			
			auto alignedStack = align_stack(scope.BpOffset());
			// Use the red zone if:
			// - the flag doesn't forbid it
			// - if the stack of the function does not exceed 128 bytes (-8 because we pushed rbp before)
			// - if it's a leaf function
			// https://eli.thegreenplace.net/2011/09/06/stack-frame-layout-on-x86-64#id10
			if (m_flags.mno_red_zone || scope.BpOffset() >= 120)
			{
				if (!m_flags.mno_red_zone)
					m_asm << "sub rsp, " << alignedStack - 120 << "\n";
				else
					m_asm << "sub rsp, " << (alignedStack ? alignedStack : 16) << "\n";
			}
			m_asm << scope.Asm();
			if (!scope.Returns())
			{
				m_asm << "xor eax, eax\n";
				if (scope.StackPointer() == scope.BasePointer() && scope.BpOffset() < 120)
					m_asm << "pop rbp\n";
				else
					m_asm << "leave\n";
				m_asm << "ret\n";
			}
		}
	}
	m_asm_str = m_asm.str();
	consolidate_labels();
	return m_asm_str;
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
		  && static_cast<FunctionDeclaration*>(node.get())->Name() == "main";
	});
	if (main_it == m_ast.end())
		error("No entry point 'main'");

	auto main = static_cast<FunctionDeclaration*>(main_it->get());

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
std::string ProgramGenerator::FormatAsm(const std::string& assembly)
{
	std::string formatted;
	std::array<std::string, 2> asm_keywords = {
		"global", "section"
	};
	std::string buffer;
	size_t index = 0;
	const size_t min_mnemonic_len = 7;
	while (assembly[index] != '\0')
	{
		buffer += assembly[index];

		// If it's a keyword, ignore the line without formatting it.
		if (std::find(asm_keywords.begin(), asm_keywords.end(), buffer) != asm_keywords.end() || buffer.ends_with(":"))
		{
			++index;
			for (; assembly[index] != '\n'; ++index)
				buffer += assembly[index];
			formatted += buffer;
			buffer = "";
			continue;
		}
		else if (assembly[index] == '\n')
		{
			// Add spaces before and after the mnemonic (e.g. "xor eax, eax" -> "     xor   eax, eax";
			if (buffer.find(' ') != std::string::npos)
			{
				auto spaces_to_add = (min_mnemonic_len - buffer.find(' ')) % min_mnemonic_len;
				buffer.insert(buffer.find(' '), spaces_to_add, ' ');
			}
			buffer.insert(0, "     ");
			formatted += buffer;
			buffer = "";
		}
		++index;
	}
	return formatted;
}

// Surely we can generate the labels better to avoid this, but this is the best I've got for now.
void ProgramGenerator::consolidate_labels()
{
	std::string buffer;
	std::string lastBuffer;
	std::vector<std::pair<std::string, std::string>> labelPairs;
	size_t index = 0;
	while (m_asm_str[index] != '\0')
	{
		if (m_asm_str[index] == '\n')
		{
			lastBuffer = buffer;
			buffer = "";
			++index;
			continue;
		}
		buffer += m_asm_str[index];
		if (buffer.ends_with(':'))
		{
			if (lastBuffer.ends_with(":"))
			{
				labelPairs.emplace_back(lastBuffer, buffer);

			}
		}
		++index;
	}
	for (auto& labelPair : labelPairs)
	{
		m_asm_str.replace(m_asm_str.find(labelPair.first), labelPair.first.length(), labelPair.second);
		m_asm_str.replace(m_asm_str.find(labelPair.second), labelPair.second.length() + 1, "");
		labelPair.first.resize(labelPair.first.size() - 1);
		labelPair.second.resize(labelPair.second.size() - 1);
		m_asm_str.replace(m_asm_str.find(labelPair.first), labelPair.first.length(), labelPair.second);
	}
	println("Consolidated {} label(s)", labelPairs.size());
}
size_t ProgramGenerator::align_stack(size_t stackSize)
{
	while ((16 + stackSize) % 16 != 0)
		++stackSize;
	return stackSize;
}

}
