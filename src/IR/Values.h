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

	[[nodiscard]] std::string TypeToString() const
	{
		struct TypeVisitor
		{
			std::string operator()(SingleValueType type) const
			{
				switch (type)
				{
				case SingleValueType::Void:
					return "void";
				case SingleValueType::Half:
					return "half";
				case SingleValueType::Float:
					return "float";
				case SingleValueType::Double:
					return "double";
				default:
				ASSERT_NOT_REACHABLE();
				}
			}

			std::string operator()(const IntType& type) const
			{
				return "i" + std::to_string(type.Size);
			}

			std::string operator()(const PtrType&) const
			{
				return "ptr";
			}
		} visitor;
		return std::visit(visitor, Type);
	}
};
struct Variable;
using Values = std::variant<Constant, std::shared_ptr<Variable>>;
}