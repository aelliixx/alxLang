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
	auto type = variable.TypeAsIdentifier();
	auto struct_it =
		std::find_if(m_program_ast.begin(), m_program_ast.end(), [&variable](const std::unique_ptr<ASTNode>& ast_node)
		{
		  if (ast_node->class_name() == "StructDeclaration")
		  {
			  return variable.TypeName() == static_cast<StructDeclaration*>(ast_node.get())->Name();
		  }
		  return false;
		});
	assert(struct_it != m_program_ast.end() && "Could not find struct declaration");

	auto struct_decl = static_cast<StructDeclaration*>(struct_it->get());

	// Initialise all default initialised members

	for (const auto& member_ptr : struct_decl->Members())
	{
		const auto& member = static_cast<const VariableDeclaration&>(*member_ptr);
		if (member.Value() != nullptr)
		{
			add_to_stack(const_cast<VariableDeclaration*>(&member));
			if (member.Value()->class_name() == "NumberLiteral")
			{
				assert(member.TypeIndex() == 0 && "Non-primitive types are not yet supported");
				m_asm << mov(offset(bp_offset, size_of(member.TypeAsPrimitive())), 4,
							 static_cast<NumberLiteral&>(*member.Value()).Value());
			}
		}
	}

}

}