/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-08.
//

#include "Ir.h"
#include "../libs/Println.h"

namespace alx::ir {

void IR::Generate()
{
	for (const auto& node : m_ast)
	{
		if (node->class_name() == "FunctionDeclaration")
		{
			auto& func = static_cast<FunctionDeclaration&>(*node);
			GenerateFunction(func);
		}
		else
		{
//			ASSERT_NOT_IMPLEMENTED();
		}
	}
}

void IR::GenerateFunction(FunctionDeclaration& astNode)
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

		const std::variant<TokenType, std::unique_ptr<Identifier>>& argType = *arg->Type();
		const bool isIdent = argType.index() == 1;
		FunctionParameter param{
			.Visibility = VisibilityAttribute::Local,
			.Name = arg->Name(),
		};
		if (isIdent)
		{
			param.Type = StructType{
				.Name = std::get<std::unique_ptr<Identifier>>(argType)->Name(),
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
	function->Body.emplace_back(LabelType{ "entry" });
	GenerateFuncParameters(astNode, *function);

	m_ir.emplace_back(std::move(function));
}

void IR::GenerateFuncParameters(FunctionDeclaration& astNode, Function& function)
{

}


}
