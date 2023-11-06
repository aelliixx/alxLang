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

struct ReturnInst {
	Values Value;
};

struct BranchInst {
	IntType Size{ 1 };
	std::optional<Values> Condition{};
	LabelType TrueLabel;
	std::optional<LabelType> FalseLabel{};
};

// Binary operations

struct AddInst {
	Values Lhs;
	Values Rhs;
};

struct SubInst {
	Values Lhs;
	Values Rhs;
};

struct MulInst {
	Values Lhs;
	Values Rhs;
};

struct SDivInst {
	Values Lhs;
	Values Rhs;
};

#if 0 // For future use
struct FAddInst {
	Values Lhs;
	Values Rhs;
};

struct FSubInst {
	Values Lhs;
	Values Rhs;
};

struct FMulInst {
	Values Lhs;
	Values Rhs;
};

struct UDivInst {
	Values Lhs;
	Values Rhs;
};



struct PowInst {
	Values Lhs;
	Values Rhs;
};

struct SRemInst {
	Values Lhs;
	Values Rhs;
};

struct URemInst {
	Values Lhs;
	Values Rhs;
};
#endif

// Memory access and addressing instructions

// We use alloca instruction in the code generator to increment the stack pointer
struct AllocaInst {
	std::shared_ptr<Types> Type;
	// Sizeof type in bytes
	[[nodiscard]] size_t Size() const
	{
		struct SizeVisitor {
			size_t operator()(const SingleValueType& value) const
			{
				switch (value) {
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

			size_t operator()(const IntType& type) const { return type.Size; }

			size_t operator()(const PtrType&) const { return sizeof(void*); }

			size_t operator()(const StructType& type) const
			{
				size_t size = 0;
				for (const auto& t : type.TypeList) {
					size += std::visit(SizeVisitor{}, t);
				}
				return size;
			}

			size_t operator()(const ArrayType& type) const { return type.Size * std::visit(SizeVisitor{}, *type.Type); }
			size_t operator()(const LabelType&) const { ASSERT_NOT_REACHABLE(); };
		};
		return std::visit(SizeVisitor{}, *Type);
	}
};

struct Variable;
struct StoreInst {
	Values Value;
	std::shared_ptr<Variable> Ptr;
	AlignAttribute Alignment;
};

struct LoadInst {
	Types Type;
	std::shared_ptr<Variable> Ptr;
	AlignAttribute Alignment;
};

// Other instructions

enum class CmpPredicate
{
	EQ,
	NE,
	SLT,
	SLE,
	SGT,
	SGE,
	ULT,
	ULE,
	UGT,
	UGE
};

struct ICmpInst {
	Values Lhs;
	Values Rhs;
	CmpPredicate Predicate;
};

using IdentifierInstruction = std::variant<AllocaInst, LoadInst, AddInst, SubInst, MulInst, SDivInst, ICmpInst>;

inline std::string cmpPredicateToString(CmpPredicate predicate)
{
	switch (predicate) {
	case CmpPredicate::EQ:
		return "eq";
	case CmpPredicate::NE:
		return "ne";
	case CmpPredicate::SLT:
		return "slt";
	case CmpPredicate::SLE:
		return "sle";
	case CmpPredicate::SGT:
		return "sgt";
	case CmpPredicate::SGE:
		return "sge";
	case CmpPredicate::ULT:
		return "ult";
	case CmpPredicate::ULE:
		return "ule";
	case CmpPredicate::UGT:
		return "ugt";
	case CmpPredicate::UGE:
		return "uge";
	}
	ASSERT_NOT_REACHABLE();
}

}; // namespace alx::ir
