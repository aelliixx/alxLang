/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-08.
//

#include "Ir.h"

#include <utility>
#include "../libs/Println.h"
#include "Instructions.h"

namespace alx::ir {

void IR::Generate()
{
	println("Generating IR:");
	for (const auto& node : m_ast)
	{
		if (node->class_name() == "FunctionDeclaration")
		{
			auto& func = static_cast<FunctionDeclaration&>(*node);
			generate_function(func);
		}
		else
		{
			println(Colour::Red, "Unknown node type: {;255;255;255}", node->class_name());
		}
	}
	println("Done generating IR");
}

Values IR::NumberLiteralToValue(const NumberLiteral& literal, size_t size)
{
	const auto& numLit = static_cast<const NumberLiteral&>(literal);
	if (isIntegerLiteral(numLit.Type()))
	{
		return Constant{ .Type = IntType{ size }, .Value = numLit.AsInt() };
	}
	else if (!isIntegerLiteral(numLit.Type()) && isNumberLiteral(numLit.Type()))
	{
		SingleValueType type;
		if (numLit.Type() == TokenType::T_FLOAT_L)
			type = SingleValueType::Float;
		else if (numLit.Type() == TokenType::T_DOUBLE_L)
			type = SingleValueType::Double;
		return Constant{ .Type = type, .Value = numLit.AsDouble() };
	}
	else
	{
		println(Colour::Red, "Unknown number type: {;255;255;255}",
				token_to_string(numLit.Type()));
		ASSERT_NOT_REACHABLE();
	}
}


}
