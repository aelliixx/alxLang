/*
 * Copyright (c) 2023 Donatas Mockus.
 */
#include <iostream>
#include <fstream>
#include <libs/Println.h>
#include <Utils/Flags.h>
#include "libs/argparse.hpp"
#include "Compiler.h"

int main(int argc, const char** argv)
{
	argparse::ArgumentParser program("alxLang compiler", "0.0.3");
	program.add_description("A lexer, parser, and direct-to-asm compiler "
							"for alxLang - yet another general purpose programming language.");

	program.add_argument("-O0").implicit_value(true).default_value(true).help("Turn off all optimisations");

	program.add_argument("-d", "--dump-ast")
		.default_value(false)
		.implicit_value(true)
		.help("Display the abstract syntax tree generated by the parser.");

	program.add_argument("-t", "--show_timing")
		.default_value(false)
		.implicit_value(true)
		.help("Display the time it took to complete each compilation stage.");

	program.add_argument("-a", "--asm")
		.default_value(false)
		.implicit_value(true)
		.help("Output assembly to the console.");

	program.add_argument("--dump-ir")
		.default_value(false)
		.implicit_value(true)
		.help("Output intermediate representation to the console.");
	
	program.add_argument("-q", "--quiet")
		.default_value(false)
		.implicit_value(true)
		.help("Disables all output ignoring any other flags.");
	
	program.add_argument("-S", "--no-assemble")
		.default_value(false)
		.implicit_value(true)
		.help("Do not assemble the generated assembly.");

	program.add_argument("--asm-no-format")
		.default_value(false)
		.implicit_value(true)
		.help("Output assembly to the console without formatting.");

	program.add_argument("-mno-red-zone").default_value(false).implicit_value(true);

	program.add_argument("-fdiagnostics-colour")
		.default_value(true)
		.implicit_value(true)
		.help("Enable colours in diagnostic output.");
	
	program.add_argument("-Werror")
		.default_value(false)
		.implicit_value(true)
		.help("Treat all warnings as errors.");

	program.add_argument("filename");

	try {
		program.parse_args(argc, argv);
	}
	catch (std::runtime_error& err) {
		alx::println(alx::Colour::LightRed, "{}", err.what());
		return 1;
	}
	auto programName = program.get<std::string>("filename");
	if (!program.get<bool>("-q"))
		alx::println("Parsing {}", programName);
	std::string sourceBuffer;
	{
		auto ss = std::ostringstream{};
		std::ifstream input(programName);
		if (!input.is_open()) {
			if (!program.get<bool>("-q"))
				alx::error("File {} not found!", programName);
			return EXIT_FAILURE;
		}
		ss << input.rdbuf();
		sourceBuffer = ss.str();
		input.close();
	}
	if (sourceBuffer.length() <= 0)
		return 0;

	alx::Compiler compiler{ sourceBuffer, programName, alx::resolveFlags(program), alx::resolveDebugFlags(program) };
	compiler.Compile();
	// nasm -f elf64 comp.asm -o comp.o && ld comp.o -o comp
}