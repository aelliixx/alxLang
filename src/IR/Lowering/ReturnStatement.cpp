/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-29.
//

#include "../Ir.h"

namespace alx::ir {

void IR::generate_return_statement(const ReturnStatement& astNode, Function& function, bool hasReturned)
{
	LogicalBlock ret{{ "return" }};

	if (astNode.Argument()->class_name() == "NumberLiteral")
	{
		auto& numLit = static_cast<NumberLiteral&>(*astNode.Argument());
		auto value = numLit;
		if (isIntegerLiteral(value.Type()))
		{
			Constant returnValue{ .Type = IntType{ size_of(value.Type()) }, .Value = value.AsInt() };
			ret.Body.emplace_back(ReturnInst{ returnValue });
		}
		else if (!isIntegerLiteral(value.Type()) && isNumberLiteral(value.Type()))
		{
			SingleValueType type;
			if (value.Type() == TokenType::T_FLOAT_L)
				type = SingleValueType::Float;
			else if (value.Type() == TokenType::T_DOUBLE_L)
				type = SingleValueType::Double;
			Constant returnValue{ .Type = type, .Value = value.AsDouble() };
			ret.Body.emplace_back(ReturnInst{ returnValue });
		}
		else
			println(Colour::Red, "Unknown number type: {;255;255;255}",
					token_to_string(value.Type()));
	}
	else if (astNode.Argument()->class_name() == "BinaryExpression")
	{
		auto& binExpr = static_cast<BinaryExpression&>(*astNode.Argument());
		if (binExpr.Constexpr())
		{
			auto value = binExpr.Evaluate();
			if (isIntegerLiteral(value->Type()))
			{
				Constant returnValue{ .Type = IntType{ size_of(value->Type()) }, .Value = value->AsInt() };
				ret.Body.emplace_back(ReturnInst{ returnValue });
			}
			else if (!isIntegerLiteral(value->Type()) && isNumberLiteral(value->Type()))
			{
				SingleValueType type;
				if (value->Type() == TokenType::T_FLOAT_L)
					type = SingleValueType::Float;
				else if (value->Type() == TokenType::T_DOUBLE_L)
					type = SingleValueType::Double;

				Constant returnValue{ .Type = type, .Value = value->AsDouble() };
				ret.Body.emplace_back(ReturnInst{ returnValue });
			}
			else
				println(Colour::Red, "Unknown number type: {;255;255;255}",
						token_to_string(value->Type()));

		}
	}
	else
	{
		println(Colour::Red, "Unknown node type: {;255;255;255}", astNode.Argument()->class_name());
		ASSERT_NOT_REACHABLE();
	}

	if (hasReturned)
		function.Blocks.emplace_back(std::move(ret));
	else
		function.Blocks[0].Body.insert(std::end(function.Blocks[0].Body),
									   std::begin(ret.Body),
									   std::end(ret.Body));
}

}