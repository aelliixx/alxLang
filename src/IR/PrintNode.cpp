/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-10.
//

#include "Ir.h"

namespace alx::ir {

#define BLUE                                                                                                           \
	Colour { 70, 160, 220 }
#define GREEN                                                                                                          \
	Colour { 60, 197, 172 }
#define OLIVE                                                                                                          \
	Colour { 171, 189, 138 }

#if OUTPUT_IR_TO_STRING
#define print(...) ir.m_ir_string += alx::getFormatted(__VA_ARGS__)
#define println(...) (ir.m_ir_string += alx::getFormatted(__VA_ARGS__) + "\n")
#endif

struct ConstantVisitor {
	std::string operator()(long constant) { return std::to_string(constant); }
	std::string operator()(float constant) { return std::to_string(constant); }
	std::string operator()(double constant) { return std::to_string(constant); }
};
struct ValueVisitor {
	bool OutputIdentifier = true;
	bool OutputType = true;
	std::string operator()(const Constant& constant) const
	{
		if (constant.Type.index() == 0 && std::get<SingleValueType>(constant.Type) == SingleValueType::Void)
			return alx::getFormatted("{;60;197;172}", "void");
		else if (OutputIdentifier)
			return alx::getFormatted("{;60;197;172}{;171;189;138}",
									 OutputType ? constant.TypeToString() + " " : "",
									 std::visit(ConstantVisitor{}, constant.Value));
		else
			return alx::getFormatted("{;60;197;172}", constant.TypeToString());
	}
	std::string operator()(const std::shared_ptr<Variable>& variable) const
	{
		struct AllocatorVisitor {
			std::string operator()(const AllocaInst& alloca) { return IR::TypesToString(*alloca.Type); }
			std::string operator()(const LoadInst& load) { return IR::TypesToString(load.Type); }
			std::string operator()(const AddInst& add) { return std::visit(ValueVisitor{ false }, add.Rhs); }
			std::string operator()(const SubInst& sub) { return std::visit(ValueVisitor{ false }, sub.Rhs); }
			std::string operator()(const MulInst& sub) { return std::visit(ValueVisitor{ false }, sub.Rhs); }
			std::string operator()(const SDivInst& sub) { return std::visit(ValueVisitor{ false }, sub.Rhs); }
			std::string operator()(const ICmpInst& cmp) { return std::visit(ValueVisitor{ false }, cmp.Rhs); }
		};
		if (OutputIdentifier)
			return alx::getFormatted("{;60;197;172}{;70;160;220}",
									 OutputType ? std::visit(AllocatorVisitor{}, variable->Allocation) + " " : "",
									 (variable->Visibility == VisibilityAttribute::Local ? "%" : "@") + variable->Name);
		else
			return alx::getFormatted("{;60;197;172}", std::visit(AllocatorVisitor{}, variable->Allocation));
	}
};

void IR::Dump()
{
#if !OUTPUT_IR_TO_STRING
	println("Dumping IR with --dump-ir:\n");
#endif
	struct IrVisitor {
		IR& ir;
		void operator()(const std::unique_ptr<Function>& func) { func->PrintNode(ir); }
	};
	for (const auto& node : m_ir) std::visit(IrVisitor{ *this }, node);
}

void FunctionParameter::PrintNode(IR&) const
{
	print(GREEN, "{} ", IR::TypesToString(Type));
	for (const auto& attribute : Attributes) print("{} ", IR::EnumToString(attribute));
	print(BLUE, "{}{}", (Visibility == VisibilityAttribute::Local ? "%" : "@"), Name);
}

void Function::PrintNode(IR& ir) const
{
	print(BLUE, "define ");
	print("{}", IR::EnumToString(Linkage));
	print(GREEN, " {}", IR::TypesToString(ReturnType));
	print(BLUE, " {}{}", Visibility == VisibilityAttribute::Global ? "@" : "%", Name);

	if (!Arguments.empty()) {
		print("(");
		auto separator = "";
		for (const auto& arg : Arguments) {
			print(separator);
			arg.PrintNode(ir);
			separator = ", ";
		}
		print(")");
	}

	println(" {");

	struct BodyVisitor {
		IR& ir;
		void operator()(const LabelType&) {}
		void operator()(const ReturnInst& ret)
		{
			std::string returnType = std::visit(ValueVisitor{}, ret.Value);
			println("  ret {}", returnType);
		}
		void operator()(const Variable& var) { var.PrintNode(ir); }
		void operator()(const StoreInst& store)
		{
			std::string value = std::visit(ValueVisitor{}, store.Value);
			print("  store");
			print(" {}", value);
			print(", ptr ");
			print(BLUE, "{}{}", store.Ptr->Visibility == VisibilityAttribute::Local ? "%" : "@", store.Ptr->Name);
			println(", align {}", store.Alignment.Alignment);
		}
		void operator()(const BranchInst& branch)
		{
			print("  br ");
			if (branch.Condition.has_value()) {
				print(GREEN, "{} ", IR::TypesToString(branch.Size));
				print(std::visit(ValueVisitor{ .OutputType = false }, branch.Condition.value()));
				print(", ");
			}
			print(GREEN, "label ");
			print(BLUE, "%{}", branch.TrueLabel.Name);
			if (branch.FalseLabel.has_value()) {
				print(", ");
				print(GREEN, "label ");
				print(BLUE, "%{}", branch.FalseLabel.value().Name);
			}
			println();
		}
	};

	for (const auto& block : Blocks) {
		println(BLUE, "{}:", block.Label.Name);
		for (const auto& child : block.Body) std::visit(BodyVisitor{ ir }, child);
		println();
	}
	println("}");
}

void Variable::PrintNode(IR& ir) const
{
	struct AllocatorVisitor {
		IR& ir;
		void operator()(const AllocaInst& alloca)
		{
			print(" = alloca ");
			print(GREEN, "{}", IR::TypesToString(*alloca.Type));
		}
		void operator()(const LoadInst& load)
		{
			print(" = load ");
			print(GREEN, "{}", IR::TypesToString(load.Type));
			print(", ");
			print("ptr ");
			print(BLUE, "{}{}", load.Ptr->Visibility == VisibilityAttribute::Local ? "%" : "@", load.Ptr->Name);
		}
		void operator()(const AddInst& add)
		{
			print(" = add ");
			print(std::visit(ValueVisitor{}, add.Lhs));
			print(", ");
			print(std::visit(ValueVisitor{ .OutputType = false }, add.Rhs));
		}
		void operator()(const SubInst& sub)
		{
			print(" = sub ");
			print(std::visit(ValueVisitor{}, sub.Lhs));
			print(", ");
			print(std::visit(ValueVisitor{ .OutputType = false }, sub.Rhs));
		}
		void operator()(const MulInst& sub)
		{
			print(" = mul ");
			print(std::visit(ValueVisitor{}, sub.Lhs));
			print(", ");
			print(std::visit(ValueVisitor{ .OutputType = false }, sub.Rhs));
		}
		void operator()(const SDivInst& sub)
		{
			print(" = sdiv ");
			print(std::visit(ValueVisitor{}, sub.Lhs));
			print(", ");
			print(std::visit(ValueVisitor{ .OutputType = false }, sub.Rhs));
		}
		void operator()(const ICmpInst& cmp)
		{
			print(" = icmp ");
			print(cmpPredicateToString(cmp.Predicate));
			print(" ");
			print(std::visit(ValueVisitor{}, cmp.Lhs));
			print(", ");
			print(std::visit(ValueVisitor{ .OutputType = false }, cmp.Rhs));
		}
	} visitor{ ir };
	print("  ");
	print(BLUE, "{}{}", Visibility == VisibilityAttribute::Local ? "%" : "@", Name);
	std::visit(visitor, Allocation);
	if (!Attributes.empty()) {
		print(",");
		for (const auto& attribute : Attributes) print(" {}", IR::EnumToString(attribute));
	}
	println("");
}

std::string IR::TypesToString(const Types& types)
{
	struct TypeVisitor {
		std::string operator()(SingleValueType type) const
		{
			switch (type) {
			case SingleValueType::Void:
				return "void";
			case SingleValueType::Half:
				return "half";
			case SingleValueType::Float:
				return "float";
			case SingleValueType::Double:
				return "double";
			}
			ASSERT_NOT_REACHABLE();
		}
		std::string operator()(IntType intType) const { return getFormatted("i{}", intType.Size * 8); }
		std::string operator()(const LabelType&) const { return "label"; }
		std::string operator()(PtrType) const { return "ptr"; }
		std::string operator()(const StructType&) const { return "struct"; }
		std::string operator()(const ArrayType&) const { return "array"; }
	};
	return std::visit(TypeVisitor{}, types);
}

std::string IR::EnumToString(std::variant<LinkageType, ParameterAttributes> variant)
{
	struct Visitor {
		int value;
		std::string operator()(LinkageType linkage)
		{
			switch (linkage) {
			case LinkageType::External:
				return "external";
			case LinkageType::Internal:
				return "internal";
			}
			ASSERT_NOT_REACHABLE();
		}
		std::string operator()(const ParameterAttributes& paramAttributes)
		{
			struct ParameterVisitor {
				std::string operator()(ParamAttributes attributes)
				{
					switch (attributes) {
					case ParamAttributes::ZeroExt:
						return "zeroext";
					case ParamAttributes::SignExt:
						return "signext";
					case ParamAttributes::ByVal:
						return "byval";
					}
					ASSERT_NOT_REACHABLE();
				}
				std::string operator()(AlignAttribute alignment)
				{
					return getFormatted("align {}", alignment.Alignment);
				}
			};
			return std::visit(ParameterVisitor{}, paramAttributes);
		}
	};
	return std::visit(Visitor{}, variant);
}
Types IR::TokenTypeToIRType(TokenType tokenType)
{
	switch (tokenType) {
	case TokenType::T_CHAR_L:
	case TokenType::T_CHAR:
	case TokenType::T_BOOL:
	case TokenType::T_TRUE:
	case TokenType::T_FALSE:
		return IntType{ 1 };
	case TokenType::T_INT_L:
	case TokenType::T_INT:
		return IntType{ 4 };
	case TokenType::T_LONG:
		return IntType{ 8 };
	case TokenType::T_SHORT:
		return IntType{ 2 };
	case TokenType::T_FLOAT_L:
	case TokenType::T_FLOAT:
		return SingleValueType::Float;
	case TokenType::T_DOUBLE_L:
	case TokenType::T_DOUBLE:
		return SingleValueType::Double;
	case TokenType::T_VOID:
		return SingleValueType::Void;
	case TokenType::T_STR_L:
	case TokenType::T_STRING:
		return PtrType{};
	case TokenType::T_CLASS:
	case TokenType::T_STRUCT:
	case TokenType::T_ENUM:
	case TokenType::T_IDENTIFIER:
		return StructType{};
	default:
		ASSERT_NOT_REACHABLE();
	}
}


}