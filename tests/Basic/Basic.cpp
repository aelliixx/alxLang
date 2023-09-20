/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-19.
//

#include "../../src/Compiler.h"

int main(int, char* argv[])
{
	const alx::DebugFlags df{ .quiet_mode = true };
	std::string arg = argv[1];
	if (arg == "0")
	{
		auto code = "";
		alx::Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		return 0;
	}
	if (arg == "main")
	{
		auto code = "int main() {}";
		alx::Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		std::string expected =
			"global _start\n"
			"section .bss\n"
			"section .data\n"
			"section .text\n"
			"\n"
			"_start:\n"
			"xor ebp, ebp\n"
			"call main\n"
			"mov rdi, rax\n"
			"mov rax, 60\n"
			"syscall\n"
			"\n"
			"main:\n"
			"push rbp\n"
			"mov rbp, rsp\n"
			"xor eax, eax\n"
			"pop rbp\n"
			"ret\n";
		return expected != compiler.GetAsm();
	}
}
