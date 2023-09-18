/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-14.
//

#include "BlockGenerator.h"

namespace alx {

void BlockGenerator::generate_while_statement(ASTNode* node)
{
	auto statement = static_cast<WhileStatement*>(node);
	m_asm << "jmp " << generate_local_label(statement->Condition()) << "\n";
	m_asm << generate_local_label(statement->BodyPtr()) << ":\n";
	generate_body(statement->Body());

	m_asm << generate_local_label(statement->Condition()) << ":\n";
	generate_branch(statement->Condition(), {}, generate_local_label(statement->BodyPtr()));

	
}

}