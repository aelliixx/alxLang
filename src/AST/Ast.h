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
#include "../Utils/Types.h"
#include "../libs/Println.h"

namespace alx {
enum class AccessModeType
{
	a_global = 0,        // Global functions, global classes and structs, global enums
	a_scoped = 1,        // Local variables
	a_public = 2,
	a_protected = 3,
	a_private = 4
};

class ASTNode
{
protected:
public:
	[[maybe_unused]] [[nodiscard]] virtual std::string class_name() const = 0;
	[[maybe_unused]] virtual void PrintNode(int) const = 0;
	virtual ~ASTNode() = default;
	class StructDeclaration;
};

class Expression : public ASTNode
{
public:
	[[nodiscard]] std::string class_name() const override { return "Expression"; }
	[[maybe_unused]] void PrintNode(int indent) const override;
};

class ScopeNode : public ASTNode
{
protected:
	std::vector<std::unique_ptr<ASTNode>> m_children;
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
	[[nodiscard]] const std::vector<std::unique_ptr<ASTNode>>& GetChildren() const { return m_children; }
};

class BlockStatement : public ScopeNode
{
	[[nodiscard]] std::string class_name() const override { return "BlockStatement"; }

public:
	[[maybe_unused]] void PrintNode(int indent) const override;
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
	bool m_is_unsigned{}; // FIXME

public:
	[[maybe_unused]] void PrintNode(int indent) const override;

	NumberLiteral(TokenType type, std::string value)
		: m_type(type),
		  m_value(std::move(value))
	{
		std::string::size_type n;
		while ((n = m_value.find(',')) != std::string::npos)
			m_value.replace(n, 1, "");
	}

	[[nodiscard]] TokenType Type() const { return m_type; }
	[[nodiscard]] const std::string& Value() const { return m_value; }
	[[nodiscard]] long AsInt() const { return std::stoi(m_value); }
	[[nodiscard]] float AsFloat() const { return std::stof(m_value); }
	[[nodiscard]] double AsDouble() const { return std::stod(m_value); }
	[[nodiscard]] long AsBoolNum() const { return !!std::stoi(m_value); }
	[[nodiscard]] std::string AsBool() const
	{
		return AsInt() ? "true" : "false";
	}
};

class StringLiteral : public Expression
{
	[[nodiscard]] std::string class_name() const override { return "StringLiteral"; }

	std::string m_value;

public:
	[[maybe_unused]] void PrintNode(int indent) const override;

	explicit StringLiteral(std::string value)
		: m_value(std::move(value)) {}

	[[nodiscard]] std::string Value() const { return m_value; }
	[[nodiscard]] uint Length() const { return m_value.length(); }

};

class BinaryExpression : public Expression
{
	[[nodiscard]] std::string class_name() const override { return "BinaryExpression"; }
	std::unique_ptr<Expression> m_lhs, m_rhs;
	TokenType m_binary_op;
	bool m_constexpr{};
	std::optional<std::variant<NumberLiteral, StringLiteral>> m_value; // Only valid when constexpr
	bool m_operands_match{};
public:
	BinaryExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, TokenType binaryOp);

	[[maybe_unused]] void PrintNode(int indent) const override;
	[[nodiscard]] TokenType Operator() const { return m_binary_op; }
	[[nodiscard]] Expression* Lhs() const { return m_lhs.get(); }
	[[nodiscard]] Expression* Rhs() const { return m_rhs.get(); }
	[[nodiscard]] bool Constexpr() const { return m_constexpr; }
	[[nodiscard]] bool OperandsMatch() const { return m_operands_match; }
	[[nodiscard]] std::unique_ptr<NumberLiteral> Evaluate() const;
};

class UnaryExpression : public Expression
{
	[[nodiscard]] std::string class_name() const override { return "UnaryExpression"; }
	std::unique_ptr<Expression> m_rhs;
	TokenType m_unary_op;

public:
	[[maybe_unused]] void PrintNode(int indent) const override;
	UnaryExpression(std::unique_ptr<Expression> rhs, TokenType op) : m_rhs(std::move(rhs)), m_unary_op(op) {}

	[[nodiscard]] TokenType Operator() const { return m_unary_op; }
	[[nodiscard]] Expression* Rhs() const { return m_rhs.get(); }
};

class VariableDeclaration : public ASTNode
{
	[[nodiscard]] std::string class_name() const override { return "VariableDeclaration"; }

