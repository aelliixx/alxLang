/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-19.
//
#pragma once

#include "Tokeniser/Tokeniser.h"
#include "Parser/Parser.h"
#include "Codegen/x86_64_linux/ProgramGenerator.h"
namespace alx {

class Compiler
{
	std::unique_ptr<Tokeniser> m_tokeniser;
	std::unique_ptr<Parser> m_parser;
	std::unique_ptr<ProgramGenerator> m_generator;
	std::shared_ptr<ErrorHandler> m_error_handler;
	const Flags m_flags;
	const std::string m_code;
	const DebugFlags m_debug_flags;
public:
	Compiler(const std::string& code,
			 const std::string& filename,
			 Flags flags,
			 DebugFlags debugFlags);

	void Compile();
	
	std::string GetAsm();
	const Program& GetAst();
	std::string GetFormattedAsm();
};

} // alx

