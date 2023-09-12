/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include <algorithm>
#include <regex>
#include "../../libs/Println.h"
#include "BlockGenerator.h"
#include "../../libs/Utils.h"

namespace alx {

void BlockGenerator::GenerateBlock()
{
	for (const auto& node : m_block_ast.Children())
	{
		auto node_name = node->class_name();
		if (node_name == "VariableDeclaration")
			generate_variables(node);
		else if (node_name == "ReturnStatement")
			generate_return_statement(node);
		else if (node_name == "BinaryExpression")
			generate_binary_expression(node.get());
		else if (node_name == "IfStatement")
			generate_if_statement(node);
		else
			error("Codegen error: Unexpected node '{}'", node_name);
	}
}

void BlockGenerator::generate_return_statement(const std::unique_ptr<ASTNode>& node)
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

void BlockGenerator::add_to_stack(VariableDeclaration* var, size_t ptr)
{
	// FIXME: Align the stack
	auto offset = ptr != 0 ? size_of(var->Type()) : 0;
	align_stack(offset);

	bp += ptr != 0 ? size_of(var->Type()) : 0;
	m_stack[var->Ident().Name()] = { var, bp };
}

void BlockGenerator::add_to_stack(const std::string& name, Expression* value)
{
	if (m_stack.find(name) != m_stack.end())
	{
		auto size = size_of(m_stack[name].first->Type());
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

void BlockGenerator::align_stack(size_t offset)
{
	if (offset == 0) return;
	while ((bp + offset) % offset != 0)
		++bp;
}

void BlockGenerator::push(const std::string& reg)
{
	sp += 8;
	m_asm << "push " << reg << "\n";
}

void BlockGenerator::pop(const std::string& reg)
{
	sp -= 8;
	m_asm << "pop " << reg << "\n";
}

std::string BlockGenerator::bytes_to_data_size(size_t bytes)
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

void BlockGenerator::throw_not_assignable(const Expression* lhs, const Expression* rhs, TokenType op)
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

std::string BlockGenerator::mov(BlockGenerator::Reg dest,
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

std::string BlockGenerator::mov(BlockGenerator::Reg dest, BlockGenerator::Reg src, size_t src_size, bool sign)
{
	std::stringstream code;
	code << "mov " << reg(dest, src_size) << ", " << reg(src, src_size) << "\n";
	return code.str();
}

std::string BlockGenerator::mov(const std::string& dest,
								size_t src_size,
								BlockGenerator::Reg src,
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

std::string BlockGenerator::mov(const std::string& dest,
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

std::string BlockGenerator::offset(size_t offset, BlockGenerator::Reg base, size_t reg_size, bool positive)
{

	std::stringstream code;
	code << "[" << reg(base, reg_size) << (positive ? "+" : "-") << offset << "]";
	return code.str();
}

std::string BlockGenerator::reg(BlockGenerator::Reg reg, size_t bytes)
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

void BlockGenerator::assert_ident_declared(const Identifier* lhs_id)
{
	if (m_stack.find(lhs_id->Name()) == m_stack.end())
		error("Use of undeclared identifier '{}'", lhs_id->Name());
}
std::string BlockGenerator::generate_local_label(IfStatement* statement)
{
	if (!m_local_labels.contains(statement))
	{
		std::stringstream label;
		label << ".L" << m_label_index;
		m_local_labels[statement] = label.str();
	}
	return m_local_labels[statement];
}

} // alx