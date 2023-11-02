/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-12.
//

#include "Parser.h"

namespace alx {
std::unique_ptr<UnaryExpression> Parser::parse_unary_expression()
{
	auto op = consume();
	auto rhs = parse_term();
	
	if (op.Type == TokenType::T_SUB || op.Type == TokenType::T_ADD)
	{
		if (!rhs || rhs->class_name() != "Identifier") // FIXME: allow member expressions
		{
			// TODO: get variable from m_variables and check if it's const
			m_error->Error(op.LineNumber, op.ColumnNumber + 1, op.PosNumber + 1, "Expression is not assignable");
			return nullptr;
		}
		else if (rhs->class_name() == "Identifier") {
			auto ident = static_cast<Identifier*>(rhs.get());
			if (!ident->Assignable()) {
				m_error->Error(op.LineNumber,
							   op.ColumnNumber + 1,
							   op.PosNumber + 1,
							   "Cannot assign to constant '{}'",
							   ident->Name());
				return nullptr;
			}
		}
	}
	if (!rhs)
		return nullptr;
	return std::make_unique<UnaryExpression>(std::move(rhs), op.Type);
}
}