	std::variant<TokenType, std::unique_ptr<Identifier>> m_type;
	std::unique_ptr<Identifier> m_identifier;
	std::unique_ptr<Expression> m_value;
	AccessModeType m_access_mode = AccessModeType::a_scoped;
public:
	[[maybe_unused]] void PrintNode(int indent) const override;

	VariableDeclaration(std::variant<TokenType, std::unique_ptr<Identifier>> type,
						std::unique_ptr<Identifier> identifier,
						std::unique_ptr<Expression> value)
		: m_type(std::move(type)),
		  m_identifier(std::move(identifier)),
		  m_value(std::move(value))
	{
	}

	VariableDeclaration(std::variant<TokenType, std::unique_ptr<Identifier>> type,
						std::unique_ptr<Identifier> identifier,
						std::unique_ptr<Expression> value,
						AccessModeType accessMode)
		: m_type(std::move(type)),
		  m_identifier(std::move(identifier)),
		  m_value(std::move(value)),
		  m_access_mode(accessMode)
	{
	}

	VariableDeclaration(std::variant<TokenType, std::unique_ptr<Identifier>> type,
						std::unique_ptr<Identifier> identifier)
		: m_type(std::move(type)),
		  m_identifier(std::move(identifier))
	{
	}

	VariableDeclaration(std::variant<TokenType, std::unique_ptr<Identifier>> type,
						std::unique_ptr<Identifier> identifier,
						AccessModeType accessMode)
		: m_type(std::move(type)),
		  m_identifier(std::move(identifier)),
		  m_access_mode(accessMode)
	{
	}

	void AssignValue(std::unique_ptr<Expression> expression)
	{
		m_value = std::move(expression);
	}

	[[nodiscard]] AccessModeType AccessMode() const { return m_access_mode; }
	[[nodiscard]] const Identifier& Ident() const { return *m_identifier; }
	[[nodiscard]] Expression* Value() const { return m_value.get(); }
	[[nodiscard]] const std::string& Name() const { return m_identifier->Name(); }
	[[nodiscard]] auto& Type() const { return m_type; }
	[[nodiscard]] size_t TypeIndex() const { return m_type.index(); }
	[[nodiscard]] TokenType TypeAsPrimitive() const { return std::get<TokenType>(m_type); }
	[[nodiscard]] Identifier* TypeAsIdentifier() const { return std::get<std::unique_ptr<Identifier>>(m_type).get(); }
	[[nodiscard]] std::string TypeName() const
	{
		if (m_type.index() == 0)
			return token_to_string(std::get<0>(m_type));
		if (m_type.index() == 1)
			return std::get<1>(m_type)->Name();
		ASSERT_NOT_REACHABLE();
	}
};

class IfStatement : public ScopeNode
{
	[[nodiscard]] std::string class_name() const override { return "IfStatement"; }
	std::unique_ptr<Expression> m_condition;
	std::unique_ptr<BlockStatement> m_body;
	std::optional<std::unique_ptr<ScopeNode>> m_alternate; // This can be a block statement or another if statement
public:
	[[maybe_unused]] void PrintNode(int indent) const override;

	IfStatement(std::unique_ptr<Expression> expression,
				std::unique_ptr<BlockStatement> body)
		: m_condition(std::move(expression)),
		  m_body(std::move(body)) {}

	void SetCondition(const Expression& expression)
	{
		m_condition = std::make_unique<Expression>(expression);
	}
	void SetAlternate(std::unique_ptr<ScopeNode> alternate)
	{
		m_alternate = std::move(alternate);
	}

	[[nodiscard]] Expression* Condition() const { return m_condition.get(); }
	[[nodiscard]] const std::optional<std::unique_ptr<ScopeNode>>& Alternate() const { return m_alternate; }
	[[nodiscard]] bool HasAlternate() const { return m_alternate.has_value(); }
	[[nodiscard]] ScopeNode* GetAlternate() const { return m_alternate.value().get(); }
	[[nodiscard]] const BlockStatement& Body() const { return *m_body; }
};

class WhileStatement : public ScopeNode
{
	[[nodiscard]] std::string class_name() const override { return "WhileStatement"; }
	std::unique_ptr<Expression> m_condition;
	std::unique_ptr<BlockStatement> m_body;
public:
	[[maybe_unused]] void PrintNode(int indent) const override;

	WhileStatement(std::unique_ptr<Expression> expression,
				   std::unique_ptr<BlockStatement> body)
		: m_condition(std::move(expression)),
		  m_body(std::move(body)) {}

	void SetCondition(const Expression& expression)
	{
		m_condition = std::make_unique<Expression>(expression);
	}

