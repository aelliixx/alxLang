/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-29.
//

#include "../Ir.h"

namespace alx::ir {
std::optional<Values>

IR::generate_bin_eq(const BinaryExpression& eqExpr, // NOLINT(*-no-recursion)
					Function& function)
{
	auto lhs = eqExpr.Lhs();
	auto rhs = eqExpr.Rhs();
	if (lhs->class_name() == "Identifier") {
		auto astIdentifier = static_cast<const Identifier&>(*lhs);
		auto variable = function.FindVariableByIdentifier(astIdentifier.Name());
		if (!variable)
			return {};

		if (rhs->class_name() == "NumberLiteral") {
			auto value = IR::NumberLiteralToValue(static_cast<const NumberLiteral&>(*rhs), variable->Size());
			StoreInst store{ .Value = value, .Ptr = variable, .Alignment = { variable->Size() } };
			function.AppendInstruction(store);
			return value;
		}
		else if (rhs->class_name() == "BinaryExpression") {
			auto& binExpr = static_cast<BinaryExpression&>(*rhs);
			if (binExpr.Constexpr()) {
				const auto& rhsExpr = static_cast<const BinaryExpression&>(*rhs);
				auto eval = rhsExpr.Evaluate();
				auto value = IR::NumberLiteralToValue(*eval, variable->Size());
				StoreInst store{ .Value = value, .Ptr = variable, .Alignment = { variable->Size() } };
				function.AppendInstruction(store);
				return value;
			}
			else {
				auto expr = generate_binary_expression(binExpr, function);
				MUST(expr);
				StoreInst store{ .Value = expr.value(), .Ptr = variable, .Alignment = { variable->Size() } };
				function.AppendInstruction(store);
				return expr.value();
			}
		}
		else if (rhs->class_name() == "Identifier") {
			auto& ident = static_cast<Identifier&>(*rhs);
			auto rhsVariable = function.FindVariableByIdentifier(ident.Name());
			if (!rhsVariable)
				return {};
			LoadInst load{ .Type = *(std::get<AllocaInst>(rhsVariable->Allocation).Type),
						   .Ptr = rhsVariable,
						   .Alignment = { std::get<AllocaInst>(rhsVariable->Allocation).Size() } };
			auto loadTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(),
						  .Attributes = { AlignAttribute{ std::get<AllocaInst>(rhsVariable->Allocation).Size() } },
						  .Allocation = load,
						  .IsTemporary = true });
			StoreInst store{ .Value = loadTemp,
							 .Ptr = variable,
							 .Alignment = { std::get<AllocaInst>(variable->Allocation).Size() } };
			function.Blocks.back().Body.emplace_back(*loadTemp);
			function.AppendInstruction(store);
			return loadTemp;
		}
		else if (rhs->class_name() == "BinaryExpression") {
			auto& binExpr = static_cast<BinaryExpression&>(*rhs);
			if (binExpr.Constexpr()) {
				const auto& rhsExpr = static_cast<const BinaryExpression&>(*rhs);
				auto eval = rhsExpr.Evaluate();
				auto value = IR::NumberLiteralToValue(*eval, variable->Size());
				StoreInst store{ .Value = value, .Ptr = variable, .Alignment = { variable->Size() } };
				function.AppendInstruction(store);
				return value;
			}
			else {
				auto result = generate_binary_expression(binExpr, function);
				MUST(result.has_value());
				return result.value();
			}
		}
		else if (rhs->class_name() == "UnaryExpression") {
			auto& unExpr = static_cast<UnaryExpression&>(*rhs);
			auto result = generate_unary_expression(unExpr, function);
			if (!result.has_value())
				return {};
			StoreInst store{ .Value = result.value(), .Ptr = variable, .Alignment = { variable->Size() } };
			function.Blocks.back().Body.emplace_back(store);
			return result.value();
		}
		else // TODO: MemberExpression, StringLiteral
		{
			println(Colour::Red, "Unknown rhs expression type: {;255;255;255}", rhs->class_name());
		}
	}
	else if (lhs->class_name() == "NumberLiteral")
		ASSERT_NOT_REACHABLE(); // Can't assign to prvalue
	else if (lhs->class_name() == "BinaryExpression")
		ASSERT_NOT_REACHABLE(); // Can't assign to prvalue
	else if (lhs->class_name() == "UnaryExpression")
		ASSERT_NOT_REACHABLE(); // Can't assign to prvalue
	else if (lhs->class_name() == "MemberExpression")
		ASSERT_NOT_IMPLEMENTED();
	else if (lhs->class_name() == "StringLiteral")
		ASSERT_NOT_REACHABLE(); // Read-only value is not assignable

	println(Colour::Red, "Unknown lhs expression type: {;255;255;255}", lhs->class_name());
	ASSERT_NOT_REACHABLE();
}

