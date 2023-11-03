/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-06.
//

#pragma once

#include <optional>
#include <map>
#include <regex>
#include "../Utils/Types.h"
#include "../libs/ErrorHandler.h"

namespace alx {

struct Token
{
	Token(TokenType type, size_t lineNum, size_t colNum, size_t posNum)
		: Type(type),
		  LineNumber(lineNum),
		  ColumnNumber(colNum),
		  PosNumber(posNum - 1) {}
	Token(TokenType type, std::optional<std::string> value, size_t lineNum, size_t colNum, size_t posNum)
		: Type(type),
		  Value(std::move(value)),
		  LineNumber(lineNum),
		  ColumnNumber(colNum),
		  PosNumber(posNum - 1) {}
	TokenType Type;
	std::optional<std::string> Value;
	size_t LineNumber;
	size_t ColumnNumber;
	size_t PosNumber;
};

class Tokeniser
{
private:
	std::string m_temp_char;
	std::shared_ptr<ErrorHandler> m_error_handler;

public:
	Tokeniser(std::string source, const std::shared_ptr<ErrorHandler>& errorHandler);
	[[nodiscard]] std::vector<Token> Tokenise();

private:

	std::string m_source;
	size_t m_index{};
	size_t m_line_index{ 1 };
	size_t m_column_index{ 0 };
	std::vector<Token> m_tokens{};
	std::map<std::string, TokenType> m_keywords;

	static bool is_space(char);
	bool is_alpha(char);
	bool is_alpha_numeric(char);
	bool is_number(const std::string&);
	bool is_double(const std::string&);
	bool is_float(const std::string&);
	bool is_integer(const std::string&);
	bool is_digit(char);

	[[nodiscard]] std::optional<char> peek(int ahead = 0) const;
	char consume();

};
}