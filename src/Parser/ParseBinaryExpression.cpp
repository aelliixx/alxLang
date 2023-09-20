/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include "Parser.h"
#include "../libs/ErrorHandler.h"
namespace alx {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
std::unique_ptr<Expression> Parser::parse_binary_operation(std::unique_ptr<Expression> lhs, int precedence)
{
	while (peek().has_value())
	{
		if (peek().value().Type == TokenType::T_SEMI || !isBinaryOp(peek().value().Type))
		{
			return lhs;
		}
		auto op = consume();
		if (!isBinaryOp(op.Type))
			return lhs;
		int tokenPrecedence = get_binary_op_precedence(op);
		if (tokenPrecedence < precedence)
			return lhs;

		auto rhs = parse_term();
		if (!rhs)
			return nullptr;

		int nextPrecedence = get_binary_op_precedence(peek().value());
		if (tokenPrecedence < nextPrecedence)
		{
			rhs = parse_binary_operation(std::move(rhs), tokenPrecedence + 1);
			if (!rhs)
				return nullptr;
		}
		if (lhs->class_name() == "NumberLiteral" && op.Type == TokenType::T_EQ)
			m_error->Error(op.LineNumber, op.ColumnNumber, op.PosNumber, "Expression is not assignable");
		if (lhs->class_name() == "BinaryExpression" && op.Type == TokenType::T_EQ)
			if (static_cast<BinaryExpression&>(*lhs).Rhs()->class_name() == "NumberLiteral")
				m_error->Error(op.LineNumber, op.ColumnNumber, op.PosNumber, "Expression is not assignable");
		lhs = std::make_unique<BinaryExpression>(std::move(lhs), std::move(rhs), op.Type);
	}
	ASSERT_NOT_REACHABLE();
}
#pragma clang diagnostic pop

}