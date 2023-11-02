/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-08.
//
#ifdef __clang__
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
#endif
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
class IR;

struct FunctionParameter {
	Types Type{};
	std::vector<ParameterAttributes> Attributes{};
	VisibilityAttribute Visibility = VisibilityAttribute::Local;
	std::string Name;
	void PrintNode(IR& ir) const;
};

using BodyTypes = std::variant<LabelType, ReturnInst, Variable, StoreInst, BranchInst>;
class LogicalBlock
{
	std::vector<Variable> m_identifiers_cache;

public:
	LabelType Label;
	std::vector<BodyTypes> Body;

	[[nodiscard]] std::vector<Variable>& Identifiers()
	{
		if (!m_identifiers_cache.empty()) {
			return m_identifiers_cache;
		}
		m_identifiers_cache.clear();
		for (const auto& types : Body) {
			if (std::holds_alternative<Variable>(types)) {
				m_identifiers_cache.push_back(std::get<Variable>(types));
			}
		}
		return m_identifiers_cache;
	}

	LogicalBlock() = default;
	explicit LogicalBlock(LabelType label) : Label(std::move(label)) {}
};

struct Function {
	std::string Name;
	ParameterAttributes Attributes{};
	LinkageType Linkage = LinkageType::Internal;
	VisibilityAttribute Visibility = VisibilityAttribute::Local;
	Types ReturnType;

	std::vector<FunctionParameter> Arguments{};
	std::vector<LogicalBlock> Blocks{};

	[[nodiscard]] std::shared_ptr<Variable> FindVariableByIdentifier(const std::string& name);
	[[nodiscard]] LogicalBlock& GetBlockByLabel(const std::string& label);

	void PrintNode(IR&) const;

	size_t UnnamedTemporaryCounter = 0;
	
	std::map<std::string, size_t> NamedTemporaries{};

	void AppendInstruction(const BodyTypes& body) { Blocks.back().Body.push_back(body); }
	
	void AppendBlock(const LogicalBlock& block) { Blocks.push_back(block); }
	
	std::string GetNewUnnamedTemporary() { return std::to_string(UnnamedTemporaryCounter++); }
	std::string GetNewNamedTemporary(const std::string& name)
	{
		auto it = NamedTemporaries.find(name);
		if (it == NamedTemporaries.end()) {
			NamedTemporaries[name] = 1;
			return name;
		}
		return name + "." + std::to_string(it->second++);
	}
};

#ifdef __clang__
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
#endif
class IR
{
	using IRNodes = std::variant<std::unique_ptr<Function>>;
	const std::vector<std::unique_ptr<ASTNode>>& m_ast{};
	std::vector<IRNodes> m_ir;
	std::string m_ir_string;
	friend void FunctionParameter::PrintNode(IR& ir) const;
	friend void Function::PrintNode(IR&) const;
	friend void Variable::PrintNode(IR& ir) const;

public:
	explicit IR(const std::vector<std::unique_ptr<ASTNode>>& ast) : m_ast(ast) {}

	void Generate();
	[[nodiscard]] const std::vector<IRNodes>& GetIR() const { return m_ir; }
	void Dump();

	static std::string EnumToString(std::variant<LinkageType, ParameterAttributes>);
	static std::string TypesToString(const Types&);
	static Types TokenTypeToIRType(TokenType);
	static Values NumberLiteralToValue(const NumberLiteral&, size_t);

#if OUTPUT_IR_TO_STRING
	[[nodiscard]] const std::string& GetIRString() const { return m_ir_string; }
#endif

private:
	void generate_function(FunctionDeclaration&);
	void generate_func_parameters(FunctionDeclaration&, Function&);
	void generate_body(const BlockStatement& functionDeclaration, Function& function);
	void generate_variable(const VariableDeclaration&, Function&);
	static void generate_return_statement(const ReturnStatement&, Function&, bool);
	std::optional<Values> generate_binary_expression(const BinaryExpression&, Function&);
	
	[[nodiscard]] std::optional<std::shared_ptr<Variable>> generate_unary_expression(const UnaryExpression&, Function&);
	template<typename Func>
	[[nodiscard]] std::optional<std::shared_ptr<Variable>> generate_unary_op(const UnaryExpression&,
																			 Function&, Func);
	
	[[nodiscard]] std::optional<Values> generate_bin_eq(const BinaryExpression&, Function&);
	template<typename Func>
	[[nodiscard]] std::optional<std::shared_ptr<Variable>> generate_binary_op(const BinaryExpression& binaryExpression,
																			  Function& function,
																			  Func instruction);
	
	void generate_if_statement(IfStatement&, Function&);
	void generate_while_statement(WhileStatement&, Function&);
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

} // namespace alx::ir
#ifdef __clang__
#pragma clang diagnostic pop
#endif