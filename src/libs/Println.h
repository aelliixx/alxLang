#pragma once
#include <assert.h>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <type_traits>
#include <utility>
#include <vector>

#include "../Utils/Types.h"
#include "Colours.h"

#define MAX_ARGS 256

namespace alx {

namespace __alx {

template<typename T>
concept Stringable = std::convertible_to<T, std::string>;

template<typename T>
concept Char = std::is_same_v<T, char>;

template<typename T>
concept Number = std::is_arithmetic_v<T> &&
	!std::is_same_v<T, bool> &&
	!std::is_same_v<T, char>;

template<typename T>
concept Boolean = std::is_same_v<T, bool>;

template<__alx::Stringable T>
std::string string_cast(T from) { return from; }

template<Boolean T>
std::string string_cast(T from) { return from ? "true" : "false"; }

template<__alx::Number T>
std::string string_cast(T from) { return std::to_string(from); }

template<__alx::Char T>
std::string string_cast(T from) { return std::string(1, from); }

template<typename... Param>
class VariadicArgParser
{

	std::string _text;
	std::string _format;
	std::vector<std::string> _args;

	template<typename T, typename... Rest>
	void set_args(T& first, Rest& ... rest)
	{
		_args.push_back(string_cast(first));
		set_args(rest...);
	}

	void set_args() {}

	std::string parse_colour_sequence(const std::string& str)
	{
		std::stringstream stream(str);
		std::string item;
		std::string out = "\033[38;2";
		int matches = 0;
		while (std::getline(stream, item, ';'))
			if (item.length() > 0)
			{
				++matches;
				out += ";" + item;
			}
		for (int i = matches; i < 3; ++i)
			out += ";0";
		return out;
	}

	std::string parse_vector(const std::string& str)
	{
		static std::string componentColours[] = { ";200;0;0m", ";0;200;0m", ";0;0;200m", ";200;200;0m" };
		std::stringstream base(str);
		std::stringstream stream(str);
		std::string item;
		std::string out = "{";
		int matches = 0;
		int maxMatches = 0;
		while (std::getline(base, item, ','))
			++maxMatches;

		while (std::getline(stream, item, ','))
		{
			out +=
				"\033[38;2" + componentColours[matches % 4] + item + "\033[0m" + (matches != maxMatches - 1 ? "," : "");
			++matches;
		}

		out += "}";
		return out;
	}

	void parse_params()
	{
		std::string parsed = _format;
		auto length = parsed.length();
		size_t bodyArgs = 0;
		const size_t argCount = _args.size();

		bool paramSequence = false;
		uint seqStartIndex;

		for (uint i = 0; i < length; ++i)
		{
			if (parsed[i] == '{')
			{
				paramSequence = true;
				seqStartIndex = i;
				continue;
			}
			if (paramSequence && parsed[i] == 'v')
			{
				paramSequence = false;
				const auto vector = _args.at(bodyArgs);
				auto span = parse_vector(vector.substr(1, vector.length() - 2));
				parsed.replace(seqStartIndex, 3, span);
				++bodyArgs;
				continue;
			}
			if (paramSequence && parsed[i] == '>')
			{
				paramSequence = false;
				const auto indentLength = std::stoi(_args.at(bodyArgs));
				std::string padding(indentLength, ' ');
				parsed.replace(seqStartIndex, 3, padding);
				++bodyArgs;
				continue;
			}

			if (parsed[i] == '}' && paramSequence)
			{
				paramSequence = false;
				const auto n = i - seqStartIndex + 1;
				if (n > 2)
				{
					const auto colourSequence = parsed.substr(seqStartIndex + 1, i - seqStartIndex - 1);
					auto span = parse_colour_sequence(colourSequence);

					span += "m" + _args.at(bodyArgs) + "\033[0m";
					parsed.replace(seqStartIndex, n, span);
				}
				else
				{
					// FIXME: Fix bodyArgs being incremented by an argument string which itself contains '{}'
					//        e.g. println("{}", "foo {}"), crashes the program
					parsed.replace(seqStartIndex, i - seqStartIndex + 1, _args.at(bodyArgs));
					if (_args.at(bodyArgs).empty())
						--i;
					--i;
				}

				++bodyArgs;
				assert(bodyArgs <= argCount && "Error: body argument count cannot exceed argument count.");
				length = parsed.length();
			}
			length = parsed.length();
		}
		assert(argCount == bodyArgs && "Error: argument count cannot exceed body argument count.");
		_text = parsed;
	}

public:
	VariadicArgParser() = delete;

	explicit VariadicArgParser(std::string format, const Param& ... args)
		: _format(std::move(format))
	{
		set_args(args...);
		parse_params();
	}

