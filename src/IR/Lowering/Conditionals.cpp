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

	if (statement.Condition()->class_name() == "BinaryExpression") {
		auto cmp = generate_binary_expression(static_cast<BinaryExpression&>(*statement.Condition()), function);
		MUST(cmp);

		LogicalBlock ifThen{ { function.GetNewNamedTemporary("if.then") } };
		LogicalBlock ifElse{ { function.GetNewNamedTemporary("if.else") } };
		LogicalBlock ifEnd{ { function.GetNewNamedTemporary("if.end") } };

		BranchInst branchInst{ .Condition = cmp.value(),
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
	else if (statement.Condition()->class_name() == "UnaryOperator") {
		
	}
	else {
		MUST(false && "Unknown condition type");
	}
}


}
