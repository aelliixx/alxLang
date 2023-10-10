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

void BlockGenerator::generate_variables(const VariableDeclaration& node)
{
//	auto variable = static_cast<VariableDeclaration*>(node.get());
//	auto size = size_of(node->type);
//	auto value = variable->Value();

	struct TypeValue
	{
		NumberLiteral& numberLiteral;
		long operator()(const RefPtr<Identifier>& type) {}
		long operator()(TokenType tokenType) const
		{
			switch (tokenType)
			{
			case TokenType::T_CHAR:
			case TokenType::T_SHORT:
			case TokenType::T_INT:
			case TokenType::T_LONG:
				return numberLiteral.AsInt();
			case TokenType::T_BOOL:
				return numberLiteral.AsBoolNum();
			case TokenType::T_FLOAT:
			case TokenType::T_DOUBLE:
			case TokenType::T_STRING:
			default:
			ASSERT_NOT_REACHABLE();
			}
		}
	};

	struct IsUnsigned
	{
		bool operator()(const RefPtr<Identifier>& type) { return false; }
		bool operator()(TokenType tokenType) { return isUnsigned(tokenType); }
	};

	struct Type
	{
		TokenType operator()(const RefPtr<Identifier>& type) { return TokenType::T_IDENTIFIER; }
		TokenType operator()(TokenType tokenType) { return tokenType; }
	};

	struct ValueVisitor
	{
		BlockGenerator& gen;
		size_t size;
		TypeExpression type;
		size_t lhsPtr;
		void operator()(const RefPtr<Identifier>& rhs)
		{
			auto rhsStack = gen.m_stack[rhs->Name()];
			auto lhsType = std::visit(Type{}, type);
			auto rhsType = std::visit(Type{}, gen.m_stack_types[rhs->Name()]);
			if (rhsType != TokenType::T_BOOL &&
				lhsType != TokenType::T_BOOL)
			{

				gen.m_asm << "cmp " << offset(rhsStack.first, rhsStack.second) << ", 0\n";
				gen.m_asm << "setne " << reg(Reg::rax, size) << "\n";
				gen.m_asm << mov(offset(lhsPtr, size), size, Reg::rax);
				return;
			}
			else if (size_of(gen.m_stack_types[rhs->Name()]) <= 2
				&& (lhsType == TokenType::T_CHAR || lhsType == TokenType::T_BOOL || lhsType == TokenType::T_SHORT))
			{
				if (lhsType == TokenType::T_SHORT
					&& !(rhsType == TokenType::T_BOOL || rhsType == TokenType::T_SHORT))
					gen.m_asm
						<< mov(reg(Reg::rax, 2), rhsStack.second, offset(rhsStack.first, rhsStack.second), 4, true);
				else
					gen.m_asm << mov(Reg::rax, rhsStack.second, offset(rhsStack.first, rhsStack.second), 4, true);
				gen.m_asm << mov(offset(lhsPtr, size), size, Reg::rax);
				return;
			}
			gen.generate_assignment_ident(*rhs, size, isUnsigned(rhsType));
		}
		void operator()(const RefPtr<MemberExpression>& rhs)
		{
			auto rhsVar = gen.m_stack.at(rhs->Object().Name() + "::" + rhs->Member().Name());
			gen.m_asm << mov(Reg::rax, rhsVar.second, offset(rhsVar.first, rhsVar.second));
			gen.m_asm << mov(offset(lhsPtr, size), size, Reg::rax);
		}
		void operator()(const RefPtr<NumberLiteral>& rhs)
		{

			auto rhsVal = std::visit(TypeValue{ .numberLiteral = *rhs }, type);
			auto isUnsigned = std::visit(IsUnsigned{}, type);
			gen.m_asm << alx::BlockGenerator::mov
				(offset(gen.m_bp_offset, size), size, rhsVal, size, isUnsigned);
		}
		void operator()(const RefPtr<StringLiteral>&)
		{
			ASSERT_NOT_IMPLEMENTED();
		}
		void operator()(const RefPtr<BinaryExpression>& rhs)
		{
			if (rhs->Constexpr())
			{
				gen.m_asm << mov(offset(gen.m_bp_offset, size), size, rhs->Evaluate()->AsInt());
				return;
			}
			Context context = { .LhsSize = size, .AssignmentChain = true };
			gen.generate_binary_expression(rhs.get(), context);
			gen.m_asm << mov(offset(lhsPtr, size), size, reg(Reg::rax, size));
		}
		void operator()(const RefPtr<UnaryExpression>& rhs)
		{
			gen.generate_unary_expression(rhs.get());
			gen.m_asm << mov(offset(lhsPtr, size), size, reg(Reg::rax, size));
		}
	};
	auto size = size_of(node->SingleValueType());
	add_to_stack(node->Name(), size, node->SingleValueType());
	auto lhsPtr = m_stack[node->Name()].first;
	ValueVisitor visitor{ .gen = *this, .size = size, .type = node->SingleValueType(), .lhsPtr = lhsPtr };
	std::visit(visitor, node->Value());
}

}