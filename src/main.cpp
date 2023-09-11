/*
 * Copyright (c) 2023 Donatas Mockus.
 */
#include <Parser/Parser.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <libs/Println.h>
#include <Tokeniser/Tokeniser.h>
#include "Codegen/x86_64_linux/Generator.h"

// TODO:
// 	- Implement a call expression
//  - Have a new m_stack for each stack frame
//    -> Have a new class for generating each block statement which contains the m_stack 
// 	- Match function return type and actual return type
// 	- Properly initialise the stack frame (look into it)
// 	- Implement an exponent operator
// 	- Profile the parsing stage

using sys_clock = std::chrono::system_clock;
using seconds = std::chrono::duration<double>;

int main(int argc, const char** argv)
{
	if (argc < 2)
	{
		alx::error("No input file supplied");
		return EXIT_FAILURE;
	}

	alx::println("Parsing {}", argv[1]);

	std::string sourceBuffer;
	{
		auto ss = std::ostringstream{};
		std::ifstream input(argv[1]);
		if (!input.is_open())
		{
			alx::error("File {} not found!", argv[1]);
			return EXIT_FAILURE;
		}
		ss << input.rdbuf();
		sourceBuffer = ss.str();
		input.close();
	}
	if (sourceBuffer.length() <= 0)
	{
		alx::println(alx::Colour::Red, "File {} is empty!", argv[1]);
		return EXIT_FAILURE;
	}

	const auto start = sys_clock::now();
	alx::Tokeniser tokeniser(sourceBuffer);
	auto tokens = tokeniser.Tokenise();

	{
		const seconds duration = sys_clock::now() - start;
		alx::println(alx::Colour::LightGreen, "Tokenised in {}ms", duration.count() * 1000);
	}
	const auto parseStart = sys_clock::now();

	alx::Parser parser(tokens);
	auto ast = parser.Parse();

	{
		const seconds duration = sys_clock::now() - parseStart;
		alx::println(alx::Colour::LightGreen, "Built AST in {}ms", duration.count() * 1000);
	}

	const auto generateStart = sys_clock::now();

	alx::Generator generator(ast->Children()); 
	
	try {
	auto nasm = generator.Generate();
	} catch (std::runtime_error&) {
		alx::println(alx::Colour::LightRed, "Something went wrong when generating assembly. AST:");
		ast->PrintNode(0);
		return 1;
	} 

	{
		const seconds duration = sys_clock::now() - generateStart;
		const seconds totalDuration = sys_clock::now() - start;
		alx::println(alx::Colour::LightGreen, "Compiled assembly in {}ms", duration.count() * 1000);
		alx::println(alx::Colour::LightGreen, "Total compilation time in {}ms", totalDuration.count() * 1000);
	}
	
	alx::println();
	ast->PrintNode(0);
	alx::println();
	alx::println(generator.Asm());
	
	// nasm -f elf64 comp.asm -o comp.o && ld comp.o -o comp
}