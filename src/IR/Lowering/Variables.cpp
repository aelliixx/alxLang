/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-29.
//

#include "../Ir.h"

namespace alx::ir {
void IR::generate_variable(const VariableDeclaration& variable, Function& function)
{
	const auto& name = variable.Name();

	if (std::holds_alternative<TokenType>(variable.Type())) // TokenType
	{
		TokenType primitive = variable.TypeAsPrimitive();
		auto size = size_of(primitive);
		auto identifier = std::make_shared<Variable>(
			Variable{ .Name = name,
					  .Attributes = { AlignAttribute{ size_of(primitive) } },
					  .Allocation = AllocaInst{ .Type = std::make_unique<Types>(IR::TokenTypeToIRType(primitive)) } });
		// TODO: make sure this doesn't need to be appended after generating the binary expression value
		//       https://github.com/aelliixx/alxLang/commit/a7d957e601fe3348b0178a096b1da80b33a1a27a
		function.AppendInstruction(*identifier);
		

		if (variable.Value()) {
			if (variable.Value()->class_name() == "NumberLiteral") {
				const auto& numLit = static_cast<NumberLiteral&>(*variable.Value());
				if (isIntegerLiteral(numLit.Type())) {
					Constant value{ .Type = IntType{ size }, .Value = numLit.AsInt() };
					StoreInst store{ .Value = value, .Ptr = identifier, .Alignment = { size } };
					function.Blocks.back().Body.emplace_back(store);
				}
				else if (!isIntegerLiteral(numLit.Type()) && isNumberLiteral(numLit.Type())) {
					SingleValueType type;
					if (numLit.Type() == TokenType::T_FLOAT_L)
						type = SingleValueType::Float;
					else if (numLit.Type() == TokenType::T_DOUBLE_L)
						type = SingleValueType::Double;

					Constant value{ .Type = type, .Value = numLit.AsDouble() };
					StoreInst store{ .Value = value, .Ptr = identifier, .Alignment = { size_of(numLit.Type()) } };
					function.Blocks.back().Body.emplace_back(store);
				}
				else
					println(Colour::Red, "Unknown number primitive: {;255;255;255}", token_to_string(numLit.Type()));
			}
			else if (variable.Value()->class_name() == "Identifier") {

				auto& ident = static_cast<Identifier&>(*variable.Value());
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
				StoreInst store{ .Value = temporary, .Ptr = identifier, .Alignment = { size_of(primitive) } };
				function.Blocks.back().Body.emplace_back(*temporary);
				function.Blocks.back().Body.emplace_back(store);
			}
			else if (variable.Value()->class_name() == "BinaryExpression") {
				auto& binExpr = static_cast<BinaryExpression&>(*variable.Value());
				if (binExpr.Constexpr()) {
					const auto& rhsExpr = static_cast<const BinaryExpression&>(*variable.Value());
					auto eval = rhsExpr.Evaluate();
					auto value = NumberLiteralToValue(*eval, identifier->Size());
					StoreInst store{ .Value = value, .Ptr = identifier, .Alignment = { size_of(primitive) } };
					function.Blocks.back().Body.emplace_back(store);
				}
				else {
					auto result = generate_binary_expression(binExpr, function);
					MUST(result.has_value());
					StoreInst store{ .Value = result.value(), .Ptr = identifier, .Alignment = { size } };
					function.AppendInstruction(store);
				}
			}
			else if (variable.Value()->class_name() == "UnaryExpression") {
				auto result = generate_unary_expression(static_cast<UnaryExpression&>(*variable.Value()), function);
				MUST(result.has_value());
				StoreInst store{ .Value = result.value(), .Ptr = identifier, .Alignment = { size_of(primitive) } };
				function.Blocks.back().Body.emplace_back(store);
			}
			else
				ASSERT_NOT_IMPLEMENTED_MSG(
					getFormatted(Colour::Red, "Unknown variable value type: {}", variable.Value()->class_name()));
		}
	}
	else {
		ASSERT_NOT_IMPLEMENTED_MSG(
			getFormatted(Colour::Red, "Immediate assignment of struct members is not supported yet"));
		auto ident = variable.TypeAsIdentifier();
		auto structDefinition = std::find_if(m_ast.begin(), m_ast.end(), [&](const std::unique_ptr<ASTNode>& node) {
			return node->class_name() == "StructDeclaration"
				&& static_cast<StructDeclaration*>(node.get())->Name() == ident->Name();
		});
		if (structDefinition == m_ast.end()) {
			println(Colour::Red, "Unknown struct type: {;255;255;255}", ident->Name());
			return;
		}
		auto size = static_cast<const StructDeclaration&>(**structDefinition).Size();
		function.Blocks.back().Body.emplace_back(
			Variable{ .Name = name,
					  .Attributes = { AlignAttribute{ size } },
					  .Allocation = AllocaInst{ .Type = std::make_unique<Types>(StructType{ .Name = name }) } });
	}
}
}