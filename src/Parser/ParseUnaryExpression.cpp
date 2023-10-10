/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-12.
//

#include "Parser.h"

namespace alx {
RefPtr<UnaryExpression> Parser::parse_unary_expression()
{
	auto op = consume();
	auto rhs = parse_term();
	if (!rhs)
		return nullptr;
	return std::make_shared<UnaryExpression>(std::make_shared<ValueExpression>(rhs), op.Type);
}
}