/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-06.
//

#include <regex>
#include "Tokeniser.h"
#include "../libs/Println.h"
#include "../libs/Error.h"

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
	// Branching
	m_keywords["if"] = TokenType::T_IF;
	m_keywords["else"] = TokenType::T_ELSE;
	m_keywords["while"] = TokenType::T_WHILE;
	m_keywords["for"] = TokenType::T_FOR;
}

std::vector<Token> Tokeniser::Tokenise()
{
	try
	{
		while (peek().has_value())
		{
			std::string buffer{};
			// Is whitespace
			while (peek().has_value() && (is_space(peek().value()) || peek().value() == '\n'))
			{
				if (peek().value() == '\n')
				{
					++m_line_index;
					m_column_index = 0;
				}
				consume();
			}
			// Is a keyword or an identifier
			if (is_alpha(peek().value()))
			{
				buffer += consume();
				while (peek().has_value() && is_alpha_numeric(peek().value()))
					buffer += consume();

				if (m_keywords.contains(buffer))
				{
					std::string value = buffer;
					if (buffer == "true")
						value = "1";
					else if (buffer == "false")
						value = "0";
					m_tokens.emplace_back(m_keywords.find(buffer)->second, value, m_line_index, m_column_index);
					continue;
				}
				m_tokens.emplace_back(TokenType::T_IDENTIFIER, buffer, m_line_index, m_column_index);
				continue;
			}
				// Is a number
			else if (is_digit(peek().value()) || peek().value() == '.' ||
				(peek(1).has_value() && isdigit(peek(1).value()) && (peek().value() == '+' || peek().value() == '-')))
			{
				buffer += consume();
				while (peek().has_value()
					&& (is_digit(peek().value()) || peek().value() == '.' || peek().value() == ','))
					buffer += consume();

				if (!is_number(buffer))
				{
					if (buffer == ".")
					{
						m_tokens.emplace_back(TokenType::T_DOT, buffer, m_line_index, m_column_index);
						continue;
					}
					println(Colour::Red,
							"Unexpected token in '{}', at line: {}, position: {}",
							buffer,
							m_line_index,
							m_column_index);
				}
				if (is_integer(buffer))
				{
					m_tokens.emplace_back(TokenType::T_INT_L, buffer, m_line_index, m_column_index);
					continue;
				}
				else if (is_float(buffer))
				{
					m_tokens.emplace_back(TokenType::T_FLOAT_L, buffer, m_line_index, m_column_index);
					continue;
				}
				else if (is_double(buffer))
				{
					m_tokens.emplace_back(TokenType::T_DOUBLE_L, buffer, m_line_index, m_column_index);
					continue;
				}
			}
			else if (peek().value() == ',')
			{
				consume();
				m_tokens.emplace_back(TokenType::T_COMMA, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == '(')
			{
				consume();
				m_tokens.emplace_back(TokenType::T_OPEN_PAREN, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == ')')
			{
				consume();
				m_tokens.emplace_back(TokenType::T_CLOSE_PAREN, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == '{')
			{
				consume();
				m_tokens.emplace_back(TokenType::T_CURLY_OPEN, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == '}')
			{
				consume();
				m_tokens.emplace_back(TokenType::T_CURLY_CLOSE, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == '=')
			{
				consume();
				if (peek().value() == '=')
				{
					consume();
					m_tokens.emplace_back(TokenType::T_EQEQ, m_line_index, m_column_index);
					continue;
				}
				m_tokens.emplace_back(TokenType::T_EQ, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == '<')
			{
				consume();
				if (peek().value() == '=')
				{
					consume();
					m_tokens.emplace_back(TokenType::T_LTE, m_line_index, m_column_index);
					continue;
				}
				m_tokens.emplace_back(TokenType::T_LT, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == '>')
			{
				consume();
				if (peek().value() == '=')
				{
					consume();
					m_tokens.emplace_back(TokenType::T_GTE, m_line_index, m_column_index);
					continue;
				}
				m_tokens.emplace_back(TokenType::T_GT, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == '!')
			{
				consume();
				if (peek().value() == '=')
				{
					consume();
					m_tokens.emplace_back(TokenType::T_NOT_EQ, m_line_index, m_column_index);
					continue;
				}
				m_tokens.emplace_back(TokenType::T_NOT, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == ';')
			{
				consume();
				m_tokens.emplace_back(TokenType::T_SEMI, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == '+')
			{
				consume();
				if (peek().value() == '=')
				{
					consume();
					m_tokens.emplace_back(TokenType::T_ADD_EQ, m_line_index, m_column_index);
					continue;
				}
				m_tokens.emplace_back(TokenType::T_PLUS, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == '-')
			{
				consume();
				if (peek().value() == '=')
				{
					consume();
					m_tokens.emplace_back(TokenType::T_SUB_EQ, m_line_index, m_column_index);
					continue;
				}
				if (peek().value() == '>')
				{
					consume();
					m_tokens.emplace_back(TokenType::T_ARROW, m_line_index, m_column_index);
					continue;
				}
				m_tokens.emplace_back(TokenType::T_MINUS, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == '*')
			{
				consume();
				m_tokens.emplace_back(TokenType::T_STAR, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == '^')
			{
				consume();
				m_tokens.emplace_back(TokenType::T_POW, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == ':')
			{
				consume();
				m_tokens.emplace_back(TokenType::T_COLON, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == '.')
			{
				consume();
				m_tokens.emplace_back(TokenType::T_DOT, m_line_index, m_column_index);
				continue;
			}
			else if (peek().value() == '/')
			{
				if (peek(1).value() == '/') // Comment
				{
					while (consume() != '\n');
					continue;
				}
				consume();
				m_tokens.emplace_back(TokenType::T_FWD_SLASH, m_line_index, m_column_index);
				continue;
			}
			error("Undefined token {} at ", buffer);
		}
	}
	catch (const std::bad_optional_access&) {} // If the tokens suddenly end, it doesn't necessarily mean we have an invalid token stream.
	                                               // If we do, we'll catch that in the parsing stage anyway.
	return m_tokens;
}

bool Tokeniser::is_space(char character)
{
	return character == ' ';
}

bool Tokeniser::is_alpha(char character)
{
	m_temp_char = (char)character;
	return std::regex_match(m_temp_char, m_alpha);
}

bool Tokeniser::is_alpha_numeric(char character)
{
	m_temp_char = (char)character;
	return std::regex_match(m_temp_char, m_alpha_numeric);
}

bool Tokeniser::is_double(const std::string& number)
{
	return std::regex_match(number, m_double);
}

bool Tokeniser::is_digit(char character)
{
	m_temp_char = (char)character;
	return std::regex_match(m_temp_char, m_digit);
}
bool Tokeniser::is_integer(const std::string& number)
{
	return std::regex_match(number, m_integer);
}
bool Tokeniser::is_float(const std::string& number)
{
	return std::regex_match(number, m_float);
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
	++m_column_index;
	++m_index;
	return c;
}
}
