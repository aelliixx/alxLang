/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include "Parser.h"
namespace alx {
std::unique_ptr<ReturnStatement> Parser::parse_return_statement()
{
	consume(); // Eat 'return'
	auto expr = parse_expression();
	consume(); // Eat ';'
	return std::make_unique<ReturnStatement>(std::move(expr));
}
};
