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
#include "../../Utils/Utils.h"
#include "../../libs/Error.h"

namespace alx {

void BlockGenerator::GenerateBlock()
{
	for (const auto& node : m_block_ast.Children())
	{
		auto node_name = node->class_name();
		if (node_name == "VariableDeclaration")
		{
			const auto& var = static_cast<VariableDeclaration&>(*node);
			if (var.TypeIndex() == 0)
				generate_variables(node);
			else
				generate_struct_variable(*node);
		}
		else if (node_name == "ReturnStatement")
			generate_return_statement(node);
		else if (node_name == "BinaryExpression")
			generate_binary_expression(node.get());
		else if (node_name == "IfStatement")
			generate_if_statement(node.get(), {});
		else if (node_name == "WhileStatement")
			generate_while_statement(node.get());
		else if (node_name == "StructDeclaration")
			generate_structs(*node);
		else
			error("Codegen error: Unexpected node '{}'", node_name);
	}

}

void BlockGenerator::generate_return_statement(const std::unique_ptr<ASTNode>& node)
{
	auto ret = static_cast<ReturnStatement*>(node.get());
	auto arg = ret->Argument();
	// FIXME get the actual class
	if (arg->class_name() == "Identifier")
	{
		const auto identifier = static_cast<Identifier*>(arg);
		assert_ident_initialised(identifier);
		auto rhs_ptr = m_stack.at(identifier->Name()).second;
		m_asm << mov(Reg::rax, 8, offset(rhs_ptr, 8));
	}
	else if (arg->class_name() == "NumberLiteral")
	{
		const auto literal = static_cast<NumberLiteral*>(arg);
		switch (literal->Type())
		{
		case TokenType::T_INT_L:
			if (literal->AsInt() == 0)
			{
				m_asm << "xor rax, rax\n";
				break;
			}
			m_asm << "mov rax, " << literal->Value() << "\n";
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
		generate_binary_expression(arg);
	else if (arg->class_name() == "UnaryExpression")
		generate_unary_expression(arg);
	else if (arg->class_name() == "MemberExpression")
	{
		// TODO: Test this code
		const auto member = static_cast<MemberExpression*>(arg);
		auto rhs_ptr = m_stack.at(member->Member().Name()).second;
		m_asm << mov(Reg::rax, 8, offset(rhs_ptr, 8));
	}
	else
	{
		ASSERT_NOT_REACHABLE();
	}
	if (!m_in_global_scope)
	{
//		m_asm << "jmp " << generate_local_label(m_parent_body.value()) << "\n";
	}
	else
	{
//		if (m_returns)
//			m_asm << generate_local_label(m_parent_body.value()) << ":\n";
	}
	if (sp == bp && bp_offset < 120)
		m_asm << "pop rbp\n";
	else
		m_asm << "leave\n";
	m_asm << "ret\n";
	m_early_returns = true;
}

void BlockGenerator::add_to_stack(VariableDeclaration* const var, size_t ptr)
{
	auto offset = ptr != 0 ? size_of(var->TypeAsPrimitive()) : 0;
	align_stack(offset);
//	if (m_stack.find(var->Name()) != m_stack.end())
//		assert(false && "Cannot have two vars with the same name on the stack");
	bp_offset += ptr != 0 ? size_of(var->TypeAsPrimitive()) : 0;
	m_stack[var->Ident().Name()] = { var, offset };
}

void BlockGenerator::add_to_stack(const std::string& name, Expression* value)
{
	if (m_stack.find(name) != m_stack.end())
	{
		auto size = size_of(m_stack[name].first->TypeAsPrimitive());
		if (!m_stack[name].first->Value()) // If the value is uninitialised, initialise it
		{
			align_stack(size);
			bp_offset += size;
			m_stack[name].second = bp_offset;
		}
		m_stack[name].first->AssignValue(std::make_unique<Expression>(*value));
		return;
	}
	ASSERT_NOT_REACHABLE();
}

void BlockGenerator::align_stack(size_t offset)
{
	if (offset == 0) return;
	while ((offset + bp_offset) % offset != 0)
		++bp_offset;
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
			  static_cast<const NumberLiteral*>(lhs)->Value(),
			  token_to_string(op),
			  static_cast<const NumberLiteral*>(rhs)->Value());
	else if (rhs->class_name() == "Identifier")
		error("Expression '{} {} {}' is not assignable",
			  static_cast<const NumberLiteral*>(lhs)->Value(),
			  token_to_string(op),
			  static_cast<const Identifier*>(rhs)->Name());
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
	if (dest_size - src_size >= 2)
		code << (is_unsigned ? "movzx " : "movsx ");
	else
		code << "mov ";
	// If rhs is a byte or a word, use the data size of lhs (because movzx/movsx already extended it)
	code << reg(dest, (src_size <= 2 ? dest_size : src_size)) << ", " << src << "\n";
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
	if (dest_size - src_size >= 2)
		code << (is_unsigned ? "movzx " : "movsx ");
	else
		code << "mov ";
	code << dest << ", " << reg(src, src_size) << "\n";
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

	if (dest_size == 0)
		dest_size = src_size;
	// If rhs is a byte or a word, use movzx/movsx to extend it to a dword or qword, otherwise - regular mov
	if (dest_size - src_size >= 2)
		code << (is_unsigned ? "movzx " : "movsx ");
	else
		code << "mov ";

	code << dest << ", " << src << "\n";
	return code.str();
}

std::string BlockGenerator::mov(BlockGenerator::Reg dest, size_t src_size, long src, size_t dest_size, bool is_unsigned)
{
	return mov(dest, src_size, std::to_string(src), dest_size, is_unsigned);
}
std::string BlockGenerator::mov(const std::string& dest, size_t src_size, long src, size_t dest_size, bool is_unsigned)
{
	return mov(dest, src_size, std::to_string(src), dest_size, is_unsigned);
}

std::string BlockGenerator::offset(size_t offset,
								   size_t data_size,
								   BlockGenerator::Reg base,
								   size_t reg_size,
								   bool positive)
{

	std::stringstream code;
	code << bytes_to_data_size(data_size);
	code << " [" << reg(base, reg_size) << (positive ? "+" : "-") << offset << "]";
	return code.str();
}

std::string BlockGenerator::reg(BlockGenerator::Reg reg, size_t bytes)
{
	switch (reg)
	{
	case Reg::rax:
		return bytes == 8 ? "rax" : bytes == 4 ? "eax" : bytes == 2 ? "ax" : "al";
	case Reg::rbx:
		return bytes == 8 ? "rbx" : bytes == 4 ? "ebx" : bytes == 2 ? "bx" : "bl";
	case Reg::rcx:
		return bytes == 8 ? "rcx" : bytes == 4 ? "ecx" : bytes == 2 ? "cx" : "cl";
	case Reg::rdx:
		return bytes == 8 ? "rdx" : bytes == 4 ? "edx" : bytes == 2 ? "dx" : "dl";
	case Reg::rsi:
		return bytes == 8 ? "rsi" : bytes == 4 ? "esi" : bytes == 2 ? "si" : "sil";
	case Reg::rdi:
		return bytes == 8 ? "rdi" : bytes == 4 ? "edi" : bytes == 2 ? "di" : "dil";
	case Reg::rbp:
		return bytes == 8 ? "rbp" : bytes == 4 ? "ebp" : bytes == 2 ? "offset" : "bpl";
	case Reg::rsp:
		return bytes == 8 ? "rsp" : bytes == 4 ? "esp" : bytes == 2 ? "sp" : "spl";
	case Reg::r8:
		return bytes == 8 ? "r8" : bytes == 4 ? "r8d" : bytes == 2 ? "r8w" : "r8l";
	case Reg::r9:
		return bytes == 8 ? "r9" : bytes == 4 ? "r9d" : bytes == 2 ? "r9w" : "r9l";
	case Reg::r10:
		return bytes == 8 ? "r10" : bytes == 4 ? "r10d" : bytes == 2 ? "r10w" : "r10l";
	case Reg::r11:
		return bytes == 8 ? "r11" : bytes == 4 ? "r11d" : bytes == 2 ? "r11w" : "r11l";
	case Reg::r12:
		return bytes == 8 ? "r12" : bytes == 4 ? "r12d" : bytes == 2 ? "r12w" : "r12l";
	case Reg::r13:
		return bytes == 8 ? "r13" : bytes == 4 ? "r13d" : bytes == 2 ? "r13w" : "r13l";
	case Reg::r14:
		return bytes == 8 ? "r14" : bytes == 4 ? "r14d" : bytes == 2 ? "r14w" : "r14l";
	case Reg::r15:
		return bytes == 8 ? "r15" : bytes == 4 ? "r15d" : bytes == 2 ? "r15w" : "r15l";
	default:
	ASSERT_NOT_REACHABLE();
	}

}

std::string BlockGenerator::generate_local_label(ASTNode* statement)
{
	auto it = std::find_if(m_local_labels.begin(),
						   m_local_labels.end(),
						   [&statement](const std::pair<ASTNode*, std::string>& label)
						   {
							 return label.first == statement;
						   });

	if (it == m_local_labels.end())
	{
		std::stringstream label;
		label << ".L" << m_label_index;
		std::pair<ASTNode*, std::string> new_label = { statement, label.str() };
		m_local_labels.push_back(new_label);
		++m_label_index;
		return label.str();
	}
	return it->second;
}

void BlockGenerator::generate_body(const BlockStatement& block)
{
	BlockGenerator body{ block, m_stack, bp_offset, m_label_index, m_local_labels, m_program_ast };
	body.GenerateBlock();
	m_early_returns = body.Returns();
	m_label_index = body.LabelIndex();
	m_local_labels = body.Labels();
	bp_offset = body.BpOffset();
	sp = body.StackPointer();
	bp = body.BasePointer();
	m_asm << body.Asm();
}

void BlockGenerator::assert_ident_initialised(const Identifier* lhs_id)
{
	auto it = m_stack.find(lhs_id->Name());
	if (it == m_stack.end())
		error("Codegen error: Use of undeclared identifier '{}'. This is a codegen "
			  "error and must be fixed in the parser.", lhs_id->Name());
	else if (it->second.second == 0)
		error("Codegen error: Use of uninitialised identifier '{}'. This is a codegen"
			  "error and must be fixed in the parser", lhs_id->Name());
}

void BlockGenerator::assert_ident_declared(const Identifier* lhs_id)
{
	auto it = m_stack.find(lhs_id->Name());
	if (it == m_stack.end())
		error("Codegen error: Use of undeclared identifier '{}'. This is a codegen "
			  "error and must be fixed in the parser.", lhs_id->Name());

}

} // alx