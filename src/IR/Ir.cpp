/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-08.
//

#include "Ir.h"
#include "../libs/Println.h"
#include "Instructions.h"

namespace alx::ir {

void IR::Generate()
{
	println("Generating IR:");
	for (const auto& node : m_ast)
	{
		if (node->class_name() == "FunctionDeclaration")
		{
			auto& func = static_cast<FunctionDeclaration&>(*node);
			generate_function(func);
		}
		else
		{
			println(Colour::Red, "Unknown node type: {;255;255;255}", node->class_name());
		}
	}
	println("Done generating IR");
}






}
