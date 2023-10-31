/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-30.
//

#include "../Ir.h"

namespace alx::ir {

std::optional<std::shared_ptr<Variable>> IR::generate_unary_expression(const UnaryExpression& unaryExpression, // NOLINT(*-no-recursion)
																	   Function& function)
{
	switch (unaryExpression.Operator())
	{
	case TokenType::T_MINUS:
	{
		auto rhs = unaryExpression.Rhs();
		// FIXME: this should resolve to a negative variable in the parser (e.g. -(1) -> -1)
		if (rhs->class_name() == "NumberLiteral")
		{
			fixme("Unary minus on number literal");
			auto& numLit = static_cast<NumberLiteral&>(*rhs);
			auto value = IR::NumberLiteralToValue(numLit, size_of(numLit.Type()) );
			SubInst sub{
				.Lhs = Constant{ .Type = IntType{ 4 }, .Value = 0 },
				.Rhs = value,
			};
			auto subTemp = std::make_shared<Variable>(Variable{
				.Name = function.GetNewUnnamedTemporary(),
				.Allocation = sub,
				.IsTemporary = true
			});
			function.Blocks.back().Body.emplace_back(*subTemp);
			return subTemp;
		}
		else if (rhs->class_name() == "Identifier")
		{
			auto& ident = static_cast<Identifier&>(*rhs);
			auto rhsVariable = function.FindVariableByIdentifier(ident.Name());
			MUST(rhsVariable);
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
			SubInst sub{
				.Lhs = Constant{ .Type = IntType{ 4 }, .Value = 0 },
				.Rhs = temporary,
			};
			auto subTemp = std::make_shared<Variable>(Variable{
				.Name = function.GetNewUnnamedTemporary(),
				.Allocation = sub,
				.IsTemporary = true
			});
			function.Blocks.back().Body.emplace_back(*temporary);
			function.Blocks.back().Body.emplace_back(*subTemp);
			return subTemp;
		}
		else if (rhs->class_name() == "BinaryExpression")
		{
			auto& binExpr = static_cast<BinaryExpression&>(*rhs);
			if (binExpr.Constexpr())
			{
				const auto& rhsExpr = static_cast<const BinaryExpression&>(*rhs);
				auto eval = rhsExpr.Evaluate();
				auto value = IR::NumberLiteralToValue(*eval, size_of(eval->Type()) );
				SubInst sub{
					.Lhs = Constant{ .Type = IntType{ 4 }, .Value = 0 },
					.Rhs = value,
				};
				auto subTemp = std::make_shared<Variable>(Variable{
					.Name = function.GetNewUnnamedTemporary(),
					.Allocation = sub,
					.IsTemporary = true
				});
				function.Blocks.back().Body.emplace_back(*subTemp);
				return subTemp;
			}
			else
			{
				auto result = generate_binary_expression(binExpr, function);
				MUST(result.has_value());
				SubInst sub{
					.Lhs = Constant{ .Type = IntType{ 4 }, .Value = 0 },
					.Rhs = result.value(),
				};
				auto subTemp = std::make_shared<Variable>(Variable{
					.Name = function.GetNewUnnamedTemporary(),
					.Allocation = sub,
					.IsTemporary = true
				});
				function.Blocks.back().Body.emplace_back(*subTemp);
				return subTemp;
			}
		}
		else if (rhs->class_name() == "UnaryExpression")
		{
			auto& unExpr = static_cast<UnaryExpression&>(*rhs);
			auto result = generate_unary_expression(unExpr, function);
			if (!result.has_value())
				return {};
			SubInst sub{
				.Lhs = Constant{ .Type = IntType{ 4 }, .Value = 0 },
				.Rhs = result.value(), // FIXME
			};
			auto subTemp = std::make_shared<Variable>(Variable{
				.Name = function.GetNewUnnamedTemporary(),
				.Allocation = sub,
				.IsTemporary = true
			});
			function.Blocks.back().Body.emplace_back(*subTemp);
			return subTemp;
		}
		else if (rhs->class_name() == "MemberExpression")
			ASSERT_NOT_IMPLEMENTED();
		else if (rhs->class_name() == "StringLiteral")
			ASSERT_NOT_IMPLEMENTED();
		else // Call expression
			ASSERT_NOT_IMPLEMENTED();

	}
	case TokenType::T_PLUS:

	case TokenType::T_SUB:
		[[fallthrough]];
	case TokenType::T_ADD:
		[[fallthrough]];
	case TokenType::T_NOT:
		[[fallthrough]];
	default:
		println(Colour::Red,
				"Unknown unary expression type: {;255;255;255}",
				token_to_string(unaryExpression.Operator()));
		return {};
	}
}

}