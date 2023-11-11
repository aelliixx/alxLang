/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-16.
//
#pragma once
#include <algorithm>
#include <list>
#include <utility>
#include "Println.h"

namespace alx {

class CompilerError final : public std::runtime_error
{
public:
	explicit CompilerError(const std::string& msg) : std::runtime_error(msg) {}
};

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

class ErrorHandler
{
private:
	std::vector<std::string> m_error_codes;
	size_t m_error_count{};
	size_t m_warning_count{};
	size_t m_note_count{};
	std::string m_code;
	std::string m_file_name;
	bool m_werror{};

public:
	ErrorHandler(std::string code, std::string fileName,
				 bool werror) : m_code(std::move(code)), m_file_name(std::move(fileName)), m_werror(werror) {}

	[[nodiscard]] size_t ErrorCount() const { return m_error_count; }

	void EmitErrorCount() const
	{
		println("\nBuild {}. Found: ", (m_error_count > 0 ? "failed" : "finished"));
		println("{} {255;}. {} {255;165;}. {} {0;100;200}",
				m_error_count,
				(m_error_count != 1 ? "errors" : "error"),
				m_warning_count,
				(m_warning_count != 1 ? "warnings" : "warning"),
				m_note_count,
				(m_note_count != 1 ? "notes" : "note"));
	}

	template<typename... Param>
	void Warning(size_t lineNum, size_t colNum, size_t posNum, const std::string& format, const Param&... arguments)
	{
		if (m_werror) {
			Error(lineNum, colNum, posNum, format, arguments...);
			return;
		}
		++m_warning_count;
		const auto text = getFormatted(format, arguments...);
		give_context(lineNum, colNum, posNum);
		auto colour = Colour::Orange;
		println(" | \033[38;2;{};{};{}m{}\033[0m\n | At line: {}, position: {}\n",
				colour.r,
				colour.g,
				colour.b,
				text,
				lineNum,
				colNum);
	}

	template<typename... Param>
	void Error(size_t lineNum, size_t colNum, size_t posNum, const std::string& format, const Param&... arguments)
	{
		++m_error_count;
		const auto text = getFormatted(format, arguments...);
		auto colour = Colour::LightRed;

		give_context(lineNum, colNum, posNum);

		println(" | \033[38;2;{};{};{}m{}\033[0m\n | At line: {}, position: {}\n",
				colour.r,
				colour.g,
				colour.b,
				text,
				lineNum,
				colNum);
	}

	template<typename... Param>
	void Note(size_t lineNum, size_t colNum, size_t posNum, const std::string& format, const Param&... arguments)
	{
		++m_note_count;
		const auto text = getFormatted(format, arguments...);
		Colour colour{ 0, 100, 200 };

		give_context(lineNum, colNum, posNum);

		println(" | \033[38;2;{};{};{}m{}\033[0m\n | At line: {}, position: {}\n",
				colour.r,
				colour.g,
				colour.b,
				text,
				lineNum,
				colNum);
	}

	template<typename... Param>
	void FatalError(size_t lineNum, size_t colNum, size_t posNum, const std::string& format, const Param&... arguments)
	{
		const auto text = getFormatted(format, arguments...);
		Error(lineNum, colNum, posNum, text);
		throw CompilerError(text);
	}

private:
	void give_context(size_t lineNum, size_t colNum, size_t posNum)
	{
		std::string lineBuffer;
		size_t lineStartIndex{ 0 };
		size_t errorIndexInLine{ 0 };
		for (auto i = posNum; m_code[i] != '\n'; --i) {
			if (!m_code.at(i))
				break;
			lineStartIndex = i;
		}
		for (int i = 0; m_code[lineStartIndex] != '\n'; ++lineStartIndex) {
			lineBuffer += m_code[lineStartIndex];
			++i;
			if (lineStartIndex == posNum)
				errorIndexInLine = i;
		}

		println("--> {}:{}:{}", m_file_name, lineNum, colNum);
		println(" | {}", lineBuffer);
		println(" | {}{}", std::string(errorIndexInLine ? errorIndexInLine - 1 : 0, ' '), '^');
	}
};

template<typename... Param>
void error(const std::string& format, const Param&... arguments)
{
	const auto text = getFormatted(format, arguments...);
	auto colour = Colour::LightRed;
	//	println("{}:", context);
	println("\033[38;2;{};{};{}m{}\033[0m", colour.r, colour.g, colour.b, text);
	throw std::runtime_error(text);
}

}