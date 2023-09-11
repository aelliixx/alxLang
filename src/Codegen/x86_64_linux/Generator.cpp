/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include <algorithm>
#include <regex>
#include "../../libs/Println.h"
#include "Generator.h"
#include "../../libs/Utils.h"

namespace alx {

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
			if (func->Name() == "main" && m_implicit_return)
			{
				m_asm << "xor eax, eax\n";
				m_asm << "pop rbp\n";
				m_asm << "ret\n";
			}
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

void Generator::generate_block(const ScopeNode& block)
{
	for (const auto& node : block.Children())
	{
		auto node_name = node->class_name();
		if (node_name == "VariableDeclaration")
			generate_variables(node);
		if (node_name == "ReturnStatement")
			generate_return_statement(node);
		if (node_name == "BinaryExpression")
			generate_binary_expression(node.get());
	}
}

void Generator::generate_return_statement(const std::unique_ptr<ASTNode>& node)
{
	auto ret = dynamic_cast<ReturnStatement*>(node.get());
	auto arg = ret->Argument();
	// FIXME get the actual class
	if (arg->class_name() == "Identifier")
	{
		const auto identifier = dynamic_cast<Identifier*>(arg);
		assert_ident_declared(identifier);
		auto rhs_ptr = m_stack.at(identifier->Name()).second;
		m_asm << mov(reg(Reg::rax, 8), 8, offset(rhs_ptr));
	}
	else if (arg->class_name() == "NumberLiteral")
	{
		const auto literal = dynamic_cast<NumberLiteral*>(arg);
		m_asm << "mov rax, ";
		switch (literal->Type())
		{
		case TokenType::T_INT_L:
			m_asm << literal->Value() << "\n";
			break;
		case TokenType::T_FLOAT_L:
		case TokenType::T_DOUBLE_L:
		case TokenType::T_CHAR_L:
		case TokenType::T_STR_L:
		case TokenType::T_TRUE:
		case TokenType::T_FALSE:
		ASSERT_NOT_IMPLEMENTED();
		default:
			assert(false && "Not reached");
		}
	}
	else if (arg->class_name() == "BinaryExpression")
	{
		generate_binary_expression(arg);
	}
	else
	{
		ASSERT_NOT_REACHABLE();
	}

	m_asm << "pop rbp\n";
	m_asm << "ret\n";
}

void Generator::add_to_stack(VariableDeclaration* var, size_t ptr)
{
	// FIXME: Align the stack
	auto offset = ptr != 0 ? m_type_size[var->Type()] : 0;
	align_stack(offset);

	bp += ptr != 0 ? m_type_size[var->Type()] : 0;
	m_stack[var->Ident().Name()] = { var, bp };
}

void Generator::add_to_stack(const std::string& name, Expression* value)
{
	if (m_stack.find(name) != m_stack.end())
	{
		auto size = m_type_size[m_stack[name].first->Type()];
		if (!m_stack[name].first->Value()) // If the value is uninitialised, initialise it
		{
			align_stack(size);
			bp += size;
			m_stack[name].second = bp;
		}
		m_stack[name].first->AssignValue(std::make_unique<Expression>(*value));
		return;
	}
	ASSERT_NOT_REACHABLE();
}

void Generator::align_stack(size_t offset)
{
	if (offset == 0) return;
	while ((bp + offset) % offset != 0)
		++bp;
}

void Generator::push(const std::string& reg)
{
	sp += 8;
	m_asm << "push " << reg << "\n";
}

void Generator::pop(const std::string& reg)
{
	sp -= 8;
	m_asm << "pop " << reg << "\n";
}

std::string Generator::bytes_to_data_size(size_t bytes)
{
	switch (bytes)
	{
	case 1:
		return "BYTE";
	case 2:
		return "WORD";
	case 4:
		return "DWORD";
	case 8:
		return "QWORD";
	default:
	ASSERT_NOT_REACHABLE();
	}
}

void Generator::throw_not_assignable(const Expression* lhs, const Expression* rhs, TokenType op)
{
	if (rhs->class_name() == "NumberLiteral")
		error("Expression '{} {} {}' is not assignable",
			  dynamic_cast<const NumberLiteral*>(lhs)->Value(),
			  token_to_string(op),
			  dynamic_cast<const NumberLiteral*>(rhs)->Value());
	else if (rhs->class_name() == "Identifier")
		error("Expression '{} {} {}' is not assignable",
			  dynamic_cast<const NumberLiteral*>(lhs)->Value(),
			  token_to_string(op),
			  dynamic_cast<const Identifier*>(rhs)->Name());
}

std::string Generator::mov(Generator::Reg dest,
						   size_t src_size,
						   const std::string& src,
						   size_t dest_size,
						   bool is_unsigned)
{
	std::stringstream code;
	if (dest_size == 0)
		dest_size = src_size;
	// If rhs is a byte or a word, use movzx/movsx to extend it to a dword or qword, otherwise - regular mov
	if (src_size <= 2)
		code << (is_unsigned ? "movzx " : "movsx ");
	else
		code << "mov ";
	// If rhs is a byte or a word, use the data size of lhs (because movzx/movsx already extended it)
	code << reg(dest, (src_size <= 2 ? dest_size : src_size)) << ", " << bytes_to_data_size(src_size)
		 << " " << src << "\n";
	return code.str();
}

std::string Generator::mov(Generator::Reg dest, Generator::Reg src, size_t src_size, bool sign)
{
	std::stringstream code;
	code << "mov " << reg(dest, src_size) << ", " << reg(src, src_size) << "\n";
	return code.str();
}

std::string Generator::mov(const std::string& dest,
						   size_t src_size,
						   Generator::Reg src,
						   size_t dest_size,
						   bool is_unsigned)
{
	std::stringstream code;
	if (dest_size == 0)
		dest_size = src_size;

	if (dest_size == 8 && src_size == 4)
		code << "cdqe\n";
	code << "mov " << bytes_to_data_size(dest_size) << " " << dest << ", " << reg(src, src_size) << "\n";
	return code.str();
}

std::string Generator::mov(const std::string& dest,
						   size_t src_size,
						   const std::string& src,
						   size_t dest_size,
						   bool is_unsigned)
{
	std::stringstream code;
	if (dest_size == 0)
		dest_size = src_size;
	code << "mov " << bytes_to_data_size(dest_size) << " " << dest << ", " << src << "\n";
	return code.str();
}

std::string Generator::offset(size_t offset, Generator::Reg base, size_t reg_size, bool positive)
{

	std::stringstream code;
	code << "[" << reg(base, reg_size) << (positive ? "+" : "-") << offset << "]";
	return code.str();
}

std::string Generator::reg(Generator::Reg reg, size_t bytes)
{
	switch (reg)
	{
	case Reg::rax:
		return bytes == 8 ? "rax" : "eax";
	case Reg::rbx:
		return bytes == 8 ? "rbx" : "ebx";
	case Reg::rcx:
		return bytes == 8 ? "rcx" : "ecx";
	case Reg::rdx:
		return bytes == 8 ? "rdx" : "edx";
	case Reg::rsi:
		return bytes == 8 ? "rsi" : "esi";
	case Reg::rdi:
		return bytes == 8 ? "rdi" : "edi";
	case Reg::rbp:
		return bytes == 8 ? "rbp" : "ebp";
	case Reg::rsp:
		return bytes == 8 ? "rsp" : "esp";
	case Reg::r8:
		return bytes == 8 ? "r8" : "r8d";
	case Reg::r9:
		return bytes == 8 ? "r9" : "r9d";
	case Reg::r10:
		return bytes == 8 ? "r10" : "r10d";
	case Reg::r11:
		return bytes == 8 ? "r11" : "r11d";
	case Reg::r12:
		return bytes == 8 ? "r12" : "r12d";
	case Reg::r13:
		return bytes == 8 ? "r13" : "r13d";
	case Reg::r14:
		return bytes == 8 ? "r14" : "r14d";
	case Reg::r15:
		return bytes == 8 ? "r15" : "r15d";
	default:
	ASSERT_NOT_REACHABLE();
	}

}

void Generator::assert_ident_declared(const Identifier* lhs_id)
{
	if (m_stack.find(lhs_id->Name()) == m_stack.end())
		error("Use of undeclared identifier '{}'", lhs_id->Name());
}

} // alx