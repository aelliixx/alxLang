/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-06.
//

#pragma once

#include <string>
#include <utility>
#include <memory>
#include <vector>
#include <variant>
#include "../Tokeniser/Tokeniser.h"
#include "Types.h"

namespace alx {

class ASTNode;

class ASTNode
{
protected:
public:
	[[maybe_unused]] [[nodiscard]] virtual std::string class_name() const = 0;
	[[maybe_unused]] virtual void PrintNode(int) const = 0;
	virtual ~ASTNode() = default;
};

class Expression : public ASTNode
{
public:
	[[nodiscard]] std::string class_name() const override { return "Expression"; }
	[[maybe_unused]] void PrintNode(int indent) const override;

};

class ScopeNode : public ASTNode
{
	std::vector<std::unique_ptr<ASTNode>> m_children;
protected:
	[[maybe_unused]] static void dump_nodes(const std::vector<std::unique_ptr<ASTNode>>&, int indent);
	ScopeNode() = default;
public:
	template<typename T, typename ...Args>
	T& Append(Args&& ...args)
	{
		auto child = std::make_unique<T>(std::forward<Args>(args)...);
		m_children.push_back(std::move(child));
		return *static_cast<T*>(m_children.back().get());
	}
	void Append(std::unique_ptr<ASTNode> node)
	{
		m_children.push_back(std::move(node));
	}

	[[maybe_unused]] void PrintNode(int indent) const override;
	[[nodiscard]] const std::vector<std::unique_ptr<ASTNode>>& Children() const { return m_children; }
};

class Program : public ScopeNode
{
	[[nodiscard]] std::string class_name() const override { return "Program"; }

public:
	[[maybe_unused]] void PrintNode(int indent) const override;
	Program() = default;
};

class BlockStatement : public ScopeNode
{
	[[nodiscard]] std::string class_name() const override { return "BlockStatement"; }
	[[maybe_unused]] void PrintNode(int indent) const override;

public:
	BlockStatement() = default;
};

class Identifier : public Expression
{
	[[nodiscard]] std::string class_name() const override { return "Identifier"; }
	std::string m_name;
	bool m_assignable{ true };

public:
	[[maybe_unused]] void PrintNode(int indent) const override;

	explicit Identifier(std::string name) : m_name(std::move(name)) {}
	Identifier(std::string name, bool assignable) : m_name(std::move(name)), m_assignable(assignable) {}

	[[nodiscard]] const std::string& Name() const { return m_name; }
	[[nodiscard]] bool Assignable() const { return m_assignable; }
};

class NumberLiteral : public Expression
{
	[[nodiscard]] std::string class_name() const override { return "NumberLiteral"; }

	TokenType m_type;
	std::string m_value;
	bool is_unsigned{}; // FIXME

public:
	[[maybe_unused]] void PrintNode(int indent) const override;

	NumberLiteral(TokenType type, std::string  value)
		: m_type(type),
		  m_value(std::move(value)) {}

	[[nodiscard]] TokenType Type() const { return m_type; }
	[[nodiscard]] const std::string& Value() const { return m_value; }
};

class StringLiteral : public Expression
{
	[[nodiscard]] std::string class_name() const override { return "NumberLiteral"; }

	std::string m_value;

public:
	[[maybe_unused]] void PrintNode(int indent) const override;

	explicit StringLiteral(std::string value)
		: m_value(std::move(value)) {}

	[[nodiscard]] std::string Value() const { return m_value; }
	[[nodiscard]] uint Lenght() const { return m_value.length(); }

};

class BinaryExpression : public Expression
{
	[[nodiscard]] std::string class_name() const override { return "BinaryExpression"; }
	std::unique_ptr<Expression> m_lhs, m_rhs;
	TokenType m_binaryOp;
	bool m_constexpr{};
	std::optional<std::variant<NumberLiteral, StringLiteral>> m_value; // Only valid when constexpr
	bool m_operands_match{};
public:
	BinaryExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, TokenType binaryOp);

	[[maybe_unused]] void PrintNode(int indent) const override;
	[[nodiscard]] TokenType Operator() const { return m_binaryOp; }
	[[nodiscard]] Expression* LHS() const { return m_lhs.get(); }
	[[nodiscard]] Expression* RHS() const { return m_rhs.get(); }
	[[nodiscard]] bool Constexpr() const { return m_constexpr; }
	[[nodiscard]] bool OperandsMatch() const { return m_operands_match; }
	[[nodiscard]] std::unique_ptr<NumberLiteral> Evaluate() const;
};

class VariableDeclaration : public ASTNode
{
	[[nodiscard]] std::string class_name() const override { return "VariableDeclaration"; }

	TokenType m_type;
	std::unique_ptr<Identifier> m_identifier;
	std::unique_ptr<Expression> m_value;
public:
	[[maybe_unused]] void PrintNode(int indent) const override;

	VariableDeclaration(TokenType type, std::unique_ptr<Identifier> identifier, std::unique_ptr<Expression> value)
		: m_type(type),
		  m_identifier(std::move(identifier)),
		  m_value(std::move(value))
	{
		assert(is_number_type(m_type) && "Invalid variable type");
	}

	VariableDeclaration(TokenType type, std::unique_ptr<Identifier> identifier)
		: m_type(type),
		  m_identifier(std::move(identifier))
	{
		assert(is_number_type(m_type) && "Invalid variable type");
	}

	void AssignValue(std::unique_ptr<Expression> expression)
	{
		m_value = std::move(expression);
	}

	[[nodiscard]] TokenType Type() const { return m_type; }
	[[nodiscard]] const Identifier& Ident() const { return *m_identifier; }
	[[nodiscard]] Expression* Value() const { return m_value.get(); }
	[[nodiscard]] const std::string& Name() const { return m_identifier->Name(); }
};

class FunctionDeclaration : public ASTNode
{
	[[nodiscard]] std::string class_name() const override { return "FunctionDeclaration"; }

	TokenType m_return_type;
	std::unique_ptr<Identifier> m_identifier;
	std::vector<VariableDeclaration> m_arguments;
	std::unique_ptr<ScopeNode> m_body;

public:
	explicit FunctionDeclaration(TokenType return_type,
								 std::unique_ptr<Identifier> name,
								 std::unique_ptr<ScopeNode> body)
		: m_return_type(return_type),
		  m_identifier(std::move(name)),
		  m_body(std::move(body)) {}

	[[maybe_unused]] void PrintNode(int indent) const override;
	[[nodiscard]] const Identifier& Ident() const { return *m_identifier; }
	[[nodiscard]] const std::string& Name() const { return m_identifier->Name(); }
	[[nodiscard]] const ScopeNode& Body() const { return *m_body; }
};

class ReturnStatement : public ASTNode
{
	[[nodiscard]] std::string class_name() const override { return "ReturnStatement"; }
	std::unique_ptr<Expression> m_argument;
public:
	[[maybe_unused]] void PrintNode(int indent) const override;
	explicit ReturnStatement(std::unique_ptr<Expression> argument) : m_argument(std::move(argument)) {}
	[[nodiscard]] Expression* Argument() const { return m_argument.get(); }
};

}