/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-11-02.
//

#include "../Ir.h"

namespace alx::ir {

void IR::generate_if_statement(IfStatement& statement, Function& function)
{

	if (statement.Condition()->class_name() == "BinaryExpression"
		|| statement.Condition()->class_name() == "UnaryExpression")
	{
		LogicalBlock ifThen{ { function.GetNewNamedTemporary("if.then") } };
		LogicalBlock ifElse{ { function.GetNewNamedTemporary("if.else") } };
		LogicalBlock ifEnd{ { function.GetNewNamedTemporary("if.end") } };

		std::optional<Values> condition;
		if (statement.Condition()->class_name() == "BinaryExpression")
			condition = generate_binary_expression(static_cast<BinaryExpression&>(*statement.Condition()), function);
		else if (statement.Condition()->class_name() == "UnaryExpression") {
			condition = generate_unary_expression(static_cast<UnaryExpression&>(*statement.Condition()), function);
			// FIXME: use fcmp for floating point
			// FIXME: use unsigned compares for unsigned types
			ICmpInst icmpInst{ .Lhs = condition.value(),
							   .Rhs = Constant{ .Type = IntType{ 4 }, .Value = 0 },
							   .Predicate = CmpPredicate::NE };
			Variable icmpTemp{ .Name = function.GetNewUnnamedTemporary(),
							   .Attributes = { AlignAttribute{ 4 } },
							   .Allocation = icmpInst,
							   .IsTemporary = true };
			function.AppendInstruction(icmpTemp);
		}

		MUST(condition);

		BranchInst branchInst{ .Condition = condition.value(),
							   .TrueLabel = ifThen.Label,
							   .FalseLabel = statement.HasAlternate() ? ifElse.Label : ifEnd.Label };

		function.AppendInstruction(branchInst);

		// If
		function.AppendBlock(ifThen);
		generate_body(statement.Body(), function);

		BranchInst endBranch{ .TrueLabel = ifEnd.Label };
		function.AppendInstruction(endBranch);

		// Else
		if (statement.HasAlternate()) {
			function.AppendBlock(ifElse);
			if (statement.Alternate().value()->class_name() == "IfStatement")
				generate_if_statement(static_cast<IfStatement&>(*statement.Alternate().value()), function);
			else
				generate_body(static_cast<BlockStatement&>(*statement.Alternate().value()), function);
			function.AppendInstruction(endBranch);
		}
		function.AppendBlock(ifEnd);
	}
	else if (statement.Condition()->class_name() == "NumberLiteral") {
		auto& condNum = static_cast<NumberLiteral&>(*statement.Condition());
		if (condNum.AsInt()) {
			generate_body(statement.Body(), function);
			return;
		}
		else {
			if (statement.HasAlternate()) {
				if (statement.Alternate().value()->class_name() == "IfStatement")
					generate_if_statement(static_cast<IfStatement&>(*statement.Alternate().value()), function);
				else
					generate_body(static_cast<BlockStatement&>(*statement.Alternate().value()), function);
			}
			return;
		}
	}
	else {
		MUST(false && "Unknown condition type");
	}
}


void IR::generate_while_statement(WhileStatement& statement, Function& function)
{

	if (statement.Condition()->class_name() == "BinaryExpression"
		|| statement.Condition()->class_name() == "UnaryExpression")
	{

		std::optional<Values> condition;

		bool isAlwaysTrue = false;
		bool isNeverTrue = false;
		if (statement.Condition()->class_name() == "BinaryExpression") {
			const auto& binExp = static_cast<const BinaryExpression&>(*statement.Condition());
			if (binExp.Constexpr()) {
				isAlwaysTrue = binExp.Evaluate()->AsBoolNum();
				isNeverTrue = !isAlwaysTrue;
			}
		}
		else if (statement.Condition()->class_name() == "UnaryExpression") {
			// FIXME: check if unary is constexpr
		}

		if (isNeverTrue)
			return;

		LogicalBlock whileBody{ { function.GetNewNamedTemporary("while.body") } };
		if (isAlwaysTrue) {
			auto bodyBranch = BranchInst{ .TrueLabel = whileBody.Label };
			function.AppendInstruction(bodyBranch);
			function.AppendBlock(whileBody);
			generate_body(statement.Body(), function);
			BranchInst loopBranch{ .TrueLabel = whileBody.Label };
			function.AppendInstruction(loopBranch);
			return;
		}


		LogicalBlock whileCond{ { function.GetNewNamedTemporary("while.cond") } };
		LogicalBlock whileEnd{ { function.GetNewNamedTemporary("while.end") } };
		BranchInst condBranch{ .TrueLabel = whileCond.Label };
		function.AppendInstruction(condBranch);

		function.AppendBlock(whileCond);

		if (statement.Condition()->class_name() == "BinaryExpression")
			condition = generate_binary_expression(static_cast<BinaryExpression&>(*statement.Condition()), function);
		else if (statement.Condition()->class_name() == "UnaryExpression")
			condition = generate_unary_expression(static_cast<UnaryExpression&>(*statement.Condition()), function);


		BranchInst bodyBranch{ .Condition = condition.value(),
							   .TrueLabel = whileBody.Label,
							   .FalseLabel = whileEnd.Label };
		function.AppendInstruction(bodyBranch);

		function.AppendBlock(whileBody);
		generate_body(statement.Body(), function);
		BranchInst loopBranch{ .TrueLabel = whileCond.Label };
		function.AppendInstruction(loopBranch);

		function.AppendBlock(whileEnd);
		return;
	}
	else if (statement.Condition()->class_name() == "NumberLiteral") {
		auto& condNum = static_cast<NumberLiteral&>(*statement.Condition());
		if (condNum.AsBoolNum()) {
			LogicalBlock whileBody{ { function.GetNewNamedTemporary("while.body") } };
			auto bodyBranch = BranchInst{ .TrueLabel = whileBody.Label };
			function.AppendInstruction(bodyBranch);
			function.AppendBlock(whileBody);
			generate_body(statement.Body(), function);
			BranchInst loopBranch{ .TrueLabel = whileBody.Label };
			function.AppendInstruction(loopBranch);
			return;
		}
		else {
			return;
		}
	}
	else if (statement.Condition()->class_name() == "Identifier") {
		const auto& ident = static_cast<const Identifier&>(*statement.Condition());
		const auto& variable = function.FindVariableByIdentifier(ident.Name());
		LogicalBlock whileBody{ { function.GetNewNamedTemporary("while.body") } };
		LogicalBlock whileCond{ { function.GetNewNamedTemporary("while.cond") } };
		LogicalBlock whileEnd{ { function.GetNewNamedTemporary("while.end") } };

		BranchInst condBranch{ .TrueLabel = whileCond.Label };
		function.AppendInstruction(condBranch);

		function.AppendBlock(whileCond);
		LoadInst load{ .Type = *(std::get<AllocaInst>(variable->Allocation).Type),
					   .Ptr = variable,
					   .Alignment = { std::get<AllocaInst>(variable->Allocation).Size() }};
		auto loadTemp = std::make_shared<Variable>(
			Variable{ .Name = function.GetNewUnnamedTemporary(),
					  .Attributes = { AlignAttribute{ std::get<AllocaInst>(variable->Allocation).Size() } },
					  .Allocation = load,
					  .IsTemporary = true });
		ICmpInst cmp{ .Lhs = loadTemp,
					  .Rhs = Constant{ .Type = IntType{ 1 }, .Value = 0 },
					  .Predicate = CmpPredicate::NE };
		auto icmpTemp = std::make_shared<Variable>(
			Variable{ .Name = function.GetNewUnnamedTemporary(), .Allocation = cmp, .IsTemporary = true });
		function.AppendInstruction(*loadTemp);
		function.AppendInstruction(*icmpTemp);
		BranchInst bodyBranch{ .Condition = icmpTemp, .TrueLabel = whileBody.Label, .FalseLabel = whileEnd.Label };
		function.AppendInstruction(bodyBranch);

		function.AppendBlock(whileBody);
		generate_body(statement.Body(), function);
		BranchInst loopBranch{ .TrueLabel = whileCond.Label };

		function.AppendInstruction(loopBranch);
		function.AppendBlock(whileEnd);
		return;
	}
	else
		ASSERT_NOT_IMPLEMENTED_MSG(getFormatted("Unknown condition type: {}", statement.Condition()->class_name()));
	ASSERT_NOT_REACHABLE();
}


}
