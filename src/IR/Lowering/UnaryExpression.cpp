/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-30.
//

#include "../Ir.h"

namespace alx::ir {

template<typename Func>
std::optional<std::shared_ptr<Variable>> IR::generate_unary_op(const UnaryExpression& unaryExpression,
															   Function& function,
															   Func instruction)
{
	auto rhs = unaryExpression.Rhs();
	// FIXME: this should resolve to a negative variable in the parser (e.g. -(1) -> -1)
	if (rhs->class_name() == "NumberLiteral") {
		fixme("Unary minus on number literal");
		auto& numLit = static_cast<NumberLiteral&>(*rhs);
		auto value = IR::NumberLiteralToValue(numLit, size_of(numLit.Type()));
		auto temp = instruction(value);
		return temp;
	}
	else if (rhs->class_name() == "Identifier") {
		auto& ident = static_cast<Identifier&>(*rhs);
		auto rhsVariable = function.FindVariableByIdentifier(ident.Name());
		MUST(rhsVariable);
		LoadInst load{ .Type = *(std::get<AllocaInst>(rhsVariable->Allocation).Type),
					   .Ptr = rhsVariable,
					   .Alignment = { std::get<AllocaInst>(rhsVariable->Allocation).Size() } };
		auto loadTemp = std::make_shared<Variable>(
			Variable{ .Name = function.GetNewUnnamedTemporary(),
					  .Attributes = { AlignAttribute{ std::get<AllocaInst>(rhsVariable->Allocation).Size() } },
					  .Allocation = load,
					  .IsTemporary = true });
		function.AppendInstruction(*loadTemp);
		auto temp = instruction(loadTemp);
		return temp;
	}
	else if (rhs->class_name() == "BinaryExpression") {
		auto& binExpr = static_cast<BinaryExpression&>(*rhs);
		if (binExpr.Constexpr()) {
			fixme("Unary minus on number literal");
			const auto& rhsExpr = static_cast<const BinaryExpression&>(*rhs);
			auto eval = rhsExpr.Evaluate();
			auto value = IR::NumberLiteralToValue(*eval, size_of(eval->Type()));
			auto temp = instruction(value);
			return temp;
		}
		else {
			auto result = generate_binary_expression(binExpr, function);
			MUST(result.has_value());
			auto temp = instruction(result.value());
			return temp;
		}
	}
	else if (rhs->class_name() == "UnaryExpression") {
		auto& unExpr = static_cast<UnaryExpression&>(*rhs);
		auto result = generate_unary_expression(unExpr, function);
		MUST(result.has_value());
		auto temp = instruction(result.value());
		return temp;
	}
	else if (rhs->class_name() == "MemberExpression")
		ASSERT_NOT_IMPLEMENTED(); // NOLINT(*-branch-clone)
	else if (rhs->class_name() == "StringLiteral")
		ASSERT_NOT_IMPLEMENTED();
	else // Call expression
		ASSERT_NOT_IMPLEMENTED();
}

std::optional<std::shared_ptr<Variable>> IR::generate_unary_expression(
	const UnaryExpression& unaryExpression, // NOLINT(*-no-recursion)
	Function& function)
{
	switch (unaryExpression.Operator()) {
	case TokenType::T_MINUS:
		return generate_unary_op(unaryExpression, function, [&function](const Values& value) {
			SubInst sub{
				.Lhs = Constant{ .Type = IntType{ 4 }, .Value = 0 },
				.Rhs = value,
			};
			auto subTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = sub, .IsTemporary = true });
			function.AppendInstruction(*subTemp);
			return subTemp;
		});
	case TokenType::T_PLUS:
		return generate_unary_op(unaryExpression, function, [&function](const Values& value) {
			if (std::holds_alternative<std::shared_ptr<Variable>>(value))
				return std::get<std::shared_ptr<Variable>>(value);
			AddInst add{
				.Lhs = Constant{ .Type = IntType{ 4 }, .Value = 0 },
				.Rhs = value,
			};
			auto addTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = add, .IsTemporary = true });
			function.AppendInstruction(*addTemp);
			return addTemp;
		});
	case TokenType::T_SUB: {
		MUST(unaryExpression.Rhs()->class_name() == "Identifier"); // FIXME: allow member expressions
		return generate_unary_op(unaryExpression, function, [&unaryExpression, &function](const Values& value) {
			MUST(std::holds_alternative<std::shared_ptr<Variable>>(value));
			SubInst sub{
				.Lhs = value,
				.Rhs = Constant{ .Type = IntType{ 4 }, .Value = 1 },
			};
			auto subTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = sub, .IsTemporary = true });
			function.AppendInstruction(*subTemp);
			auto rhsVariable =
				function.FindVariableByIdentifier(static_cast<const Identifier&>(*unaryExpression.Rhs()).Name());
			MUST(rhsVariable);
			StoreInst store{ .Value = subTemp, .Ptr = rhsVariable, .Alignment = { rhsVariable->Size() } };
			function.AppendInstruction(store);
			return subTemp;
		});
	}
	case TokenType::T_ADD: {
		MUST(unaryExpression.Rhs()->class_name() == "Identifier"); // FIXME: allow member expressions
		return generate_unary_op(unaryExpression, function, [&unaryExpression, &function](const Values& value) {
			MUST(std::holds_alternative<std::shared_ptr<Variable>>(value));
			AddInst add{
				.Lhs = value,
				.Rhs = Constant{ .Type = IntType{ 4 }, .Value = 1 },
			};
			auto subTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = add, .IsTemporary = true });
			function.AppendInstruction(*subTemp);
			auto rhsVariable =
				function.FindVariableByIdentifier(static_cast<const Identifier&>(*unaryExpression.Rhs()).Name());
			MUST(rhsVariable);
			StoreInst store{ .Value = subTemp, .Ptr = rhsVariable, .Alignment = { rhsVariable->Size() } };
			function.AppendInstruction(store);
			return subTemp;
		});
	}
	case TokenType::T_NOT:
		[[fallthrough]];
	default:
		println(
			Colour::Red, "Unknown unary expression type: {;255;255;255}", token_to_string(unaryExpression.Operator()));
		return {};
	}
}

}