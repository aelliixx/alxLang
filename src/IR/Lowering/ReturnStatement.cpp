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
	else if (astNode.Argument()->class_name() == "Identifier") {
		auto& ident = static_cast<Identifier&>(*astNode.Argument());
		auto rhsVariable = function.FindVariableByIdentifier(ident.Name());
		MUST(rhsVariable);
		LoadInst load{ .Type = *(std::get<AllocaInst>(rhsVariable->Allocation).Type),
					   .Ptr = rhsVariable,
					   .Alignment = { std::get<AllocaInst>(rhsVariable->Allocation).Size() } };
		auto temporary = std::make_shared<Variable>(
			Variable{ .Name = function.GetNewUnnamedTemporary(),
					  .Attributes = { AlignAttribute{ std::get<AllocaInst>(rhsVariable->Allocation).Size() } },
					  .Allocation = load,
					  .IsTemporary = true });
		ret.Body.emplace_back(*temporary);
		ret.Body.emplace_back(ReturnInst{ temporary });
	}
	else if (astNode.Argument()->class_name() == "UnaryExpression")
	{
		auto result = generate_unary_expression(static_cast<UnaryExpression&>(*astNode.Argument()), function);
		MUST(result.has_value());
		ret.Body.emplace_back(ReturnInst{ result.value() });
	}
	else
	{
		ASSERT_NOT_IMPLEMENTED_MSG(getFormatted("Unknown return type: {}", astNode.Argument()->class_name()));
	}

	if (hasReturned)
		function.Blocks.emplace_back(std::move(ret));
	else
		function.Blocks.back().Body.insert(std::end(function.Blocks.back().Body),
									   std::begin(ret.Body),
									   std::end(ret.Body));
	hasReturned = true;
}

}