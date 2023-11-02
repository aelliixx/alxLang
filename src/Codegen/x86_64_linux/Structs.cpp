/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-16.
//

#include "BlockGenerator.h"

namespace alx {

void BlockGenerator::generate_structs(const ASTNode&)
{

}
void BlockGenerator::generate_struct_variable(const ASTNode& node)
{
	const auto& variable = static_cast<const VariableDeclaration&>(node);
	auto structIt =
		std::find_if(m_program_ast.begin(), m_program_ast.end(), [&variable](const std::unique_ptr<ASTNode>& ast_node)
		{
		  if (ast_node->class_name() == "StructDeclaration")
		  {
			  return variable.TypeName() == static_cast<StructDeclaration*>(ast_node.get())->Name();
		  }
		  return false;
		});
	MUST(structIt != m_program_ast.end() && "Could not find struct declaration");

	auto structDecl = static_cast<StructDeclaration*>(structIt->get());

	// Initialise all default initialised members
	for (const auto& memberPtr : structDecl->Members())
	{
		const auto& member = static_cast<const VariableDeclaration&>(*memberPtr);
		if (member.Value() != nullptr)
		{
			add_to_stack(variable.Name() + "::" + member.Name(), size_of(member.TypeAsPrimitive()), TokenType::T_STRUCT);
			if (member.Value()->class_name() == "NumberLiteral")
			{
				MUST(member.TypeIndex() == 0 && "Non-primitive types are not yet supported");
				m_asm << mov(offset(m_bp_offset, size_of(member.TypeAsPrimitive())), 4,
							 static_cast<NumberLiteral&>(*member.Value()).Value());
			}
		}
	}

}

}