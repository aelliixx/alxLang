/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-08.
//

#include "Ir.h"
#include "../libs/Println.h"

namespace alx::ir {

void IR::Generate()
{

}

void IR::Dump()
{

}

void Function::PrintNode()
{
	print("define {} ", IR::EnumToString(Linkage));
}

std::string IR::EnumToString(std::variant<Linkage, ParameterAttributes, SingleValueType> variant, int value)
{
	struct Visitor
	{
		int value;
		std::string operator()(Linkage linkage)
		{
			switch (linkage)
			{
			case Linkage::External:
				return "external";
			case Linkage::Internal:
				return "internal";
			}
		}
		std::string operator()(ParameterAttributes attributes)
		{
			switch (attributes)
			{
			case ParameterAttributes::ZeroExt:
				return "zeroext";
			case ParameterAttributes::SignExt:
				return "signext";
			case ParameterAttributes::Align:
				return getFormatted("align {}", value);
			}
		}
		std::string operator()(SingleValueType type)
		{
			switch (type)
			{
			case SingleValueType::Void:
				return "void";
			case SingleValueType::Int:
				return getFormatted("i{}", value);
			case SingleValueType::Half:
				return "half";
			case SingleValueType::Float:
				return "float";
			case SingleValueType::Double:
				return "double";
			}
		}
	};
	Visitor visitor{ value };
	return std::visit(visitor, variant);
}
}
