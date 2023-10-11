/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-10.
//

#include "Ir.h"

namespace alx::ir {

#define BLUE Colour{70,160,220}
#define GREEN Colour{60,197,172}

void IR::Dump()
{
	println("Dumping IR with --dump-ir:\n");
	struct IrVisitor
	{
		void operator()(const std::unique_ptr<Function>& func)
		{
			func->PrintNode();
		}
	};
	for (const auto& node : m_ir)
		std::visit(IrVisitor{}, node);
}

void FunctionParameter::PrintNode() const
{
	print(GREEN, "{} ", IR::TypesToString(Type));
	for (const auto& attribute : Attributes)
		print("{} ", IR::EnumToString(attribute));
	print(BLUE, "{}{}",
		  (Visibility == VisibilityAttribute::Local ? "%" : "@"),
		  Name);
}

void Function::PrintNode() const
{
	print(BLUE, "define ");
	print("{}", IR::EnumToString(Linkage));
	print(BLUE, " {}{}",
		  Visibility == VisibilityAttribute::Global ? "@" : "%",
		  Name);

	if (!Arguments.empty())
	{
		print("(");
		auto separator = "";
		for (const auto& arg : Arguments)
		{
			print(separator);
			arg.PrintNode();
			separator = ", ";
		}
		print(")");
	}

	println(" {");

	struct BodyVisitor
	{
		void operator()(const LabelType& label)
		{
			println(BLUE, "{}: ", label.Name);
		}
	};

	for (const auto& child : Body)
		std::visit(BodyVisitor{}, child);
	println("}");
}

std::string IR::TypesToString(const Types& types)
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
			}
			ASSERT_NOT_REACHABLE();
		}
		std::string operator()(IntType intType) const
		{
			return getFormatted("i{}", intType.Size);
		}
		std::string operator()(const LabelType&) const
		{
			return "label";
		}
		std::string operator()(PtrType) const
		{
			return "ptr";
		}
		std::string operator()(const StructType&) const
		{
			return "struct";
		}
		std::string operator()(const ArrayType&) const
		{
			return "array";
		}
	};
	return std::visit(TypeVisitor{}, types);
}

std::string IR::EnumToString(std::variant<LinkageType, ParameterAttributes> variant)
{
	struct Visitor
	{
		int value;
		std::string operator()(LinkageType linkage)
		{
			switch (linkage)
			{
			case LinkageType::External:
				return "external";
			case LinkageType::Internal:
				return "internal";
			}
			ASSERT_NOT_REACHABLE();
		}
		std::string operator()(const ParameterAttributes& paramAttributes)
		{
			struct ParameterVisitor
			{
				std::string operator()(ParamAttributes attributes)
				{
					switch (attributes)
					{
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
	switch (tokenType)
	{
	case TokenType::T_CHAR_L:
	case TokenType::T_CHAR:
	case TokenType::T_BOOL:
	case TokenType::T_TRUE:
	case TokenType::T_FALSE:
		return IntType{ 8 };
	case TokenType::T_INT_L:
	case TokenType::T_INT:
		return IntType{ 32 };
	case TokenType::T_LONG:
		return IntType{ 64 };
	case TokenType::T_SHORT:
		return IntType{ 16 };
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