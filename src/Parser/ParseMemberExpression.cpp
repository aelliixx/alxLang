/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-16.
//

#include "Parser.h"
#include "../libs/Error.h"

namespace alx {

std::unique_ptr<MemberExpression> Parser::parse_member_expression()
{
	auto identifier = must_consume(TokenType::T_IDENTIFIER);
	auto ident_ptr = std::make_unique<Identifier>(identifier.value.value());
	auto accessor = consume().type; // This is always either a '.', '->' or '::' as checked by Parser::parse_term()
	auto property = must_consume(TokenType::T_IDENTIFIER);
	auto mem_ptr = std::make_unique<Identifier>(property.value.value());
	auto scope_vars = m_variables.at(m_current_scope_name);

	// Find the identifier in the AST
	auto object = std::find_if(scope_vars.begin(), scope_vars.end(), [&ident_ptr](VariableDeclaration* var){
	  return ident_ptr->Name() == var->Name();
	});
	if (object == scope_vars.end())
		error("Use of undeclared identifier '{}' at line: {}, in position: {}",
			  ident_ptr.get()->Name(),
			  identifier.lineNumber,
			  identifier.columnNumber);

	// TODO: Need to rethink this when the class or struct definitions get hoisted eventually
	// TODO: Verify that the access level is not private or protected in this context
	// Find the initialisation of the struct or class
	const auto& struct_decl =
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
	if (struct_decl != m_program->GetChildren().end())
	{
		const auto& struct_declaration = static_cast<StructDeclaration&>(**struct_decl);
		const auto& member = std::find_if(struct_declaration.Members().begin(),
										  struct_declaration.Members().end(),
										  [&property](const auto& var)
										  {
											const auto& mem = static_cast<VariableDeclaration&>(*var);
											return mem.Name() == property.value.value();
										  });
		if (member != struct_declaration.Members().end())
			return std::make_unique<MemberExpression>(accessor, std::move(ident_ptr), std::move(mem_ptr));

		error("No member '{}' in '{}' at line: {}, in position: {}",
			  property.value.value(),
			  identifier.value.value(),
			  property.lineNumber,
			  property.columnNumber);
	}
	// If not found in struct, try classes
	const auto& class_decl =
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
	if (class_decl != m_program->GetChildren().end())
	{
		const auto& class_declaration = static_cast<ClassDeclaration&>(**class_decl);
		const auto& member = std::find_if(class_declaration.Members().begin(),
										  class_declaration.Members().end(),
										  [&property](const auto& var)
										  {
											const auto& mem = static_cast<VariableDeclaration&>(*var);
											return mem.Name() == property.value.value();
										  });
		if (member != class_declaration.Members().end())
			return std::make_unique<MemberExpression>(accessor, std::move(ident_ptr), std::move(mem_ptr));

		error("No member '{}' in '{}' at line: {}, in position: {}",
			  property.value.value(),
			  identifier.value.value(),
			  property.lineNumber,
			  property.columnNumber);
	}
	ASSERT_NOT_REACHABLE();
}

}