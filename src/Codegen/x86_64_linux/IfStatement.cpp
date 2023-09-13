/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-12.
//

#include "BlockGenerator.h"

namespace alx {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
void BlockGenerator::generate_if_statement(ASTNode* node, const std::optional<std::string>& exit_label)
{
	auto statement = dynamic_cast<IfStatement*>(node);
	auto condition = statement->Condition();
	auto exit_label_actual = exit_label.has_value() ? exit_label.value() : generate_local_label(statement);
	TokenType jump_type = TokenType::T_EQEQ;

	auto jump = [this](TokenType jump_type)
	{
	  switch (jump_type)
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
		auto cond_num = dynamic_cast<NumberLiteral*>(condition);
		if (cond_num->AsInt())
		{
			generate_body(statement->Body());
			return;
		}
	}
	else if (condition->class_name() == "BinaryExpression")
	{
		auto cond_bin_exp = dynamic_cast<BinaryExpression*>(condition);
		if (cond_bin_exp->Constexpr() && cond_bin_exp->Evaluate()->AsInt())
		{
			generate_body(statement->Body());
			return;
		}
		switch (cond_bin_exp->Operator())
		{
		case TokenType::T_EQEQ:
			jump_type = TokenType::T_NOT_EQ; // The '==' token produces a jne instruction
		case TokenType::T_NOT_EQ:
			generate_binary_expression(cond_bin_exp);
			break;
		case TokenType::T_LT:
		case TokenType::T_LTE:
			generate_binary_expression(cond_bin_exp);
			jump_type = TokenType::T_GT;
			break;
		case TokenType::T_GT:
		case TokenType::T_GTE:
			jump_type = TokenType::T_LTE;
			generate_binary_expression(cond_bin_exp);
			break;
		case TokenType::T_PLUS:
		case TokenType::T_MINUS:
		case TokenType::T_STAR:
		case TokenType::T_FWD_SLASH:
		case TokenType::T_POW:
		case TokenType::T_EQ:
		case TokenType::T_MOD:
		case TokenType::T_INT_DIV:
		case TokenType::T_SUB:
		case TokenType::T_ADD:
		case TokenType::T_ADD_EQ:
		case TokenType::T_SUB_EQ:
		case TokenType::T_MULT_EQ:
		case TokenType::T_DIV_EQ:
		case TokenType::T_MOD_EQ:
		case TokenType::T_POW_EQ:
			generate_binary_expression(cond_bin_exp);
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
		auto ident = dynamic_cast<Identifier*>(condition);
		m_asm << "cmp BYTE " << offset(m_stack[ident->Name()].second)
			  << ", 0\n"; // FIXME: Get the size of the identifier

	}

	if (statement->HasAlternate())
		m_asm << jump(jump_type) << generate_local_label(statement->GetAlternate()) << '\n';
	else
		m_asm << jump(jump_type) << exit_label_actual << '\n';

	
	generate_body(statement->Body());

	if (statement->HasAlternate())
	{
		m_asm << "jmp " << exit_label_actual << "\n";
		m_asm << generate_local_label(statement->GetAlternate()) << ":\n";
	}
	else
	{
		m_asm << exit_label_actual << ":\n";
		return;
	}

	auto alternate = statement->GetAlternate();
	if (alternate->class_name() == "IfStatement")
		generate_if_statement(alternate, exit_label_actual);
	else if (alternate->class_name() == "BlockStatement")
	{
		auto alternate_block = dynamic_cast<BlockStatement*>(alternate);
		generate_body(*alternate_block);
		m_asm << exit_label_actual << ":\n";
	}
}

#pragma clang diagnostic pop

}