template<typename Func>
std::optional<std::shared_ptr<Variable>> IR::generate_binary_op(const BinaryExpression& binaryExpression,
																Function& function,
																Func instruction)
{
	auto lhs = binaryExpression.Lhs();
	auto rhs = binaryExpression.Rhs();
	if (lhs->class_name() == "Identifier") {
		auto astIdentifier = static_cast<const Identifier&>(*lhs);
		
		auto variable = function.FindVariableByIdentifier(astIdentifier.Name());
		MUST(variable);

		LoadInst loadLhs{ .Type = *(std::get<AllocaInst>(variable->Allocation).Type),
						  .Ptr = variable,
						  .Alignment = { std::get<AllocaInst>(variable->Allocation).Size() } };
		auto temporary = std::make_shared<Variable>(
			Variable{ .Name = function.GetNewUnnamedTemporary(),
					  .Attributes = { AlignAttribute{ std::get<AllocaInst>(variable->Allocation).Size() } },
					  .Allocation = loadLhs,
					  .IsTemporary = true });

		if (rhs->class_name() == "NumberLiteral") {
			const auto& numLit = static_cast<const NumberLiteral&>(*rhs);
			auto value = IR::NumberLiteralToValue(numLit, variable->Size());
			function.AppendInstruction(*temporary);
			auto instructionTemp = instruction(temporary, value);
			return instructionTemp;
		}
		else if (rhs->class_name() == "Identifier") {
			auto& ident = static_cast<Identifier&>(*rhs);
			auto rhsVariable = function.FindVariableByIdentifier(ident.Name());
			MUST(rhsVariable);

			LoadInst loadRhs{ .Type = *(std::get<AllocaInst>(rhsVariable->Allocation).Type),
							  .Ptr = rhsVariable,
							  .Alignment = { std::get<AllocaInst>(rhsVariable->Allocation).Size() } };
			auto temporaryRhs = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(),
						  .Attributes = { AlignAttribute{ std::get<AllocaInst>(rhsVariable->Allocation).Size() } },
						  .Allocation = loadRhs,
						  .IsTemporary = true });
			function.AppendInstruction(*temporary);
			function.AppendInstruction(*temporaryRhs);
			auto instructionTemp = instruction(temporary, temporaryRhs);
			return instructionTemp;
		}
		else if (rhs->class_name() == "BinaryExpression") {
			auto result = generate_binary_expression(static_cast<BinaryExpression&>(*rhs), function);
			MUST(result.has_value());
			function.AppendInstruction(*temporary);
			auto instructionTemp = instruction(temporary, result.value());
			return instructionTemp;
		}
		else if (rhs->class_name() == "UnaryExpression") {
			auto result = generate_unary_expression(static_cast<UnaryExpression&>(*rhs), function);
			MUST(result.has_value());
			function.AppendInstruction(*temporary);
			auto instructionTemp = instruction(temporary, result.value());
			return instructionTemp;
		}
		else if (rhs->class_name() == "MemberExpression") {
			ASSERT_NOT_IMPLEMENTED();
		}
		else if (rhs->class_name() == "StringLiteral") {
			ASSERT_NOT_IMPLEMENTED();
		}
		println(Colour::Red, "Unknown rhs expression type: {;255;255;255}", rhs->class_name());
	}
	else if (lhs->class_name() == "NumberLiteral") {
		auto& numLit = static_cast<const NumberLiteral&>(*lhs);
		auto value = IR::NumberLiteralToValue(numLit, size_of(numLit.Type()));

		if (rhs->class_name() == "Identifier") {
			auto ident = static_cast<const Identifier&>(*rhs);
			auto variable = function.FindVariableByIdentifier(ident.Name());
			MUST(variable);
			LoadInst loadLhs{ .Type = *(std::get<AllocaInst>(variable->Allocation).Type),
							  .Ptr = variable,
							  .Alignment = { std::get<AllocaInst>(variable->Allocation).Size() } };
			auto temporary = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(),
						  .Attributes = { AlignAttribute{ std::get<AllocaInst>(variable->Allocation).Size() } },
						  .Allocation = loadLhs,
						  .IsTemporary = true });
			function.AppendInstruction(*temporary);
			auto instructionTemp = instruction(value, temporary);
			return instructionTemp;
		}
		else if (rhs->class_name() == "NumberLiteral") {
			// This should always be a constant expression
			ASSERT_NOT_REACHABLE();
		}
		else if (rhs->class_name() == "BinaryExpression") {
			auto result = generate_binary_expression(static_cast<BinaryExpression&>(*rhs), function);
			MUST(result.has_value());
			auto instructionTemp = instruction(value, result.value());
			return instructionTemp;
		}
		else if (rhs->class_name() == "UnaryExpression") {
			auto result = generate_unary_expression(static_cast<UnaryExpression&>(*rhs), function);
			MUST(result.has_value());
			auto instructionTemp = instruction(value, result.value());
			return instructionTemp;
		}
		else if (rhs->class_name() == "MemberExpression") {
			ASSERT_NOT_IMPLEMENTED();
		}
		else if (rhs->class_name() == "StringLiteral") {
			ASSERT_NOT_IMPLEMENTED();
		}
		println(Colour::Red, "Unknown rhs expression type: {;255;255;255}", rhs->class_name());
	}
	else if (lhs->class_name() == "BinaryExpression") {
		const auto& binExp = static_cast<const BinaryExpression&>(*lhs);

		std::shared_ptr<Variable> resulting;
		std::variant<Values, NumberLiteral> result;
		if (binExp.Constexpr())
			result = *binExp.Evaluate();
		else {
			auto eval = generate_binary_expression(binExp, function);
			MUST(eval.has_value());
			result = eval.value();
		}
		if (rhs->class_name() == "Identifier") {

			auto ident = static_cast<const Identifier&>(*rhs);
			auto variable = function.FindVariableByIdentifier(ident.Name());
			MUST(variable);
			LoadInst loadLhs{ .Type = *(std::get<AllocaInst>(variable->Allocation).Type),
							  .Ptr = variable,
							  .Alignment = { std::get<AllocaInst>(variable->Allocation).Size() } };
			auto temporary = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(),
						  .Attributes = { AlignAttribute{ std::get<AllocaInst>(variable->Allocation).Size() } },
						  .Allocation = loadLhs,
						  .IsTemporary = true });
			if (binExp.Constexpr()) {
				auto lhsValue = NumberLiteralToValue(std::get<NumberLiteral>(result),
													 size_of(std::get<NumberLiteral>(result).Type()));
				auto instructionTemp = instruction(lhsValue, temporary);
				function.AppendInstruction(*temporary);
				return instructionTemp;
			}
			auto lhsValue = std::get<Values>(result);
			function.AppendInstruction(*temporary);
			auto instructionTemp = instruction(lhsValue, temporary);
			return instructionTemp;
		}
		else if (rhs->class_name() == "NumberLiteral") {
			// If this is not constexpr, we messed up in the parser
			MUST(!binExp.Constexpr() && "Binary op with 'NumberLiteral's on both sides should be constexpr");
			auto resultVariable = std::get<Values>(result);
			auto value = IR::NumberLiteralToValue(static_cast<const NumberLiteral&>(*rhs),
												  std::get<std::shared_ptr<Variable>>(resultVariable)->Size());
			auto instructionTemp = instruction(resultVariable, value);
			return instructionTemp;
		}
		else if (rhs->class_name() == "BinaryExpression") {
			const auto& rhsBinExp = static_cast<const BinaryExpression&>(*rhs);
			if (binExp.Constexpr()) {

				auto lhsValue = NumberLiteralToValue(std::get<NumberLiteral>(result),
													 size_of(std::get<NumberLiteral>(result).Type()));
				if (rhsBinExp.Constexpr()) {
					auto rhsValue =
						NumberLiteralToValue(*rhsBinExp.Evaluate(), size_of(std::get<NumberLiteral>(result).Type()));
					auto instructionTemp = instruction(lhsValue, rhsValue);
					return instructionTemp;
				}
				auto rhsValue = generate_binary_expression(rhsBinExp, function);
				MUST(rhsValue.has_value());
				auto instructionTemp = instruction(lhsValue, rhsValue.value());
				return instructionTemp;
			}
			if (rhsBinExp.Constexpr()) {
				auto rhsValue =
					NumberLiteralToValue(*rhsBinExp.Evaluate(), size_of(std::get<NumberLiteral>(result).Type()));
				auto instructionTemp = instruction(std::get<Values>(result), rhsValue);
				return instructionTemp;
			}
			auto rhsValue = generate_binary_expression(rhsBinExp, function);
			MUST(rhsValue.has_value());
			auto instructionTemp = instruction(std::get<Values>(result), rhsValue.value());
			return instructionTemp;
		}
		else if (rhs->class_name() == "UnaryExpression") {
			auto rhsValue = generate_unary_expression(static_cast<UnaryExpression&>(*rhs), function);
			MUST(rhsValue.has_value());
			if (binExp.Constexpr()) {
				auto lhsValue = NumberLiteralToValue(std::get<NumberLiteral>(result),
													 size_of(std::get<NumberLiteral>(result).Type()));
				auto instructionTemp = instruction(lhsValue, rhsValue.value());
				return instructionTemp;
			}
			auto instructionTemp = instruction(std::get<Values>(result), rhsValue.value());
			return instructionTemp;
		}
		else if (rhs->class_name() == "MemberExpression") {
			ASSERT_NOT_IMPLEMENTED();
		}
		else if (rhs->class_name() == "StringLiteral") {
			ASSERT_NOT_IMPLEMENTED();
		}
		println(Colour::Red, "Unknown rhs expression type: {;255;255;255}", rhs->class_name());
	}
	else if (lhs->class_name() == "UnaryExpression") {
		auto& unExpr = static_cast<UnaryExpression&>(*lhs);
		auto result = generate_unary_expression(unExpr, function);

		if (rhs->class_name() == "Identifier") {
			auto ident = static_cast<const Identifier&>(*rhs);
			auto variable = function.FindVariableByIdentifier(ident.Name());
			MUST(variable);

			LoadInst loadRhs{ .Type = *(std::get<AllocaInst>(variable->Allocation).Type),
							  .Ptr = variable,
							  .Alignment = { std::get<AllocaInst>(variable->Allocation).Size() } };
			auto temporary = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(),
						  .Attributes = { AlignAttribute{ std::get<AllocaInst>(variable->Allocation).Size() } },
						  .Allocation = loadRhs,
						  .IsTemporary = true });
			function.AppendInstruction(*temporary);
			auto instructionTemp = instruction(result.value(), temporary);
			return instructionTemp;
		}
		else if (rhs->class_name() == "NumberLiteral") {
			auto value = IR::NumberLiteralToValue(static_cast<const NumberLiteral&>(*rhs), result.value()->Size());
			auto instructionTemp = instruction(result.value(), value);
			return instructionTemp;
		}
		else if (rhs->class_name() == "BinaryExpression") {
			auto rhsValue = generate_binary_expression(static_cast<BinaryExpression&>(*rhs), function);
			MUST(rhsValue.has_value());
			auto instructionTemp = instruction(result.value(), rhsValue.value());
			return instructionTemp;
		}
		else if (rhs->class_name() == "UnaryExpression") {
			auto rhsValue = generate_unary_expression(static_cast<UnaryExpression&>(*rhs), function);
			MUST(rhsValue.has_value());
			auto instructionTemp = instruction(result.value(), rhsValue.value());
			return instructionTemp;
		}
		else if (rhs->class_name() == "MemberExpression") {
			ASSERT_NOT_IMPLEMENTED();
		}
		else if (rhs->class_name() == "StringLiteral") {
			ASSERT_NOT_IMPLEMENTED();
		}
		println(Colour::Red, "Unknown rhs expression type: {;255;255;255}", rhs->class_name());
	}
	else if (lhs->class_name() == "MemberExpression") {
		ASSERT_NOT_IMPLEMENTED();
	}
	else if (lhs->class_name() == "StringLiteral") {
		ASSERT_NOT_IMPLEMENTED();
	}
	else {
		println(Colour::Red, "Unknown lhs expression type: {;255;255;255}", lhs->class_name());
	}
	ASSERT_NOT_REACHABLE();
}

