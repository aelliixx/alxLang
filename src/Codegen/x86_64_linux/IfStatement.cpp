/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-12.
//

#include "BlockGenerator.h"

namespace alx {

void BlockGenerator::generate_if_statement(const std::unique_ptr<ASTNode>& node)
{
	auto statement = dynamic_cast<IfStatement*>(node.get());
	auto condition = statement->Condition();
	if (condition->class_name() == "Identifier")
	{

	}
	if (condition->class_name() == "NumberLiteral")
	{
		auto cond_num = dynamic_cast<NumberLiteral*>(condition);
		if (cond_num->AsInt())
		{
			BlockGenerator body{ statement->Body(), m_stack, bp };
			body.GenerateBlock();
			m_asm << body.Asm();
		}
	}
	else if (condition->class_name() == "BinaryExpression")
	{
		auto cond_bin_exp = dynamic_cast<BinaryExpression*>(condition);
		if (cond_bin_exp->Constexpr() && cond_bin_exp->Evaluate()->AsInt())
		{
			BlockGenerator body{ statement->Body(), m_stack, bp };
			body.GenerateBlock();
			m_asm << body.Asm();
		}
	}
	else if (condition->class_name() == "Identifier")
	{
		// TODO: Check if it's nullptr
		auto ident = dynamic_cast<Identifier*>(condition);
		m_asm << "cmp BYTE " << offset(m_stack[ident->Name()].second)
			  << ", 0\n"; // FIXME: Get the size of the identifier
		m_asm << "je " << generate_local_label(statement) << '\n';
		BlockGenerator body{ statement->Body(), m_stack, bp };
		body.GenerateBlock();
		m_asm << body.Asm();
		m_asm << generate_local_label(statement) << ":\n";
	}

}

}