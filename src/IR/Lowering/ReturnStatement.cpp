/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-29.
//

#include "../Ir.h"

namespace alx::ir {

void Function::ResolveReturnSentinels()
{
	if (MultipleReturns) {
		auto alloca = AllocaInst{ .Type = std::make_shared<Types>(ReturnType) };
		auto retVal = std::make_shared<Variable>(
			Variable{ .Name = "retval", .Attributes = { AlignAttribute{ alloca.Size() } }, .Allocation = alloca });
		Blocks.front().Body.insert(Blocks.front().Body.begin(), *retVal);

		LogicalBlock ret{ { "return" } };
		{
			auto load = LoadInst{ .Type = ReturnType, .Ptr = retVal, .Alignment = { retVal->Size() } };
			auto temporary = std::make_shared<Variable>(Variable{ .Name = GetNewUnnamedTemporary(),
																  .Attributes = { load.Alignment },
																  .Allocation = load,
																  .IsTemporary = true });
			auto retInst = ReturnInst{ .Value = temporary };
			ret.Body.emplace_back(*temporary);
			ret.Body.emplace_back(retInst);
		}
		
		// Loop over all blocks and replace return instructions with store instructions
		for (auto& block : Blocks) {
			for (auto& body : block.Body) {
				if (std::holds_alternative<ReturnInst>(body)) {
					auto& retInst = std::get<ReturnInst>(body);
					StoreInst store{ .Value = retInst.Value,
									 .Ptr = retVal,
									 .Alignment = { retVal->Size() } }; // FIXME: Do the ze, se, etc.
					body = store;
					bool found = false;
					for (auto& body2 : block.Body) {
						if (std::holds_alternative<BranchInst>(body2)) {
							auto& branchInst = std::get<BranchInst>(body2);
							if (branchInst.TrueLabel.Name != ret.Label.Name)
							{
								branchInst.TrueLabel = ret.Label;
								found = true;
							}
						}
					}
					if (!found) {
						BranchInst branchInst{ .TrueLabel = ret.Label };
						block.Body.emplace_back(branchInst);
					}
				}
			}
		}
		AppendBlock(ret);
	}
}

void IR::generate_return_statement(const ReturnStatement& astNode, Function& function)
{
	LogicalBlock ret{ { "return" } };

	if (astNode.Argument()->class_name() == "NumberLiteral") {
		auto& numLit = static_cast<NumberLiteral&>(*astNode.Argument());
		auto value = numLit;
		if (isIntegerLiteral(value.Type())) {
			Constant returnValue{ .Type = IntType{ size_of(value.Type()) }, .Value = value.AsInt() };
			ret.Body.emplace_back(ReturnInst{ returnValue });
		}
		else if (!isIntegerLiteral(value.Type()) && isNumberLiteral(value.Type())) {
			SingleValueType type;
			if (value.Type() == TokenType::T_FLOAT_L)
				type = SingleValueType::Float;
			else if (value.Type() == TokenType::T_DOUBLE_L)
				type = SingleValueType::Double;
			Constant returnValue{ .Type = type, .Value = value.AsDouble() };
			ret.Body.emplace_back(ReturnInst{ returnValue });
		}
		else
			println(Colour::Red, "Unknown number type: {;255;255;255}", token_to_string(value.Type()));
	}
	else if (astNode.Argument()->class_name() == "BinaryExpression") {
		auto& binExpr = static_cast<BinaryExpression&>(*astNode.Argument());
		if (binExpr.Constexpr()) {
			auto value = binExpr.Evaluate();
			if (isIntegerLiteral(value->Type())) {
				Constant returnValue{ .Type = IntType{ size_of(value->Type()) }, .Value = value->AsInt() };
				ret.Body.emplace_back(ReturnInst{ returnValue });
			}
			else if (!isIntegerLiteral(value->Type()) && isNumberLiteral(value->Type())) {
				SingleValueType type;
				if (value->Type() == TokenType::T_FLOAT_L)
					type = SingleValueType::Float;
				else if (value->Type() == TokenType::T_DOUBLE_L)
					type = SingleValueType::Double;

				Constant returnValue{ .Type = type, .Value = value->AsDouble() };
				ret.Body.emplace_back(ReturnInst{ returnValue });
			}
			else
				println(Colour::Red, "Unknown number type: {;255;255;255}", token_to_string(value->Type()));
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
	else if (astNode.Argument()->class_name() == "UnaryExpression") {
		auto result = generate_unary_expression(static_cast<UnaryExpression&>(*astNode.Argument()), function);
		MUST(result.has_value());
		ret.Body.emplace_back(ReturnInst{ result.value() });
	}
	else {
		ASSERT_NOT_IMPLEMENTED_MSG(getFormatted("Unknown return type: {}", astNode.Argument()->class_name()));
	}

	function.Blocks.back().Body.insert(std::end(function.Blocks.back().Body), std::begin(ret.Body), std::end(ret.Body));

	if (function.Returns)
		function.MultipleReturns = true;
	function.Returns = true;
}

}