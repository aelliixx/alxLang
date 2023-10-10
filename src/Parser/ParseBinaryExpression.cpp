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
std::optional<ValueExpression> Parser::parse_binary_operation(ValueExpression lhs, int precedence)
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

		std::optional<ValueExpression> rhs = parse_term();
		if (!rhs)
			return {};

		int nextPrecedence = get_binary_op_precedence(peek().value());
		if (tokenPrecedence < nextPrecedence)
		{
			rhs = parse_binary_operation(std::move(rhs.value()), tokenPrecedence + 1);
			if (!rhs.has_value())
				return {};
		}
		
		// FIXME: Add the errors back with visitor pattern
//		if (lhs->class_name() == "NumberLiteral" && op.Type == TokenType::T_EQ)
//			m_error->Error(op.LineNumber, op.ColumnNumber, op.PosNumber, "Expression is not assignable");
//		if (lhs->class_name() == "BinaryExpression" && op.Type == TokenType::T_EQ)
//			if (static_cast<BinaryExpression&>(*lhs).Rhs()->class_name() == "NumberLiteral")
//				m_error->Error(op.LineNumber, op.ColumnNumber, op.PosNumber, "Expression is not assignable");
		lhs = std::make_shared<BinaryExpression>(std::move(lhs), std::move(rhs.value()), op.Type);
	}
	ASSERT_NOT_REACHABLE();
}
#pragma clang diagnostic pop

}