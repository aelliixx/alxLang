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
#include "../AST/Types.h"

namespace alx {

struct Token
{
	explicit Token(TokenType type, size_t line_num, size_t col_num)
		: type(type),
		  lineNumber(line_num),
		  columnNumber(col_num - 1) {}
	Token(TokenType type, std::optional<std::string> value, size_t line_num, size_t col_num)
		: type(type),
		  value(std::move(value)),
		  lineNumber(line_num),
		  columnNumber(col_num - 1) {}
	TokenType type;
	std::optional<std::string> value;
	size_t lineNumber;
	size_t columnNumber;
};

class Tokeniser
{
private:
	const std::regex m_alpha{"[a-zA-Z][a-zA-Z0-9]*"};
	const std::regex m_alpha_numeric{"[a-zA-Z0-9-_]"};
	const std::regex m_double{R"(^[-]?(\d*|\d{1,3}(,\d{3})*)(\.\d+)?\b$)"};
	const std::regex m_integer{R"(^[-]?(\d*|\d{1,3}(,\d{3})*)\b$)"};
	const std::regex m_float{R"(^[-]?(\d*|\d{1,3}(,\d{3})*)(\.\d+)?f\b$)"};
	const std::regex m_digit{R"(\d)"};

	std::string m_temp_char;
	
	
	


public:
	explicit Tokeniser(std::string source);
	[[nodiscard]] std::vector<Token> Tokenise();

private:

	std::string m_source;
	size_t m_index{};
	size_t m_line_index{1};
	size_t m_column_index{0};
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