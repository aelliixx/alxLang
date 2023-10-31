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

namespace alx {

void BlockGenerator::GenerateBlock()
{
	for (const auto& node : m_block_ast.Children())
	{
		auto nodeName = node->class_name();
		if (nodeName == "VariableDeclaration")
		{
			const auto& var = static_cast<VariableDeclaration&>(*node);
			if (var.TypeIndex() == 0)
				generate_variables(node);
			else
				generate_struct_variable(*node);
		}
		else if (nodeName == "ReturnStatement")
			generate_return_statement(node);
		else if (nodeName == "BinaryExpression")
			generate_binary_expression(node.get());
		else if (nodeName == "IfStatement")
			generate_if_statement(node.get(), {});
		else if (nodeName == "WhileStatement")
			generate_while_statement(node.get());
		else if (nodeName == "StructDeclaration")
			generate_structs(*node);
		else
			error("Codegen Error: Unexpected node '{}'", nodeName);
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
		auto rhsPtr = m_stack.at(identifier->Name()).first;
		m_asm << mov(Reg::rax, 8, offset(rhsPtr, 8));
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
			MUST(false && "Not reached");
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
		auto rhsPtr = m_stack.at(member->Object().Name() + "::" + member->Member().Name()).first;
		m_asm << mov(Reg::rax, 8, offset(rhsPtr, 8));
	}
	else
	{
		ASSERT_NOT_REACHABLE();
	}

	if (m_sp == m_bp && m_bp_offset < 120 && !m_flags.mno_red_zone)
		m_asm << "pop rbp\n";
	else
		m_asm << "leave\n";
	m_asm << "ret\n";
	m_early_returns = true;
}

void BlockGenerator::add_to_stack(const std::string& name, size_t size, TokenType type)
{
	align_stack(size);
	m_bp_offset += size;
	if (m_stack.find(name) == m_stack.end())
		m_stack_types[name] = type;
	m_stack[name].first = m_bp_offset;
	m_stack[name].second = size;

}

void BlockGenerator::align_stack(size_t offset)
{
	if (offset == 0) return;
	while ((offset + m_bp_offset) % offset != 0)
		++m_bp_offset;
}

void BlockGenerator::push(const std::string& reg)
{
	m_sp += 8;
	m_asm << "push " << reg << "\n";
}

void BlockGenerator::pop(const std::string& reg)
{
	m_sp -= 8;
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
								size_t srcSize,
								const std::string& src,
								size_t destSize,
								bool isUnsigned)
{
	std::stringstream code;
	if (destSize == 0)
		destSize = srcSize;
	// If rhs is a byte or a word, use movzx/movsx to extend it to a dword or qword, otherwise - regular mov

	if (destSize > srcSize && destSize - srcSize >= 2)
	{
		code << (isUnsigned ? "movzx " : "movsx ");
	}
	else
		code << "mov ";
	// If rhs is a byte or a word, use the data size of lhs (because movzx/movsx already extended it)
	code << reg(dest, destSize) << ", " << src << "\n";
	return code.str();
}

std::string BlockGenerator::mov(BlockGenerator::Reg dest, BlockGenerator::Reg src, size_t srcSize, bool sign)
{
	std::stringstream code;
	code << "mov " << reg(dest, srcSize) << ", " << reg(src, srcSize) << "\n";
	return code.str();
}

std::string BlockGenerator::mov(const std::string& dest,
								size_t srcSize,
								BlockGenerator::Reg src,
								size_t destSize,
								bool isUnsigned)
{
	std::stringstream code;
	if (destSize == 0)
		destSize = srcSize;
	if (destSize == 8 && srcSize == 4)
		code << "cdqe\n";
	code << "mov ";
	code << dest << ", " << reg(src, destSize) << "\n";
	return code.str();
}

std::string BlockGenerator::mov(const std::string& dest,
								size_t srcSize,
								const std::string& src,
								size_t destSize,
								bool isUnsigned)
{
	std::stringstream code;
	if (destSize == 0)
		destSize = srcSize;

	if (destSize == 0)
		destSize = srcSize;
	// If rhs is a byte or a word, use movzx/movsx to extend it to a dword or qword, otherwise - regular mov
	if (destSize - srcSize >= 2)
		code << (isUnsigned ? "movzx " : "movsx ");
	else
		code << "mov ";

	code << dest << ", " << src << "\n";
	return code.str();
}

std::string BlockGenerator::mov(BlockGenerator::Reg dest, size_t srcSize, long src, size_t destSize, bool isUnsigned)
{
	return mov(dest, srcSize, std::to_string(src), destSize, isUnsigned);
}
std::string BlockGenerator::mov(const std::string& dest, size_t srcSize, long src, size_t destSize, bool isUnsigned)
{
	return mov(dest, srcSize, std::to_string(src), destSize, isUnsigned);
}

std::string BlockGenerator::offset(size_t offset,
								   size_t dataSize,
								   BlockGenerator::Reg base,
								   size_t regSize,
								   bool positive)
{

	std::stringstream code;
	code << bytes_to_data_size(dataSize);
	code << " [" << reg(base, regSize) << (positive ? "+" : "-") << offset << "]";
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
	BlockGenerator body{ block, m_stack, m_bp_offset, m_label_index, m_local_labels, m_program_ast, m_flags };
	body.GenerateBlock();
	m_early_returns = body.Returns();
	m_label_index = body.label_index();
	m_local_labels = body.labels();
	m_bp_offset = body.BpOffset();
	m_sp = body.StackPointer();
	m_bp = body.BasePointer();
	m_asm << body.Asm();
}

void BlockGenerator::assert_ident_initialised(const Identifier* lhsId)
{
	auto it = m_stack.find(lhsId->Name());
	if (it == m_stack.end())
		error("Codegen Error: Use of undeclared identifier '{}'. This is a codegen "
			  "Error and must be fixed in the parser.", lhsId->Name());
	else if (it->second.second == 0)
		error("Codegen Error: Use of uninitialised identifier '{}'. This is a codegen"
			  "Error and must be fixed in the parser", lhsId->Name());
}

void BlockGenerator::assert_ident_declared(const Identifier* lhsId)
{
	auto it = m_stack.find(lhsId->Name());
	if (it == m_stack.end())
		error("Codegen Error: Use of undeclared identifier '{}'. This is a codegen "
			  "Error and must be fixed in the parser.", lhsId->Name());

}

} // alx