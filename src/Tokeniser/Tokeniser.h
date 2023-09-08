/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-06.
//

#pragma once

#include <optional>
#include <map>
#include "../AST/Types.h"

namespace alx {



struct Token
{
	explicit Token(TokenType type) : Type(type) {}
	Token(TokenType type, std::optional<std::string> value) : Type(type), Value(std::move(value)) {}
	TokenType Type;
	std::optional<std::string> Value;
};

class Tokeniser
{	
public:
	explicit Tokeniser(std::string source);
	[[nodiscard]] std::vector<Token> Tokenise();
	
private:

	std::string m_source;
	size_t m_index{};
	std::vector<Token> m_tokens{};
	std::map<std::string, TokenType> m_keywords;

	static bool is_space(char);
	static bool is_alpha(char);
	static bool is_alpha_numeric(char);
	static bool is_number(const std::string&);
	static bool is_double(const std::string&);
	static bool is_float(const std::string&);
	static bool is_integer(const std::string&);
	static bool is_digit(char);

	[[nodiscard]] std::optional<char> peek(int ahead = 0) const;
	char consume();

};
}