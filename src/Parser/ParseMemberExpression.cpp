/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-16.
//

#include "Parser.h"

namespace alx {

std::unique_ptr<MemberExpression> Parser::parse_member_expression()
{
	auto identifier = must_consume(TokenType::T_IDENTIFIER);
	auto identPtr = std::make_unique<Identifier>(identifier.Value.value());
	auto accessor = consume().Type; // This is always either a '.', '->' or '::' as checked by Parser::parse_term()
	auto property = must_consume(TokenType::T_IDENTIFIER);
	auto memPtr = std::make_unique<Identifier>(property.Value.value());
	auto scopeVars = m_variables.at(m_current_scope_name);

	// Find the identifier in the AST
	auto object = std::find_if(scopeVars.begin(), scopeVars.end(), [&identPtr](VariableDeclaration* var)
	{
	  return identPtr->Name() == var->Name();
	});
	if (object == scopeVars.end())
		m_error->Error(identifier.LineNumber,
					   identifier.ColumnNumber,
					   identifier.PosNumber,
					   "Use of undeclared identifier '{}'",
					   identPtr.get()->Name());

	// TODO: Need to rethink this when the class or struct definitions get hoisted eventually
	// TODO: Verify that the access level is not private or protected in this context
	// Find the initialisation of the struct or class
	const auto& structDecl =
		std::find_if(m_program->GetChildren().begin(), m_program->GetChildren().end(), [&object](const auto& child)
		{
		  if (child->class_name() == "StructDeclaration")
		  {
			  const auto& it = static_cast<StructDeclaration&>(*child);
			  return it.Name() == (*object)->TypeName();
		  }
		  return false;
		});
	// Find the accessed variable in the struct
	if (structDecl != m_program->GetChildren().end())
	{
		const auto& structDeclaration = static_cast<StructDeclaration&>(**structDecl);
		const auto& member = std::find_if(structDeclaration.Members().begin(),
										  structDeclaration.Members().end(),
										  [&property](const auto& var)
										  {
											const auto& mem = static_cast<VariableDeclaration&>(*var);
											return mem.Name() == property.Value.value();
										  });
		if (member != structDeclaration.Members().end())
			return std::make_unique<MemberExpression>(accessor, std::move(identPtr), std::move(memPtr),
													  static_cast<VariableDeclaration&>(**member).TypeAsPrimitive());

		m_error->Error(property.LineNumber,
					   property.ColumnNumber,
					   property.PosNumber,
					   "No member '{}' in '{}'",
					   property.Value.value(),
					   identifier.Value.value());
	}
	// If not found in struct, try classes
	const auto& classDecl =
		std::find_if(m_program->GetChildren().begin(), m_program->GetChildren().end(), [&object](const auto& child)
		{
		  if (child->class_name() == "ClassDeclaration")
		  {
			  const auto& it = static_cast<ClassDeclaration&>(*child);
			  return it.Name() == (*object)->Name();
		  }
		  return false;
		});
	// Find the accessed variable in the class
	if (classDecl != m_program->GetChildren().end())
	{
		const auto& classDeclaration = static_cast<ClassDeclaration&>(**classDecl);
		const auto& member = std::find_if(classDeclaration.Members().begin(),
										  classDeclaration.Members().end(),
										  [&property](const auto& var)
										  {
											const auto& mem = static_cast<VariableDeclaration&>(*var);
											return mem.Name() == property.Value.value();
										  });
		if (member != classDeclaration.Members().end())
			return std::make_unique<MemberExpression>(accessor, std::move(identPtr), std::move(memPtr),
													  static_cast<VariableDeclaration&>(**member).TypeAsPrimitive());

		m_error->Error(property.LineNumber,
					   property.ColumnNumber,
					   property.PosNumber,
					   "No member '{}' in '{}'",
					   property.Value.value(),
					   identifier.Value.value());
	}
	ASSERT_NOT_REACHABLE();
}

}