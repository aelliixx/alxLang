/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-06.
//

#include <regex>
#include "Tokeniser.h"
#include "../libs/Println.h"

namespace alx {
Tokeniser::Tokeniser(std::string source) : m_source(std::move(source))
{
	// Types
	m_keywords["int"] = TokenType::T_INT;
	m_keywords["long"] = TokenType::T_LONG;
	m_keywords["short"] = TokenType::T_SHORT;
	m_keywords["float"] = TokenType::T_FLOAT;
	m_keywords["double"] = TokenType::T_DOUBLE;
	m_keywords["void"] = TokenType::T_VOID;
	m_keywords["string"] = TokenType::T_STRING;
	m_keywords["char"] = TokenType::T_CHAR;
	m_keywords["bool"] = TokenType::T_BOOL;
	// Structures
	m_keywords["class"] = TokenType::T_CLASS;
	m_keywords["struct"] = TokenType::T_STRUCT;
	m_keywords["enum"] = TokenType::T_ENUM;
	// Other
	m_keywords["return"] = TokenType::T_RET;
	m_keywords["extern"] = TokenType::T_EXTERN;
	// Values
	m_keywords["true"] = TokenType::T_TRUE;
	m_keywords["false"] = TokenType::T_FALSE;
}
std::vector<Token> Tokeniser::Tokenise()
{
	while (peek().has_value())
	{
		std::string buffer{};
		// Is whitespace
		while (is_space(peek().value()) || peek().value() == '\n')
			consume();
		// Is a keyword or an identifier
		if (is_alpha(peek().value()))
		{
			buffer += consume();
			while (peek().has_value() && is_alpha_numeric(peek().value()))
				buffer += consume();

			if (m_keywords.contains(buffer))
			{
				m_tokens.emplace_back(m_keywords.find(buffer)->second);
				continue;
			}
			m_tokens.emplace_back(TokenType::T_IDENTIFIER, buffer);
			continue;
		}
		// Is a number
		if (is_digit(peek().value()) || peek().value() == '.')
		{
			buffer += consume();
			while (peek().has_value() && (is_digit(peek().value()) || peek().value() == '.'
				|| ((peek().value() == '+' || peek().value() == '-') && is_digit(peek(1).value()))
				))
				buffer += consume();

			if (!is_number(buffer))
			{
				println(Colour::Red, "Unexpected token in '{}'", buffer);
				exit(EXIT_FAILURE);
			}
			if (is_integer(buffer))
			{
				m_tokens.emplace_back(TokenType::T_INT_L, buffer);
				continue;
			}
			else if (is_float(buffer))
			{
				m_tokens.emplace_back(TokenType::T_FLOAT_L, buffer);
				continue;
			}
			else if (is_double(buffer))
			{
				m_tokens.emplace_back(TokenType::T_DOUBLE_L, buffer);
				continue;
			}
		}
		if (peek().value() == '(')
		{
			consume();
			m_tokens.emplace_back(TokenType::T_OPEN_PAREN);
			continue;
		}
		if (peek().value() == ')')
		{
			consume();
			m_tokens.emplace_back(TokenType::T_CLOSE_PAREN);
			continue;
		}
		if (peek().value() == '{')
		{
			consume();
			m_tokens.emplace_back(TokenType::T_CURLY_OPEN);
			continue;
		}
		if (peek().value() == '}')
		{
			consume();
			m_tokens.emplace_back(TokenType::T_CURLY_CLOSE);
			continue;
		}
		if (peek().value() == '=')
		{
			consume();
			m_tokens.emplace_back(TokenType::T_EQ);
			continue;
		}
		if (peek().value() == ';')
		{
			consume();
			m_tokens.emplace_back(TokenType::T_SEMI);
			continue;
		}
		if (peek().value() == '+')
		{
			consume();
			m_tokens.emplace_back(TokenType::T_PLUS);
			continue;
		}
		if (peek().value() == '-')
		{
			consume();
			m_tokens.emplace_back(TokenType::T_MINUS);
			continue;
		}
		if (peek().value() == '*')
		{
			consume();
			m_tokens.emplace_back(TokenType::T_STAR);
			continue;
		}
		if (peek().value() == '^')
		{
			consume();
			m_tokens.emplace_back(TokenType::T_POW);
			continue;
		}
		if (peek().value() == '/')
		{
			if (peek(1).value() == '/') // Comment
			{
				while (consume() != '\n');
				continue;
			}
			consume();
			m_tokens.emplace_back(TokenType::T_FWD_SLASH);
			continue;
		}
		error("Undefined token {} at ", buffer);
	}
	return m_tokens;
}

bool Tokeniser::is_space(char character)
{
	return character == ' ';
}

bool Tokeniser::is_alpha(char character)
{
	std::regex reg("[a-zA-Z][a-zA-Z0-9]*");
	std::string c;
	c = (char)character;
	return std::regex_match(c, reg);
}

bool Tokeniser::is_alpha_numeric(char character)
{
	std::regex reg("[a-zA-Z0-9]");
	std::string c;
	c = (char)character;
	return std::regex_match(c, reg);
}

bool Tokeniser::is_double(const std::string& number)
{
	std::regex reg(R"(^[-]?(\d*|\d{1,3}(,\d{3})*)(\.\d+)?\b$)");
	return std::regex_match(number, reg);
}

bool Tokeniser::is_digit(char character)
{
	std::regex reg(R"(\d)");
	std::string c;
	c = (char)character;
	return std::regex_match(c, reg);
}
bool Tokeniser::is_integer(const std::string& number)
{
	std::regex reg(R"(^[-]?(\d*|\d{1,3}(,\d{3})*)\b$)");
	return std::regex_match(number, reg);
}
bool Tokeniser::is_float(const std::string& number)
{
	std::regex reg(R"(^[-]?(\d*|\d{1,3}(,\d{3})*)(\.\d+)?f\b$)");
	return std::regex_match(number, reg);
}
bool Tokeniser::is_number(const std::string& number)
{
	return is_integer(number) || is_double(number) || is_float(number);
}
std::optional<char> Tokeniser::peek(int ahead) const
{
	if (m_index + ahead < m_source.length())
		return m_source.at(m_index + ahead);
	return {};
}
char Tokeniser::consume()
{
	auto c = m_source.at(m_index);
	++m_index;
	return c;
}
}
