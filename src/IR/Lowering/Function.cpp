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
	if (std::find_if(Blocks.begin(),
					 Blocks.end(),
					 [&name, &variable](LogicalBlock& block)
					 {
					   auto identifiers = block.Identifiers();
					   auto it =
						   std::find_if(identifiers.begin(),
										identifiers.end(),
										[name, &variable](const Variable& ident)
										{
										  if (ident.Name == name)
										  {
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
void IR::generate_func_parameters(FunctionDeclaration& functionDeclaration, Function& function)
{

}
void IR::generate_func_body(FunctionDeclaration& functionDeclaration, Function& function)
{
	bool hasReturned = false;
	for (const auto& node : functionDeclaration.Body().Children())
	{
		if (node->class_name() == "ReturnStatement")
		{
			generate_return_statement(static_cast<ReturnStatement&>(*node), function, hasReturned);
			hasReturned = true;
		}
		else if (node->class_name() == "VariableDeclaration")
		{
			auto& varDecl = static_cast<VariableDeclaration&>(*node);
			generate_variable(varDecl, function);
		}
		else if (node->class_name() == "BinaryExpression")
		{
			auto& binExpr = static_cast<BinaryExpression&>(*node);
			auto result = generate_binary_expression(binExpr, function);
			MUST(result);
//			if (std::holds_alternative<std::shared_ptr<Variable>>(result.value()))
//				function.Blocks.back().Body.emplace_back(*std::get<std::shared_ptr<Variable>>(result.value()));
		}
		else
			println(Colour::Red, "Unknown node type: {;255;255;255}", node->class_name());
	}
	if (!hasReturned)
	{
		// Add a return statement if the function doesn't have one
		// FIXME: Maybe this should be done in the parser, because it assumes that non-returning functions return void
		//	      when it could be a forgotten return statement and therefore an error.
		function.Blocks.back().Body.emplace_back(ReturnInst{});
	}
}

void IR::generate_function(FunctionDeclaration& astNode)
{

	auto type = astNode.ReturnType();

	// Resolve function name
	auto identifier = astNode.Name() + "(";
	auto sep = "";
	std::vector<FunctionParameter> parameters;
	for (const auto& arg : astNode.Arguments())
	{
		identifier += sep + arg->TypeName();
		sep = ", ";

		const auto& argType = arg->Type();
		const bool isIdent = argType.index() == 1;
		FunctionParameter param{
			.Visibility = VisibilityAttribute::Local,
			.Name = arg->Name(),
		};
		if (isIdent)
		{
			param.Type = StructType{
				.Name = std::get<std::unique_ptr<alx::Identifier>>(argType)->Name(),
				.Visibility = VisibilityAttribute::Local
			};
			param.Attributes.emplace_back(ParamAttributes::ByVal);
			param.Attributes.emplace_back(AlignAttribute{ 8 });
		}
		else
			param.Type = TokenTypeToIRType(std::get<TokenType>(argType));
		parameters.push_back(std::move(param));
	}
	identifier += ")";

	auto function =
		std::make_unique<Function>(Function{ .Name = identifier,
			.Visibility = VisibilityAttribute::Global,
			.ReturnType = TokenTypeToIRType(type),
			.Arguments = std::move(parameters)
		});

	LogicalBlock entry{{ "entry" }};
	function->Blocks.push_back(entry);
	generate_func_parameters(astNode, *function);

	// Generate function body
	generate_func_body(astNode, *function);

	m_ir.emplace_back(std::move(function));
}

}