/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-28.
//

#pragma once

#include <variant>
#include "Types.h"
#include "Variables.h"
namespace alx::ir {


// Simple constants

using ConstantTypes = std::variant<SingleValueType, IntType, PtrType>;
struct Constant
{
	ConstantTypes Type;
	std::variant<long, float, double> Value;

	struct TypeVisitor
	{
		std::pair<size_t, std::string> operator()(SingleValueType type) const
		{
			switch (type)
			{
			case SingleValueType::Void:
				return { 0, "void" };
			case SingleValueType::Half:
				return { 2, "half" };
			case SingleValueType::Float:
				return { 4, "float" };
			case SingleValueType::Double:
				return { 8, "double" };
			}
			ASSERT_NOT_REACHABLE();
		}

		std::pair<size_t, std::string> operator()(const IntType& type) const
		{
			return { type.Size, "i" + std::to_string(type.Size * 8) };
		}

		std::pair<size_t, std::string> operator()(const PtrType&) const
		{
			return { 8, "ptr" };
		}
	};

	[[nodiscard]] size_t Size() const
	{
		return std::visit(TypeVisitor{}, Type).first;
	}

	[[nodiscard]] std::string TypeToString() const
	{
		return std::visit(TypeVisitor{}, Type).second;
	}
};
struct Variable;
using Values = std::variant<Constant, std::shared_ptr<Variable>>;
}