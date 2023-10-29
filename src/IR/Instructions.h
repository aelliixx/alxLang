/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-28.
//

#pragma once

#include <memory>
#include "Variables.h"
#include "Values.h"

namespace alx::ir {

// Terminator instructions



struct ReturnInst
{
	Values Value;
};

// Memory access and addressing instructions

struct AllocaInst
{
	std::shared_ptr<Types> Type;
	// Sizeof type in bytes
	[[nodiscard]] size_t Size() const
	{
		struct SizeVisitor
		{
			size_t operator()(const SingleValueType& value) const
			{
				switch (value)
				{
				case SingleValueType::Void:
					return 0;
				case SingleValueType::Half:
					return 2;
				case SingleValueType::Float:
					return 4;
				case SingleValueType::Double:
					return 8;
				default:
					ASSERT_NOT_REACHABLE();
				}
			}

			size_t operator()(const IntType& type) const
			{
				return type.Size / 8;
			}

			size_t operator()(const PtrType&) const
			{
				return sizeof(void*);
			}

			size_t operator()(const StructType& type) const
			{
				size_t size = 0;
				for (const auto& t : type.TypeList)
				{
					size += std::visit(SizeVisitor{}, t);
				}
				return size;
			}

			size_t operator()(const ArrayType& type) const
			{
				return type.Size * std::visit(SizeVisitor{}, *type.Type);
			}
			size_t operator()(const LabelType&) const
			{
				ASSERT_NOT_REACHABLE();
			};
		};
		return std::visit(SizeVisitor{}, *Type);
	}
};

struct Variable;
struct StoreInst
{
	Values Value;
	std::shared_ptr<Variable> Ptr;
	AlignAttribute Alignment;
};

struct LoadInst
{
	Types Type;
	std::shared_ptr<Variable> Ptr;
	AlignAttribute Alignment;
};

};
