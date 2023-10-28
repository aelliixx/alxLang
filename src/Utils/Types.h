/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#pragma once
#include <cassert>
#include "Utils.h"

namespace alx {

enum class TokenType
{
	// Literals
	T_INT_L, T_FLOAT_L, T_DOUBLE_L, T_CHAR_L, T_STR_L, T_TRUE, T_FALSE,
	// Functions and variables
	T_IDENTIFIER, T_RET, T_EXTERN, T_INT, T_LONG, T_SHORT, T_FLOAT, T_DOUBLE, T_VOID, T_STRING,
	T_CHAR, T_BOOL, T_CLASS, T_STRUCT, T_ENUM, T_ASM,
	// Syntax
	T_SEMI, T_OPEN_PAREN, T_CLOSE_PAREN, T_CURLY_OPEN, T_CURLY_CLOSE,
	T_SQUARE_OPEN, T_SQUARE_CLOSE, T_COMMA, T_DOT, T_ARROW, T_COLON_COLON,
	// Binary operators
	T_PLUS, T_MINUS, T_STAR, T_FWD_SLASH, T_POW, T_LT, T_GT, T_LTE, T_GTE, T_EQ,
	T_MOD, T_COLON, T_EQEQ, T_NOT_EQ,
	// Maths ops
	T_SUB, T_ADD, T_ADD_EQ, T_SUB_EQ, T_MULT_EQ, T_DIV_EQ, T_MOD_EQ, T_POW_EQ, T_NOT,
	// Branching
	T_IF, T_ELSE, T_FOR, T_WHILE

};

static size_t size_of(TokenType token)
{
	switch (token)
	{

	case TokenType::T_BOOL:
	case TokenType::T_CHAR:
	case TokenType::T_CHAR_L:
	case TokenType::T_TRUE:
	case TokenType::T_FALSE:
		return 1;
	case TokenType::T_SHORT:
		return 2;
	case TokenType::T_INT:
	case TokenType::T_FLOAT:
	case TokenType::T_INT_L:
	case TokenType::T_FLOAT_L:
		return 4;
	case TokenType::T_LONG:
	case TokenType::T_DOUBLE:
	case TokenType::T_DOUBLE_L:
		return 8;
	case TokenType::T_VOID:
	case TokenType::T_STRING:
	case TokenType::T_STR_L:
	ASSERT_NOT_IMPLEMENTED();
	default:
	ASSERT_NOT_REACHABLE();
	}
}

static TokenType literal_to_type(TokenType literal)
{
	switch (literal)
	{
	case TokenType::T_INT_L:
		return TokenType::T_INT;
	case TokenType::T_FLOAT_L:
		return TokenType::T_FLOAT;
	case TokenType::T_DOUBLE_L:
		return TokenType::T_DOUBLE;
	case TokenType::T_CHAR_L:
		return TokenType::T_CHAR;
	case TokenType::T_STR_L:
		return TokenType::T_STRING;
	case TokenType::T_TRUE:
	case TokenType::T_FALSE:
		return TokenType::T_BOOL;
	default:
		assert(false && "Not reachable");
	}
}

static bool isUnsigned(TokenType type)
{
	return type == TokenType::T_BOOL || type == TokenType::T_TRUE || type == TokenType::T_FALSE;
}

static bool isBinaryOp(TokenType type)
{
	return type == TokenType::T_PLUS || type == TokenType::T_MINUS || type == TokenType::T_STAR ||
		type == TokenType::T_FWD_SLASH || type == TokenType::T_POW || type == TokenType::T_LT ||
		type == TokenType::T_GT || type == TokenType::T_LTE || type == TokenType::T_GTE ||
		type == TokenType::T_EQ || type == TokenType::T_MOD || type == TokenType::T_COLON ||
		type == TokenType::T_EQEQ || type == TokenType::T_NOT_EQ || type == TokenType::T_SUB_EQ
		|| type == TokenType::T_ADD_EQ;
}

static bool isUnaryOp(TokenType type)
{
	return type == TokenType::T_NOT || type == TokenType::T_MINUS || type == TokenType::T_PLUS ||
		type == TokenType::T_SUB || type == TokenType::T_ADD;
}

static bool isLiteralAssignable(TokenType lhs, TokenType rhs)
{
	switch (lhs)
	{
	case TokenType::T_SHORT:
	case TokenType::T_LONG:
	case TokenType::T_INT:
		return rhs == TokenType::T_INT_L;
	case TokenType::T_FLOAT:
		return rhs == TokenType::T_FLOAT_L;
	case TokenType::T_DOUBLE:
		return rhs == TokenType::T_DOUBLE_L;
	case TokenType::T_CHAR:
		return rhs == TokenType::T_CHAR_L;
	case TokenType::T_STRING:
		return rhs == TokenType::T_STR_L;
	case TokenType::T_BOOL:
		return rhs == TokenType::T_TRUE || rhs == TokenType::T_FALSE;
	default:
		assert(false && "Not reachable");
	}
}

static bool isNumberType(TokenType type)
{
	return type == TokenType::T_INT || type == TokenType::T_FLOAT || type == TokenType::T_DOUBLE ||
		type == TokenType::T_CHAR || type == TokenType::T_BOOL || type == TokenType::T_LONG ||
		type == TokenType::T_SHORT;
}

static bool isIntegerLiteral(TokenType type)
{
	return type == TokenType::T_INT_L ||
		type == TokenType::T_CHAR_L || type == TokenType::T_TRUE || type == TokenType::T_FALSE;
}

static bool isNumberLiteral(TokenType type)
{
	return type == TokenType::T_INT_L || type == TokenType::T_FLOAT_L || type == TokenType::T_DOUBLE_L ||
		type == TokenType::T_CHAR_L || type == TokenType::T_TRUE || type == TokenType::T_FALSE;
}
}