	[[nodiscard]] Expression* Condition() const { return m_condition.get(); }
	[[nodiscard]] const BlockStatement& Body() const { return *m_body; }
	[[nodiscard]] BlockStatement* BodyPtr() const { return m_body.get(); }
};

class FunctionDeclaration : public ScopeNode
{

	[[nodiscard]] std::string class_name() const override { return "FunctionDeclaration"; }

	TokenType m_return_type;
	std::unique_ptr<Identifier> m_identifier;
	std::vector<std::unique_ptr<VariableDeclaration>> m_parameters;
	std::unique_ptr<BlockStatement> m_body;
	AccessModeType m_access_mode = AccessModeType::a_global;

public:
	FunctionDeclaration(TokenType returnType,
						std::unique_ptr<Identifier> name,
						std::unique_ptr<BlockStatement> body,
						std::vector<std::unique_ptr<VariableDeclaration>> args)
		: m_return_type(returnType),
		  m_identifier(std::move(name)),
		  m_body(std::move(body)),
		  m_parameters(std::move(args)) {}

	FunctionDeclaration(TokenType returnType,
						std::unique_ptr<Identifier> name,
						std::unique_ptr<BlockStatement> body,
						std::vector<std::unique_ptr<VariableDeclaration>> args,
						AccessModeType accessMode)
		: m_return_type(returnType),
		  m_identifier(std::move(name)),
		  m_body(std::move(body)),
		  m_parameters(std::move(args)),
		  m_access_mode(accessMode) {}

	[[maybe_unused]] void PrintNode(int indent) const override;
	[[nodiscard]] TokenType ReturnType() const { return m_return_type; }
	[[nodiscard]] AccessModeType AccessMode() const { return m_access_mode; }
	[[nodiscard]] const Identifier& Ident() const { return *m_identifier; }
	[[nodiscard]] const std::string& Name() const { return m_identifier->Name(); }
	[[nodiscard]] const BlockStatement& Body() const { return *m_body; }
	[[nodiscard]] const std::vector<std::unique_ptr<VariableDeclaration>>& Arguments() const { return m_parameters; }
	[[nodiscard]] size_t Argc() const { return m_parameters.size(); }
};

class StructDeclaration : public ASTNode
{
	[[nodiscard]] std::string class_name() const override { return "StructDeclaration"; }
	std::string m_name;
	std::vector<std::unique_ptr<ASTNode>> m_members;
	std::vector<std::unique_ptr<ASTNode>> m_methods;
	size_t m_size{};

public:
	[[maybe_unused]] void PrintNode(int indent) const override;
	StructDeclaration(std::string name,
					  std::vector<std::unique_ptr<ASTNode>> members,
					  std::vector<std::unique_ptr<ASTNode>> methods);

	[[nodiscard]] const std::string& Name() const { return m_name; }
	[[nodiscard]] const std::vector<std::unique_ptr<ASTNode>>& Members() const { return m_members; }
	[[nodiscard]] const std::vector<std::unique_ptr<ASTNode>>& Methods() const { return m_methods; }
	[[nodiscard]] size_t Size() const { return m_size; }
};

class ClassDeclaration : public StructDeclaration
{
	[[nodiscard]] std::string class_name() const override { return "ClassDeclaration"; }

};

class MemberExpression : public Expression
{
	[[nodiscard]] std::string class_name() const override { return "MemberExpression"; }
	std::unique_ptr<Identifier> m_object;
	std::unique_ptr<Identifier> m_member;
	std::variant<TokenType, std::unique_ptr<Identifier>> m_type;
	TokenType m_accessor;
public:
	[[maybe_unused]] void PrintNode(int indent) const override;

	MemberExpression(TokenType accessor,
					 std::unique_ptr<Identifier> object,
					 std::unique_ptr<Identifier> member,
					 std::variant<TokenType, std::unique_ptr<Identifier>> type)
		: m_accessor(accessor),
		  m_object(std::move(object)),
		  m_member(std::move(member)),
		  m_type(std::move(type)) {}

	[[nodiscard]] const Identifier& Object() const { return *m_object; }
	[[nodiscard]] const Identifier& Member() const { return *m_member; }
	[[nodiscard]] auto* Type() const { return &m_type; }
	[[nodiscard]] TokenType TypeAsPrimitive() const { return std::get<0>(m_type); }
	[[nodiscard]] Identifier* TypeAsIdentifier() const { return std::get<1>(m_type).get(); }
	[[nodiscard]] size_t TypeIndex() const { return m_type.index(); }
	[[nodiscard]] TokenType Accessor() const { return m_accessor; }

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