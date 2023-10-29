/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-29.
//

#include "../Ir.h"

namespace alx::ir {

static void generateBinEq(BinaryExpression& astNode, Function& function)
{
	auto lhs = astNode.Lhs();
	auto rhs = astNode.Rhs();
	if (lhs->class_name() == "Identifier")
	{
		auto astIdentifier = static_cast<const Identifier&>(*lhs);
		auto variable = function.FindVariableByIdentifier(astIdentifier.Name());
		if (!variable)
			return;
		
		auto doStore = [&variable, &function](ASTNode* rhs)
		{
		  const auto& numLit = static_cast<const NumberLiteral&>(*rhs);
		  if (isIntegerLiteral(numLit.Type()))
		  {
			  auto size = variable->Size();
			  Constant value{ .Type = IntType{ size * 8 }, .Value = numLit.AsInt() };
			  StoreInst store{ .Value = value, .Ptr = variable, .Alignment = { size }};
			  function.Blocks.back().Body.emplace_back(store); 
		  }
		  else if (!isIntegerLiteral(numLit.Type()) && isNumberLiteral(numLit.Type()))
		  {
			  SingleValueType type;
			  if (numLit.Type() == TokenType::T_FLOAT_L)
				  type = SingleValueType::Float;
			  else if (numLit.Type() == TokenType::T_DOUBLE_L)
				  type = SingleValueType::Double;
			  Constant value{ .Type = type, .Value = numLit.AsDouble() };
			  StoreInst store{ .Value = value, .Ptr = variable, .Alignment = { size_of(numLit.Type()) }};
			  function.Blocks.back().Body.emplace_back(store);
		  }
		  else
			  println(Colour::Red, "Unknown number type: {;255;255;255}",
					  token_to_string(numLit.Type()));
		};

		if (rhs->class_name() == "NumberLiteral")
		{
			doStore(rhs);
		}
		else if (rhs->class_name() == "BinaryExpression")
		{
			auto& binExpr = static_cast<BinaryExpression&>(*rhs);
			if (binExpr.Constexpr())
			{
				const auto& rhsExpr = static_cast<const BinaryExpression&>(*rhs);
				auto eval = rhsExpr.Evaluate();
				doStore(eval.get());
			}
			else
				generateBinEq(binExpr, function);
		}
		else if (rhs->class_name() == "Identifier")
		{
			auto& ident = static_cast<Identifier&>(*rhs);
			auto rhsVariable = function.FindVariableByIdentifier(ident.Name());
			if (!rhsVariable)
				return;
			LoadInst load{
				.Type = *(std::get<AllocaInst>(rhsVariable->Allocation).Type),
				.Ptr = rhsVariable,
				.Alignment = { std::get<AllocaInst>(rhsVariable->Allocation).Size() }};
			auto temporary = std::make_shared<Variable>(Variable{
				.Name = function.GetNewUnnamedTemporary(),
				.Attributes = { AlignAttribute{ std::get<AllocaInst>(rhsVariable->Allocation).Size() }},
				.Allocation = load,
				.IsTemporary = true
			});
			StoreInst store{ 
				.Value = temporary,
				.Ptr = variable,
				.Alignment = { std::get<AllocaInst>(variable->Allocation).Size() }};
			function.Blocks.back().Body.emplace_back(*temporary);
			function.Blocks.back().Body.emplace_back(store);
		}
		else // TODO: MemberExpression, UnaryExpression, StringLiteral
		{
			println(Colour::Red, "Unknown rhs expression type: {;255;255;255}", rhs->class_name());
		}
	}
	else
	{
		println(Colour::Red, "Unknown lhs expression type: {;255;255;255}", lhs->class_name());
	}
}

void IR::generate_binary_expression(BinaryExpression& astNode, Function& function)
{
	if (astNode.Operator() == TokenType::T_EQ)
	{
		generateBinEq(astNode, function);
	}
}

}