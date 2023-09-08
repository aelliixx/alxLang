/*
 * Copyright (c) 2023 Donatas Mockus.
 */
#include <Parser/Parser.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <libs/Println.h>
#include <Tokeniser/Tokeniser.h>
#include "Codegen/Generator.h"

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
	const auto start = sys_clock::now();

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
	auto nasm = generator.Generate();

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