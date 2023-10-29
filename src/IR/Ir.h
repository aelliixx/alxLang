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
#include "Variables.h"
#include "Instructions.h"

namespace alx::ir {


struct FunctionParameter
{
	Types Type;
	std::vector<ParameterAttributes> Attributes;
	VisibilityAttribute Visibility;
	std::string Name;

	void PrintNode() const;
};

using BodyTypes = std::variant<LabelType, ReturnInst, Variable, StoreInst>;
class LogicalBlock
{
	std::vector<Variable> m_identifiers_cache;
public:
	LabelType Label;
	std::vector<BodyTypes> Body;

	[[nodiscard]] std::vector<Variable>& Identifiers()
	{
		if (!m_identifiers_cache.empty())
		{
			return m_identifiers_cache;
		}
		m_identifiers_cache.clear();
		for (const auto& types : Body)
		{
			if (std::holds_alternative<Variable>(types))
			{
				auto& ident = std::get<Variable>(types);
				m_identifiers_cache.push_back(std::get<Variable>(types));
			}
		}
		return m_identifiers_cache;
	}

	LogicalBlock() = default;
	explicit LogicalBlock(LabelType label) : Label(std::move(label)) {}
};

struct Function
{
	std::string Name;
	ParameterAttributes Attributes;
	LinkageType Linkage = LinkageType::Internal;
	VisibilityAttribute Visibility = VisibilityAttribute::Local;
	Types ReturnType;

	std::vector<FunctionParameter> Arguments;
	std::vector<LogicalBlock> Blocks;
	
	[[nodiscard]] std::shared_ptr<Variable> FindVariableByIdentifier(const std::string& name);
	void PrintNode() const;
	
	std::string GetNewUnnamedTemporary()
	{
		return std::to_string(UnnamedTemporaryCounter++);
	}
	size_t UnnamedTemporaryCounter = 0;
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
	void generate_function(FunctionDeclaration&);
	void generate_func_parameters(FunctionDeclaration& astNode, Function& function);
	void generate_func_body(FunctionDeclaration& astNode, Function& function);
	void generate_variable(VariableDeclaration& variable, Function& function);
	static void generate_binary_expression(BinaryExpression& astNode, Function& function);
	static void generate_return_statement(ReturnStatement& astNode, Function& function, bool hasReturned);
};

#pragma clang diagnostic pop

}