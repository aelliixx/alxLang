/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-10.
//

#pragma once
#include <utility>

#include "Attributes.h"

namespace alx::ir {
enum class SingleValueType
{
	Void,
	Half,
	Float,
	Double,
};

struct IntType
{
	// Size in bits
	size_t Size;
	explicit IntType(size_t size) : Size(size) {}
};

struct PtrType
{

};

struct LabelType
{
	std::string Name;
};

struct StructType;
struct ArrayType;
using Types = std::variant<SingleValueType, IntType, LabelType, PtrType, StructType, ArrayType>;

struct ArrayType
{
	size_t Size;
	std::shared_ptr<Types> Type;
	ArrayType(size_t size, std::unique_ptr<Types> type) : Size(size), Type(std::move(type)) {}
};

struct StructType
{
	std::string Name;
	VisibilityAttribute Visibility = VisibilityAttribute::Local;
	std::vector<Types> TypeList;
};
}
