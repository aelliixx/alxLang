/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-12.
//

#include "BlockGenerator.h"

namespace alx {

#ifdef __clang__
#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
#endif
void BlockGenerator::generate_if_statement(ASTNode* node, const std::optional<std::string>& exitLabel)
{
	auto statement = static_cast<IfStatement*>(node);
	auto condition = statement->Condition();
	auto exitLabelActual = exitLabel.has_value() ? exitLabel.value() : generate_local_label(statement);

	if (condition->class_name() == "NumberLiteral")
	{
		auto condNum = static_cast<NumberLiteral*>(condition);
		if (condNum->AsInt())
		{
			generate_body(statement->Body());
			return;
		}
	}
	else if (condition->class_name() == "BinaryExpression")
	{
		auto condBinExp = static_cast<BinaryExpression*>(condition);
		if (condBinExp->Constexpr() && condBinExp->Evaluate()->AsInt())
		{
			generate_body(statement->Body());
			return;
		}
	}

	generate_branch(condition, statement, exitLabelActual, true);
	generate_body(statement->Body());

	if (statement->HasAlternate())
	{
		m_asm << "jmp " << exitLabelActual << "\n";
		m_asm << generate_local_label(statement->GetAlternate()) << ":\n";
	}
	else
	{
		m_asm << exitLabelActual << ":\n";
		return;
	}

	auto alternate = statement->GetAlternate();
	if (alternate->class_name() == "IfStatement")
		generate_if_statement(alternate, exitLabelActual);
	else if (alternate->class_name() == "BlockStatement")
	{
		auto alternateBlock = static_cast<BlockStatement*>(alternate);
		generate_body(*alternateBlock);
		m_asm << exitLabelActual << ":\n";
	}
}

void BlockGenerator::generate_branch(Expression* condition,
									 const std::optional<IfStatement*>& statement,
									 const std::optional<std::string>& exitLabel,
									 bool invertComparisons)
{
	TokenType jumpType = TokenType::T_EQEQ;
	auto exitLabelActual =
		exitLabel.has_value() || !statement.has_value() ? exitLabel.value() : generate_local_label(statement.value());

	auto jump = [this](TokenType jumpType)
	{
	  switch (jumpType)
	  {

	  case TokenType::T_LT:
		  return "jl ";
	  case TokenType::T_GT:
		  return "jg ";
	  case TokenType::T_LTE:
		  return "jle ";
	  case TokenType::T_GTE:
		  return "jge ";
	  case TokenType::T_EQEQ:
		  return "je ";
	  case TokenType::T_NOT_EQ:
		  return "jne ";
	  default:
	  ASSERT_NOT_REACHABLE();
	  }
	};

	if (condition->class_name() == "NumberLiteral")
	{
//		auto condNum = static_cast<NumberLiteral*>(condition);
		ASSERT_NOT_IMPLEMENTED();
	}
	else if (condition->class_name() == "BinaryExpression")
	{
		auto condBinExp = static_cast<BinaryExpression*>(condition);

		generate_binary_expression(condBinExp);
		switch (condBinExp->Operator())
		{
		case TokenType::T_EQEQ:
			jumpType = invertComparisons ? TokenType::T_NOT_EQ : TokenType::T_EQEQ;
			break;
		case TokenType::T_NOT_EQ:
			jumpType = invertComparisons ? TokenType::T_EQEQ : TokenType::T_NOT_EQ;
			break;
		case TokenType::T_LT:
		case TokenType::T_LTE:
			jumpType = invertComparisons ? TokenType::T_GT : TokenType::T_LTE;
			break;
		case TokenType::T_GT:
		case TokenType::T_GTE:
			jumpType = invertComparisons ? TokenType::T_LTE : TokenType::T_GT;
			break;
		case TokenType::T_PLUS:
		case TokenType::T_MINUS:
		case TokenType::T_STAR:
		case TokenType::T_FWD_SLASH:
		case TokenType::T_POW:
		case TokenType::T_EQ:
		case TokenType::T_MOD:
		case TokenType::T_COLON:
		case TokenType::T_SUB:
		case TokenType::T_ADD:
		case TokenType::T_ADD_EQ:
		case TokenType::T_SUB_EQ:
		case TokenType::T_MULT_EQ:
		case TokenType::T_DIV_EQ:
		case TokenType::T_MOD_EQ:
		case TokenType::T_POW_EQ:
			m_asm << "cmp eax, 0\n";
			break;
			ASSERT_NOT_IMPLEMENTED();
		default:
		ASSERT_NOT_REACHABLE();
		}
	}
	else if (condition->class_name() == "Identifier")
	{
		// TODO: Check if it's nullptr
		auto ident = static_cast<Identifier*>(condition);
		m_asm << "cmp BYTE " << offset(m_stack[ident->Name()].second, 8)
			  << ", 0\n"; // FIXME: Get the size of the identifier

	}

	if (!statement.has_value())
	{
		if (exitLabel.has_value())
			m_asm << jump(jumpType) << exitLabel.value() << "\n";
		return;
	}

	if (statement.value()->HasAlternate())
		m_asm << jump(jumpType) << generate_local_label(statement.value()->GetAlternate()) << '\n';
	else
		m_asm << jump(jumpType) << exitLabelActual << '\n';
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

}