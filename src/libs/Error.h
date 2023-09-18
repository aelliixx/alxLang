/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-16.
//

#include <list>
#include "Println.h"

namespace alx {

enum class ErrorCode
{
	E000,
	E001,
	E002,
	E003,
	E004,
	E005,
	E006,
	E007,
	E008,
	E009,
	E010, // Unexpected token in lexer
	E011, // Unexpected token in parser
	E012, // Use of undeclared class or struct identifier 
	E013, // Use of undeclared identifier
	E014, // No member x in struct or object
	E015, // Cannot assign a value to void
	E016, // Invalid binary operator
	E017, // Division by 0
	W500  // Uninitialised variable
};

struct Error
{
	static std::vector<std::string> ErrorCodes;
	static size_t ErrorCount;
	static size_t WarningCount;
	static size_t NoteCount;
	static void EmitErrorCount() {
		println("\nBuild stopped. Found: ");
		println("{} {255;}. {} {255;255;}. {0;100;200} notes", ErrorCount, "errors", WarningCount, "warnings", NoteCount, "notes");
	}
};


template<typename... Param>
void error(const std::string& format, const Param& ... arguments)
{
	++Error::ErrorCount;
	const auto text = getFormatted(format, arguments...);
	auto colour = Colour::LightRed;
//	println("{}:", context);
	println("\033[38;2;{};{};{}m{}\033[0m", colour.r, colour.g, colour.b, text);
	throw std::runtime_error(text);
}

template<typename... Param>
void error_context(const std::string& context, const std::string& format, const Param& ... arguments)
{
	++Error::ErrorCount;
	const auto text = getFormatted(format, arguments...);
	auto colour = Colour::LightRed;
	println("{}:", context);
	println("\033[38;2;{};{};{}m{}\033[0m", colour.r, colour.g, colour.b, text);
	throw std::runtime_error(text);
}

template<typename... Param>
void warning(const std::string& format, const Param& ... arguments)
{
	++Error::WarningCount;
	const auto text = getFormatted(format, arguments...);
	auto colour = Colour::Orange;
//	println("{}:", context);
	println("\033[38;2;{};{};{}m{}\033[0m", colour.r, colour.g, colour.b, text);
}

}