/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-08.
//

#include "Ir.h"
#include "../libs/Println.h"
#include "Instructions.h"

namespace alx::ir {

void IR::Generate()
{
	println("Generating IR:");
	for (const auto& node : m_ast)
	{
		if (node->class_name() == "FunctionDeclaration")
		{
			auto& func = static_cast<FunctionDeclaration&>(*node);
			GenerateFunction(func);
		}
		else
		{
			println(Colour::Red, "Unknown node type: {;255;255;255}", node->class_name());
//			ASSERT_NOT_IMPLEMENTED();
		}
	}
	println("Done generating IR");
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

	LogicalBlock entry{{ "entry" }};
	function->Blocks.push_back(entry);
	GenerateFuncParameters(astNode, *function);

	// Generate function body
	GenerateFuncBody(astNode, *function);

	m_ir.emplace_back(std::move(function));
}

void IR::GenerateFuncParameters(FunctionDeclaration& astNode, Function& function)
{

}

void IR::GenerateFuncBody(FunctionDeclaration& astNode, Function& function)
{
	for (const auto& node : astNode.Body().Children())
	{
		if (node->class_name() == "ReturnStatement")
			GenerateReturnStatement(static_cast<ReturnStatement&>(*node), function);
		else
			println(Colour::Red, "Unknown node type: {;255;255;255}", node->class_name());
	}
}

void IR::GenerateReturnStatement(ReturnStatement& astNode, Function& function)
{
	LogicalBlock ret{{ "ret" }};

	if (astNode.Argument()->class_name() == "NumberLiteral")
	{
		auto& numLit = static_cast<NumberLiteral&>(*astNode.Argument());
		auto value = numLit;
		if (isIntegerLiteral(value.Type()))
		{
			Constant returnValue{ .Type = IntType{ size_of(value.Type()) }, .Value = value.AsInt() };
			ret.Body.emplace_back(ReturnInst{ returnValue });
		}
		else if (!isIntegerLiteral(value.Type()) && isNumberLiteral(value.Type()))
		{
			SingleValueType type;
			if (value.Type() == TokenType::T_FLOAT_L)
				type = SingleValueType::Float;
			else if (value.Type() == TokenType::T_DOUBLE_L)
				type = SingleValueType::Double;
			Constant returnValue{ .Type = type, .Value = value.AsDouble() };
			ret.Body.emplace_back(ReturnInst{ returnValue });
		}
		else
			println(Colour::Red, "Unknown number type: {;255;255;255}",
					token_to_string(value.Type()));
	}
	else if (astNode.Argument()->class_name() == "BinaryExpression")
	{
		auto& binExpr = static_cast<BinaryExpression&>(*astNode.Argument());
		if (binExpr.Constexpr())
		{
			auto value = binExpr.Evaluate();
			if (isIntegerLiteral(value->Type()))
			{
				Constant returnValue{ .Type = IntType{ size_of(value->Type()) }, .Value = value->AsInt() };
				ret.Body.emplace_back(ReturnInst{ returnValue });
			}
			else if (!isIntegerLiteral(value->Type()) && isNumberLiteral(value->Type()))
			{
				SingleValueType type;
				if (value->Type() == TokenType::T_FLOAT_L)
					type = SingleValueType::Float;
				else if (value->Type() == TokenType::T_DOUBLE_L)
					type = SingleValueType::Double;

				Constant returnValue{ .Type = type, .Value = value->AsDouble() };
				ret.Body.emplace_back(ReturnInst{ returnValue });
			}
			else
				println(Colour::Red, "Unknown number type: {;255;255;255}",
						token_to_string(value->Type()));

		}
	}
	else
	{
		println(Colour::Red, "Unknown node type: {;255;255;255}", astNode.Argument()->class_name());
	}

	if (function.Blocks.size() > 1)
		function.Blocks.emplace_back(std::move(ret));
	else
		function.Blocks[0].Body.insert(std::end(function.Blocks[0].Body),
									   std::begin(ret.Body),
									   std::end(ret.Body));
}

}
