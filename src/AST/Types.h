/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-07.
//

#pragma once
#include <cassert>
namespace alx {

enum class TokenType
{
	// Literals
	T_INT_L, T_FLOAT_L, T_DOUBLE_L, T_CHAR_L, T_STR_L, T_TRUE, T_FALSE,
	// Functions and variables
	T_IDENTIFIER, T_RET, T_EXTERN, T_INT, T_LONG, T_SHORT, T_FLOAT, T_DOUBLE, T_VOID, T_STRING,
	T_CHAR, T_BOOL, T_CLASS, T_STRUCT, T_ENUM,
	// Syntax
	T_SEMI, T_OPEN_PAREN, T_CLOSE_PAREN, T_CURLY_OPEN, T_CURLY_CLOSE,
	T_SQUARE_OPEN, T_SQUARE_CLOSE, T_COMMA,
	// Binary operators
	T_PLUS, T_MINUS, T_STAR, T_FWD_SLASH, T_POW, T_LT, T_GT, T_LTE, T_GTE, T_EQ,
	T_MOD, T_INT_DIV, T_EQEQ,
	// Maths ops
	T_SUB, T_ADD, T_ADD_EQ, T_SUB_EQ, T_MULT_EQ, T_DIV_EQ, T_MOD_EQ, T_POW_EQ

};

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

static bool is_unsigned(TokenType type)
{
	return type == TokenType::T_BOOL;
}

static bool is_binary_op(TokenType type)
{
	return type == TokenType::T_PLUS || type == TokenType::T_MINUS || type == TokenType::T_STAR ||
		type == TokenType::T_FWD_SLASH || type == TokenType::T_POW || type == TokenType::T_LT ||
		type == TokenType::T_GT || type == TokenType::T_LTE || type == TokenType::T_GTE ||
		type == TokenType::T_EQ || type == TokenType::T_MOD || type == TokenType::T_INT_DIV ||
		type == TokenType::T_EQEQ;
}

static bool is_literal_assignable(TokenType lhs, TokenType rhs)
{
	switch (lhs)
	{
	case TokenType::T_SHORT:
		return rhs == TokenType::T_INT_L;
	case TokenType::T_LONG:
		return rhs == TokenType::T_INT_L;
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

static bool is_number_type(TokenType type)
{
	return type == TokenType::T_INT || type == TokenType::T_FLOAT || type == TokenType::T_DOUBLE ||
		type == TokenType::T_CHAR || type == TokenType::T_BOOL || type == TokenType::T_LONG ||
		type == TokenType::T_SHORT;
}
}
