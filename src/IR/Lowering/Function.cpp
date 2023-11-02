/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-29.
//

#include "../Ir.h"

namespace alx::ir {
std::shared_ptr<Variable> Function::FindVariableByIdentifier(const std::string& name)
{
	Variable variable;
	if (std::find_if(Blocks.begin(), Blocks.end(), [&name, &variable](LogicalBlock& block) {
		auto identifiers = block.Identifiers();
		auto it = std::find_if(identifiers.begin(), identifiers.end(), [name, &variable](const Variable& ident) {
			if (ident.Name == name) {
				variable = ident;
				return true;
			}
			return false;
		});
		if (it != identifiers.end())
			return true;
		return false;
	}) == Blocks.end())
	{
		println(Colour::Red, "Could not find variable {} in any of the blocks", name);
		return nullptr;
	}
	return std::make_unique<Variable>(variable);
}

[[maybe_unused]] LogicalBlock& Function::GetBlockByLabel(const std::string& label) { 
	auto it = std::find_if(Blocks.begin(), Blocks.end(), [&label](const LogicalBlock& block) {
		return block.Label.Name == label;
	});
	if (it == Blocks.end()) {
		println(Colour::Red, "Could not find block with label {}", label);
	}
	return *it;
}

[[maybe_unused]] void IR::generate_func_parameters([[maybe_unused]] FunctionDeclaration& functionDeclaration,
												   [[maybe_unused]] Function& function)
{}


void IR::generate_body(const BlockStatement& block, Function& function)
{
	bool hasReturned = false;
	for (const auto& node : block.Children()) {
		if (node->class_name() == "ReturnStatement") {
			generate_return_statement(static_cast<ReturnStatement&>(*node), function, hasReturned);
			hasReturned = true;
		}
		else if (node->class_name() == "VariableDeclaration") {
			auto& varDecl = static_cast<VariableDeclaration&>(*node);
			generate_variable(varDecl, function);
		}
		else if (node->class_name() == "BinaryExpression") {
			auto& binExpr = static_cast<BinaryExpression&>(*node);
			MUST(generate_binary_expression(binExpr, function).has_value());
			//			if (std::holds_alternative<std::shared_ptr<Variable>>(result.value()))
			//				function.Blocks.back().Body.emplace_back(*std::get<std::shared_ptr<Variable>>(result.value()));
		}
		else if (node->class_name() == "IfStatement") {
			auto& ifStmt = static_cast<IfStatement&>(*node);
			generate_if_statement(ifStmt, function);
		}
		else if (node->class_name() == "WhileStatement") {
			auto& whileStmt = static_cast<WhileStatement&>(*node);
			generate_while_statement(whileStmt, function);
		}
		else if (node->class_name() == "UnaryExpression") {
			auto& unaryExpr = static_cast<UnaryExpression&>(*node);
			MUST(generate_unary_expression(unaryExpr, function).has_value());
		}
		else
			ASSERT_NOT_IMPLEMENTED_MSG(getFormatted("Unknown node type: {}", node->class_name()));
	}
}

void IR::generate_function(FunctionDeclaration& functionDeclaration)
{

	auto type = functionDeclaration.ReturnType();

	// Resolve function name
	auto identifier = functionDeclaration.Name() + "(";
	auto sep = "";
	std::vector<FunctionParameter> parameters;
	for (const auto& arg : functionDeclaration.Arguments()) {
		identifier += sep + arg->TypeName();
		sep = ", ";

		const auto& argType = arg->Type();
		const bool isIdent = argType.index() == 1;
		FunctionParameter param{
			.Visibility = VisibilityAttribute::Local,
			.Name = arg->Name(),
		};
		if (isIdent) {
			param.Type = StructType{ .Name = std::get<std::unique_ptr<alx::Identifier>>(argType)->Name(),
									 .Visibility = VisibilityAttribute::Local };
			param.Attributes.emplace_back(ParamAttributes::ByVal);
			param.Attributes.emplace_back(AlignAttribute{ 8 });
		}
		else
			param.Type = TokenTypeToIRType(std::get<TokenType>(argType));
		parameters.push_back(std::move(param));
	}
	identifier += ")";

	auto function = std::make_unique<Function>(Function{ .Name = identifier,
														 .Visibility = VisibilityAttribute::Global,
														 .ReturnType = TokenTypeToIRType(type),
														 .Arguments = std::move(parameters) });

	LogicalBlock entry{ { "entry" } };
	function->Blocks.push_back(entry);
	generate_func_parameters(functionDeclaration, *function);

	// Generate function body
	generate_body(functionDeclaration.Body(), *function);

	m_ir.emplace_back(std::move(function));
}

}