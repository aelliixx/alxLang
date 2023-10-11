/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-10.
//
#pragma once

namespace alx::ir {
enum class LinkageType
{
	Internal,
	External, // Exported
};

enum class VisibilityAttribute {
	Global, // Functions, structs, classes
	Local // Everything
};

enum class ParamAttributes
{
	ZeroExt,
	SignExt,
	ByVal,
};

struct AlignAttribute {
	size_t Alignment = 8;
};

using ParameterAttributes = std::variant<ParamAttributes, AlignAttribute>;

}