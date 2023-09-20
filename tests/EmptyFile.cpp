/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-19.
//

#include "../src/Compiler.h"

int main(int, char* argv[])
{
	const alx::DebugFlags df{ .quiet_mode = true };
	auto code = "";
	alx::Compiler compiler{ code, "Addition", {}, df};
	compiler.Compile();
	return 0;
}
