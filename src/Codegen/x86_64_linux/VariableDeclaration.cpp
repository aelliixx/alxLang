/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-09.
//

#include "../../Utils/Utils.h"
#include "BlockGenerator.h"
#include "../../libs/Println.h"
#include "../../libs/ErrorHandler.h"

namespace alx {

void BlockGenerator::generate_variables(const std::unique_ptr<ASTNode>& node)
{
	auto variable = static_cast<VariableDeclaration*>(node.get());
	auto type = variable->TypeAsPrimitive();
	auto size = size_of(type);
	auto value = variable->Value();
	add_to_stack(variable->Name(), size, type);
	auto lhsPtr = m_stack[variable->Name()].first;

	if (type == TokenType::T_VOID)
		error("Cannot declare variable of variable 'void'");
	if (!value)
	{
		if (!variable->Ident().Assignable())
			assert("Must initialise constant values");
		return;
	}
	if (value->class_name() == "NumberLiteral")
	{
		auto rhs = static_cast<NumberLiteral*>(value);
		auto rhsVal = type == TokenType::T_BOOL ? rhs->AsBoolNum() : rhs->AsInt();
		m_asm << mov(offset(m_bp_offset, size), size, rhsVal, size, isUnsigned(rhs->Type()));
		return;
	}
	else if (value->class_name() == "Identifier")
	{
		// - Get the underlying identifier of the rhs
		auto rhs = static_cast<Identifier&>(*value);
		auto rhsStack = m_stack[rhs.Name()];
			
		if (m_stack_types[rhs.Name()] != TokenType::T_BOOL && type == TokenType::T_BOOL)
		{
			m_asm << "cmp " << offset(rhsStack.first, rhsStack.second) << ", 0\n";
			m_asm << "setne " << reg(Reg::rax, size) << "\n";
			m_asm << mov(offset(lhsPtr, size), size, Reg::rax);
			return;
		}
		else if (size_of(m_stack_types[rhs.Name()]) <= 2
			&& (type == TokenType::T_CHAR || type == TokenType::T_BOOL || type == TokenType::T_SHORT))
		{
			if (type == TokenType::T_SHORT && !(m_stack_types[rhs.Name()] == TokenType::T_BOOL || m_stack_types[rhs.Name()] == TokenType::T_SHORT))
				m_asm << mov(reg(Reg::rax, 2), rhsStack.second, offset(rhsStack.first, rhsStack.second), 4, true);
			else
				m_asm << mov(Reg::rax, rhsStack.second, offset(rhsStack.first, rhsStack.second), 4, true);
			m_asm << mov(offset(lhsPtr, size), size, Reg::rax);
			return;
		}
		generate_assignment_ident(rhs, size, isUnsigned(m_stack_types[rhs.Name()]));
		return;
	}
	else if (value->class_name() == "BinaryExpression")
	{
		auto rhs = static_cast<BinaryExpression*>(value);
		if (rhs->Constexpr())
		{
			m_asm << mov(offset(m_bp_offset, size), size, rhs->Evaluate()->AsInt());
			return;
		}
		Context context = { .LhsSize = size, .AssignmentChain = true };
		generate_binary_expression(rhs, context);
		m_asm << mov(offset(lhsPtr, size), size, reg(Reg::rax, size));
		return;
	}
	else if (value->class_name() == "UnaryExpression")
	{
		generate_unary_expression(variable->Value());
		m_asm << mov(offset(lhsPtr, size), size, reg(Reg::rax, size));
		return;
	}
	else if (value->class_name() == "MemberExpression")
	{
		const auto& rhs = static_cast<MemberExpression&>(*value);
		auto rhsVar = m_stack.at(rhs.Object().Name() + "::" + rhs.Member().Name());
		m_asm << mov(Reg::rax, rhsVar.second, offset(rhsVar.first, rhsVar.second));
		m_asm << mov(offset(lhsPtr, size), size, Reg::rax);
		return;
	}

	ASSERT_NOT_REACHABLE();
}

}