std::optional<Values> IR::generate_binary_expression(const BinaryExpression& binaryExpression, // NOLINT(*-no-recursion)
													 Function& function)
{
	switch (binaryExpression.Operator()) {
	case TokenType::T_EQ:
		return generate_bin_eq(binaryExpression, function);
	case TokenType::T_PLUS:
		return generate_binary_op(binaryExpression, function, [&function](const Values& variable, const Values& value) {
			AddInst add{ .Lhs = variable, .Rhs = value };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = add, .IsTemporary = true });
			function.AppendInstruction(*instructionTemp);
			return instructionTemp;
		});
	case TokenType::T_MINUS:
		return generate_binary_op(binaryExpression, function, [&function](const Values& variable, const Values& value) {
			SubInst add{ .Lhs = variable, .Rhs = value };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = add, .IsTemporary = true });
			function.AppendInstruction(*instructionTemp);
			return instructionTemp;
		});
	case TokenType::T_STAR:
		return generate_binary_op(binaryExpression, function, [&function](const Values& variable, const Values& value) {
			MulInst add{ .Lhs = variable, .Rhs = value };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = add, .IsTemporary = true });
			function.AppendInstruction(*instructionTemp);
			return instructionTemp;
		});
	case TokenType::T_FWD_SLASH:
		return generate_binary_op(binaryExpression, function, [&function](const Values& variable, const Values& value) {
			// FIXME: Check whether we need to use sdiv or udiv
			SDivInst add{ .Lhs = variable, .Rhs = value };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = add, .IsTemporary = true });
			function.AppendInstruction(*instructionTemp);
			return instructionTemp;
		});
	case TokenType::T_POW:
		[[fallthrough]];
	case TokenType::T_LT:
		return generate_binary_op(binaryExpression, function, [&function](const Values& variable, const Values& value) {
			// FIXME: Check whether we need to use icmp or fcmp
			// FIXME: Check whether we need to use slt or ult
			ICmpInst cmp{ .Lhs = variable, .Rhs = value, .Predicate = CmpPredicate::SLT };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = cmp, .IsTemporary = true });
			function.AppendInstruction(*instructionTemp);
			return instructionTemp;
		});
	case TokenType::T_GT:
		return generate_binary_op(binaryExpression, function, [&function](const Values& variable, const Values& value) {
			// FIXME: Check whether we need to use icmp or fcmp
			// FIXME: Check whether we need to use slt or ult
			ICmpInst cmp{ .Lhs = variable, .Rhs = value, .Predicate = CmpPredicate::SGT };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = cmp, .IsTemporary = true });
			function.AppendInstruction(*instructionTemp);
			return instructionTemp;
		});
	case TokenType::T_LTE:
		return generate_binary_op(binaryExpression, function, [&function](const Values& variable, const Values& value) {
			// FIXME: Check whether we need to use icmp or fcmp
			// FIXME: Check whether we need to use slt or ult
			ICmpInst cmp{ .Lhs = variable, .Rhs = value, .Predicate = CmpPredicate::SLE };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = cmp, .IsTemporary = true });
			function.AppendInstruction(*instructionTemp);
			return instructionTemp;
		});
	case TokenType::T_GTE:
		return generate_binary_op(binaryExpression, function, [&function](const Values& variable, const Values& value) {
			// FIXME: Check whether we need to use icmp or fcmp
			// FIXME: Check whether we need to use slt or ult
			ICmpInst cmp{ .Lhs = variable, .Rhs = value, .Predicate = CmpPredicate::SGE };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = cmp, .IsTemporary = true });
			function.AppendInstruction(*instructionTemp);
			return instructionTemp;
		});
	case TokenType::T_EQEQ:
		return generate_binary_op(binaryExpression, function, [&function](const Values& variable, const Values& value) {
			// FIXME: Check whether we need to use icmp or fcmp
			// FIXME: Check whether we need to use slt or ult
			ICmpInst cmp{ .Lhs = variable, .Rhs = value, .Predicate = CmpPredicate::EQ };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = cmp, .IsTemporary = true });
			function.AppendInstruction(*instructionTemp);
			return instructionTemp;
		});
	case TokenType::T_MOD:
		[[fallthrough]];
	case TokenType::T_COLON:
		ASSERT_NOT_IMPLEMENTED_MSG(token_to_string(binaryExpression.Operator()));
	case TokenType::T_NOT_EQ:
		return generate_binary_op(binaryExpression, function, [&function](const Values& variable, const Values& value) {
			// FIXME: Check whether we need to use icmp or fcmp
			// FIXME: Check whether we need to use slt or ult
			ICmpInst cmp{ .Lhs = variable, .Rhs = value, .Predicate = CmpPredicate::NE };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = cmp, .IsTemporary = true });
			function.AppendInstruction(*instructionTemp);
			return instructionTemp;
		});
	case TokenType::T_ADD_EQ: {
		MUST(binaryExpression.Lhs()->class_name() == "Identifier");
		auto lhsIdent = static_cast<const Identifier&>(*binaryExpression.Lhs());
		auto lhsVar = function.FindVariableByIdentifier(lhsIdent.Name());
		return generate_binary_op(
			binaryExpression, function, [&lhsVar, &function](const Values& variable, const Values& value) {
			AddInst add{ .Lhs = variable, .Rhs = value };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = add, .IsTemporary = true });
			StoreInst store{ .Value = instructionTemp,
							 .Ptr = lhsVar,
							 .Alignment = { std::get<AllocaInst>(lhsVar->Allocation).Size() } };
			function.AppendInstruction(*instructionTemp);
			function.AppendInstruction(store);
			return instructionTemp;
		});
	}
	case TokenType::T_SUB_EQ: {
		MUST(binaryExpression.Lhs()->class_name() == "Identifier");
		auto lhsIdent = static_cast<const Identifier&>(*binaryExpression.Lhs());
		auto lhsVar = function.FindVariableByIdentifier(lhsIdent.Name());
		return generate_binary_op(
			binaryExpression, function, [&lhsVar, &function](const Values& variable, const Values& value) {
			SubInst sub{ .Lhs = variable, .Rhs = value };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = sub, .IsTemporary = true });
			StoreInst store{ .Value = instructionTemp,
							 .Ptr = lhsVar,
							 .Alignment = { std::get<AllocaInst>(lhsVar->Allocation).Size() } };
			function.AppendInstruction(*instructionTemp);
			function.AppendInstruction(store);
			return instructionTemp;
		});
	}
	case TokenType::T_MULT_EQ: {
		MUST(binaryExpression.Lhs()->class_name() == "Identifier");
		auto lhsIdent = static_cast<const Identifier&>(*binaryExpression.Lhs());
		auto lhsVar = function.FindVariableByIdentifier(lhsIdent.Name());
		return generate_binary_op(
			binaryExpression, function, [&lhsVar, &function](const Values& variable, const Values& value) {
			MulInst mul{ .Lhs = variable, .Rhs = value };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = mul, .IsTemporary = true });
			StoreInst store{ .Value = instructionTemp,
							 .Ptr = lhsVar,
							 .Alignment = { std::get<AllocaInst>(lhsVar->Allocation).Size() } };
			function.AppendInstruction(*instructionTemp);
			function.AppendInstruction(store);
			return instructionTemp;
		});
	}
	case TokenType::T_DIV_EQ: {
		MUST(binaryExpression.Lhs()->class_name() == "Identifier");
		auto lhsIdent = static_cast<const Identifier&>(*binaryExpression.Lhs());
		auto lhsVar = function.FindVariableByIdentifier(lhsIdent.Name());
		return generate_binary_op(
			binaryExpression, function, [&lhsVar, &function](const Values& variable, const Values& value) {
			SDivInst div{ .Lhs = variable, .Rhs = value };
			auto instructionTemp = std::make_shared<Variable>(
				Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = div, .IsTemporary = true });
			StoreInst store{ .Value = instructionTemp,
							 .Ptr = lhsVar,
							 .Alignment = { std::get<AllocaInst>(lhsVar->Allocation).Size() } };
			function.AppendInstruction(*instructionTemp);
			function.AppendInstruction(store);
			return instructionTemp;
		});
	}
	case TokenType::T_MOD_EQ:
	case TokenType::T_POW_EQ:
		ASSERT_NOT_IMPLEMENTED_MSG(token_to_string(binaryExpression.Operator()));
	default:
		println(Colour::Red,
				"Unknown binary expression type: {;255;255;255}",
				token_to_string(binaryExpression.Operator()));
		return {};
	}
	ASSERT_NOT_REACHABLE();
}

} // namespace alx::ir
