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
#include "../AST/Ast.h"

namespace alx::ir {

enum class Linkage
{
	External, // Exported
	Internal
};

enum class ParameterAttributes
{
	ZeroExt,
	SignExt,
	Align,
};

enum class SingleValueType
{
	Void,
	Int,
	Half,
	Float,
	Double
};

struct Function
{
	std::string Name;
	Linkage Linkage;
	std::variant<SingleValueType> ReturnType;

	void PrintNode();
};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
class IR
{
	const std::vector<std::unique_ptr<ASTNode>>& m_ast{};

public:
	explicit IR(const std::vector<std::unique_ptr<ASTNode>>& ast)
		: m_ast(ast) {}

	void Generate();

	void Dump();

	static std::string EnumToString(std::variant<Linkage, ParameterAttributes, SingleValueType>, int = 0);
};

#pragma clang diagnostic pop

}