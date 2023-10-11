/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-08.
//

#pragma once
#include <string>
#include <variant>
#include <memory>
#include <vector>
#include "Types.h"
#include "../AST/Ast.h"

namespace alx::ir {

using BodyTypes = std::variant<LabelType>;

struct FunctionParameter {
	Types Type;
	std::vector<ParameterAttributes> Attributes;
	VisibilityAttribute Visibility;
	std::string Name;
	
	void PrintNode() const;
};

struct Function
{
	std::string Name;
	ParameterAttributes Attributes;
	LinkageType Linkage = LinkageType::Internal;
	VisibilityAttribute Visibility = VisibilityAttribute::Local;
	Types ReturnType;
	
	std::vector<FunctionParameter> Arguments;
	std::vector<BodyTypes> Body;
	
	void PrintNode() const;
};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
class IR
{
	using IRNodes = std::variant<std::unique_ptr<Function>>;
	const std::vector<std::unique_ptr<ASTNode>>& m_ast{};
	std::vector<IRNodes> m_ir;

public:
	explicit IR(const std::vector<std::unique_ptr<ASTNode>>& ast)
		: m_ast(ast) {}

	void Generate();
	[[nodiscard]] const std::vector<IRNodes>& GetIR() const { return m_ir; }
	void Dump();

	static std::string EnumToString(std::variant<LinkageType, ParameterAttributes>);
	static std::string TypesToString(const Types&);
	static Types TokenTypeToIRType(TokenType);
	
private:
	void GenerateFunction(FunctionDeclaration&);
	void GenerateFuncParameters(FunctionDeclaration& astNode, Function& function);
};

#pragma clang diagnostic pop

}