/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include "Parser.h"
#include "../libs/Utils.h"
namespace alx {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
std::unique_ptr<Expression> Parser::parse_binary_operation(std::unique_ptr<Expression> lhs, int precedence)
{
	while (peek().has_value())
	{
		if (peek().value().Type == TokenType::T_SEMI || !is_binary_op(peek().value().Type)) {
			return lhs;
		}
		auto op = consume();
		if (!is_binary_op(op.Type))
			return lhs;
		int token_precedence = get_binary_op_precedence(op);
		if (token_precedence < precedence)
			return lhs;
		
		auto rhs = parse_term();
		if (!rhs)
			return nullptr;
		
		int next_precedence = get_binary_op_precedence(peek().value());
		if (token_precedence < next_precedence) {
			rhs = parse_binary_operation(std::move(rhs), token_precedence + 1);
			if (!rhs)
				return nullptr;
		}
		
		lhs = std::make_unique<BinaryExpression>(std::move(lhs), std::move(rhs), op.Type);
	}
	ASSERT_NOT_REACHABLE();
}
#pragma clang diagnostic pop

}