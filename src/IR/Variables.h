/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-29.
//

#pragma once

#include "Instructions.h"

namespace alx::ir {

struct Variable
{
	std::string Name;
	VisibilityAttribute Visibility = VisibilityAttribute::Local;
	std::vector<ParameterAttributes> Attributes;
	std::variant<AllocaInst, LoadInst> Allocation;
	bool IsTemporary = false;
	void PrintNode() const;
	size_t Size() const
	{
		if (std::holds_alternative<AllocaInst>(Allocation))
			return std::get<AllocaInst>(Allocation).Size();
		else if (std::holds_alternative<LoadInst>(Allocation))
			return std::get<LoadInst>(Allocation).Alignment.Alignment; // TODO: Does this need to be multiplied by 8?
		else
			return 0;
	}
};
}