	operator std::string() const { return _text; }
};

}

template<typename... Param>
std::string getFormatted(const std::string& format, const Param& ... arguments)
{
	const __alx::VariadicArgParser<Param...> parser{ format, arguments... };
	return parser;
}

template<typename... Param>
void println(const std::string& format, const Param& ... arguments)
{
	const auto text = getFormatted(format, arguments...);
	std::cout << text << '\n';
}

template<typename...>
void println()
{
	std::cout << "\n";
}

template<typename... Param>
void println(Colour colour, const std::string& format, const Param& ... arguments)
{
	const auto text = getFormatted(format, arguments...);
	println("\033[38;2;{};{};{}m{}\033[0m", colour.r, colour.g, colour.b, text);
}

template<typename... Param>
void print(const std::string& format, const Param& ... arguments)
{
	const auto text = getFormatted(format, arguments...);
	std::cout << text;
}

template<typename... Param>
void print(Colour colour, const std::string& format, const Param& ... arguments)
{
	const auto text = getFormatted(format, arguments...);
	print("\033[38;2;{};{};{}m{}\033[0m", colour.r, colour.g, colour.b, text);
}

static std::string token_to_string(TokenType token)
{
	switch (token)
	{
	case TokenType::T_INT_L:
		return "int";
	case TokenType::T_FLOAT_L:
		return "float";
	case TokenType::T_DOUBLE_L:
		return "double";
	case TokenType::T_CHAR_L:
		return "char";
	case TokenType::T_STR_L:
		return "string";
	case TokenType::T_IDENTIFIER:
		return "identifier";
	case TokenType::T_RET:
		return "return";
	case TokenType::T_EXTERN:
		return "extern";
	case TokenType::T_INT:
		return "int";
	case TokenType::T_FLOAT:
		return "float";
	case TokenType::T_DOUBLE:
		return "double";
	case TokenType::T_VOID:
		return "void";
	case TokenType::T_STRING:
		return "string";
	case TokenType::T_CHAR:
		return "char";
	case TokenType::T_BOOL:
		return "bool";
	case TokenType::T_CLASS:
		return "class";
	case TokenType::T_STRUCT:
		return "struct";
	case TokenType::T_ENUM:
		return "enum";
	case TokenType::T_SEMI:
		return ";";
	case TokenType::T_OPEN_PAREN:
		return "(";
	case TokenType::T_CLOSE_PAREN:
		return ")";
	case TokenType::T_CURLY_OPEN:
		return "{";
	case TokenType::T_CURLY_CLOSE:
		return "}";
	case TokenType::T_SQUARE_OPEN:
		return "[";
	case TokenType::T_SQUARE_CLOSE:
		return "]";
	case TokenType::T_COMMA:
		return ",";
	case TokenType::T_PLUS:
		return "+";
	case TokenType::T_MINUS:
		return "-";
	case TokenType::T_STAR:
		return "*";
	case TokenType::T_FWD_SLASH:
		return "/";
	case TokenType::T_POW:
		return "^";
	case TokenType::T_LT:
		return "<";
	case TokenType::T_GT:
		return ">";
	case TokenType::T_LTE:
		return "<=";
	case TokenType::T_GTE:
		return ">=";
	case TokenType::T_EQ:
		return "=";
	case TokenType::T_MOD:
		return "%";
	case TokenType::T_SUB:
		return "--";
	case TokenType::T_ADD:
		return "++";
	case TokenType::T_ADD_EQ:
		return "+=";
	case TokenType::T_SUB_EQ:
		return "-=";
	case TokenType::T_MULT_EQ:
		return "*=";
	case TokenType::T_DIV_EQ:
		return "/=";
	case TokenType::T_MOD_EQ:
		return "%=";
	case TokenType::T_POW_EQ:
		return "^=";
	case TokenType::T_EQEQ:
		return "==";
	case TokenType::T_TRUE:
		return "true";
	case TokenType::T_FALSE:
		return "false";
	case TokenType::T_LONG:
		return "long";
	case TokenType::T_SHORT:
		return "short";
	case TokenType::T_IF:
		return "if";
	case TokenType::T_ELSE:
		return "else";
	case TokenType::T_FOR:
		return "for";
	case TokenType::T_WHILE:
		return "while";
	case TokenType::T_NOT_EQ:
		return "!=";
	case TokenType::T_NOT:
		return "!";
	case TokenType::T_ASM:
		return "asm";
	case TokenType::T_DOT:
		return ".";
	case TokenType::T_ARROW:
		return "->";
	case TokenType::T_COLON_COLON:
		return "::";
	case TokenType::T_COLON:
		return ":";
	}
	ASSERT_NOT_REACHABLE();
}
}
