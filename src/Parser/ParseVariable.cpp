/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include "Parser.h"

namespace alx {
std::shared_ptr<VariableDeclaration> Parser::parse_variable()
{
	auto typeToken = consume();
	TypeExpression type;
	if (typeToken.Type == TokenType::T_IDENTIFIER)
		type = std::make_unique<Identifier>(typeToken.Value.value());
	else
		type = typeToken.Type;
	auto identToken = consume();
	auto identifier = std::make_unique<Identifier>(identToken.Value.value());
	if (std::find_if(m_variables.at(m_current_scope_name).begin(),
					 m_variables.at(m_current_scope_name).end(),
					 [&identifier](const VariableDeclaration* var)
					 {
					   return identifier->Name() == var->Name();
					 })
		!= m_variables.at(m_current_scope_name).end())
	{
		m_error->Error(
			identToken.LineNumber,
			identToken.ColumnNumber,
			identToken.PosNumber,
			"Redefinition of variable '{}'",
			identifier->Name());
	}
	// Direct assignment
	if (peek().has_value() && peek().value().Type == TokenType::T_EQ)
	{
		must_consume(TokenType::T_EQ); // Eat '='
		if (peek().has_value() && isNumberLiteral(peek().value().Type) &&
			peek(1).has_value() && peek(1).value().Type == TokenType::T_SEMI)
		{
			auto value = parse_number_literal();
			auto var = std::make_unique<VariableDeclaration>(std::move(type), std::move(identifier), std::move(value));
			add_variable(var.get());
			return std::move(var);
		}
		else if (peek().has_value() && peek().value().Type == TokenType::T_STR_L &&
			peek(1).has_value() && peek(1).value().Type == TokenType::T_SEMI)
		{
			auto string = parse_string_literal();
			auto var = std::make_unique<VariableDeclaration>(std::move(type), std::move(identifier), std::move(string));
			add_variable(var.get());
			return std::move(var);
		}
		else if (peek().has_value() && isUnaryOp(peek().value().Type))
		{
			auto unaryExpression = parse_unary_expression();
			return std::make_unique<VariableDeclaration>(std::move(type),
														 std::move(identifier),
														 std::move(unaryExpression));
		}
		TokenType expressionType;
		ValueExpression expression = parse_expression();
		size_t expressionSize;

		struct ValueVisitor
		{
			struct TypeVisitor
			{
				size_t size{};
				Parser& parser;
				Token expressionToken;
				void operator()(TokenType tokenType) const
				{
					if (size > size_of(tokenType))
					{
						parser.m_error->Warning(expressionToken.LineNumber,
												expressionToken.ColumnNumber + 2,
												expressionToken.PosNumber + 2,
												"Narrowing conversion from type '{}' to '{}'",
												token_to_string(TokenType::T_VOID),
												token_to_string(tokenType));
					}
				}
				void operator()(const RefPtr<Identifier>&)
				{
					ASSERT_NOT_IMPLEMENTED();
				}
			};

			Parser& parser;
			TokenType expressionType;
			size_t expressionSize;
			const Token expressionToken = parser.peek().value();
			void operator()(const RefPtr<Identifier>& identifier)
			{
				auto identIt = std::find_if(parser.m_variables.at(parser.m_current_scope_name).begin(),
											parser.m_variables.at(parser.m_current_scope_name).end(),
											[identifier](VariableDeclaration* var)
											{
											  return var->Name() == identifier->Name();
											});
				TypeVisitor visitor{ .size = expressionSize, .parser = parser, .expressionToken = expressionToken };
				std::visit(visitor, (*identIt)->Type());
			}
			void operator()(const RefPtr<MemberExpression>& memberExpression)
			{
				TypeVisitor visitor{ .size = expressionSize, .parser = parser, .expressionToken = expressionToken };
				std::visit(visitor, memberExpression->Type());
			}
			void operator()(const RefPtr<NumberLiteral>&)
			{
			}
			void operator()(const RefPtr<StringLiteral>&)
			{
			}
			void operator()(const RefPtr<BinaryExpression>&)
			{
			}
			void operator()(const RefPtr<UnaryExpression>&)
			{
			}

		};
		ValueVisitor visitor{ .parser = *this, .expressionType = expressionType, .expressionSize = expressionSize };
		std::visit(visitor, expression);

		auto var = std::make_shared<VariableDeclaration>(std::move(type),
														 std::move(identifier), expression);
		add_variable(var.get());
		return std::move(var);
	}
		// Declaration
	else if (peek().has_value() && peek().value().Type == TokenType::T_SEMI)
	{
		auto var = std::make_shared<VariableDeclaration>(std::move(type), std::move(identifier));
		add_variable(var.get());
		return std::move(var);
	}
	assert(false && "Not reachable");
}
}
