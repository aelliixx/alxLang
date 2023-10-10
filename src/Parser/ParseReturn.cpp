/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#include "Parser.h"
namespace alx {

RefPtr<ReturnStatement> Parser::parse_return_statement()
{
	auto returnToken = consume(); // Eat 'return'

	// Get return type of the current function
	auto funcIterator = std::find_if(m_program->GetChildren().begin(), m_program->GetChildren().end(),
									 [&](const std::unique_ptr<ASTNode>& node)
									 {
									   if (node->class_name() == "FunctionDeclaration")
									   {
										   return static_cast<FunctionDeclaration&>(*node).Name()
											   == m_current_scope_name;
									   }
									   return false;
									 });

	assert(funcIterator != m_program->GetChildren().end() && "Function not found");
	const auto& func = static_cast<FunctionDeclaration&>(**funcIterator);
	auto returnType = func.ReturnType();

	if (returnType == TokenType::T_VOID)
	{
		auto nextToken = peek();
		if (nextToken.has_value() && nextToken.value().Type != TokenType::T_SEMI)
		{
			m_error->Error(returnToken.LineNumber,
						   returnToken.ColumnNumber,
						   returnToken.PosNumber,
						   "Void function '{}' should not return a value",
						   m_current_scope_name);
		}
	}
	auto expr = parse_expression();

	if (expr->class_name() == "NumberLiteral")
	{
		auto& numLit = static_cast<NumberLiteral&>(*expr);
		if (numLit.Type() != returnType)
			m_error->Error(returnToken.LineNumber,
						   returnToken.ColumnNumber,
						   returnToken.PosNumber,
						   "Function '{}' must return '{}'",
						   m_current_scope_name,
						   token_to_string(returnType));
	}
	else if (expr->class_name() == "StringLiteral")
	{
		if (TokenType::T_STRING != returnType)
			m_error->Error(returnToken.LineNumber,
						   returnToken.ColumnNumber,
						   returnToken.PosNumber,
						   "Function '{}' must return '{}'",
						   m_current_scope_name,
						   token_to_string(returnType));
	}
	else if (expr->class_name() == "MemberExpression")
	{
		ASSERT_NOT_IMPLEMENTED();
		auto& memberExpression = static_cast<MemberExpression&>(*expr);
		if (TokenType::T_STRING != returnType)
			m_error->Error(returnToken.LineNumber,
						   returnToken.ColumnNumber,
						   returnToken.PosNumber,
						   "Function '{}' must return '{}'",
						   m_current_scope_name,
						   token_to_string(returnType));
	}

	return std::make_unique<ReturnStatement>(std::move(expr));
}
};
