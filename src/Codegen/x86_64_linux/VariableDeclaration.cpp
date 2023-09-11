/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-09.
//

#include "../../libs/Utils.h"
#include "Generator.h"
#include "../../libs/Println.h"

namespace alx {

void Generator::generate_variables(const std::unique_ptr<ASTNode>& node)
{
	auto lhs = reinterpret_cast<VariableDeclaration*>(node.get());
	auto lhs_type = lhs->Type();
	if (!lhs->Value())
	{
		if (!lhs->Ident().Assignable())
			error("Must initialise constant values");
		add_to_stack(lhs);
		return;
	}
	if (lhs->Value()->class_name() == "NumberLiteral")
	{
		auto rhs = dynamic_cast<NumberLiteral*>(lhs->Value());
		add_to_stack(lhs); // Increment bp
		m_asm << "mov " << bytes_to_data_size(m_type_size[lhs_type]) << " [rbp-" << bp << "], " << rhs->Value() << "\n";
		return;
	}
	else if (lhs->Value()->class_name() == "Identifier")
	{
		// - Get the underlying identifier of the rhs
		auto value_identifier = dynamic_cast<Identifier*>(lhs->Value());
		// - Check if the rhs has been initialised
		if (!m_stack.at(value_identifier->Name()).first->Value()) {
			error("Cannot assign an uninitialised variable");
		}

		// - Get underlying types, type sizes, whether the value we're trying to assign is unsigned (and therefore
		//	 we should extend the value)
		auto rhs_type = m_stack.at(value_identifier->Name()).first->Type();

		auto lhs_size = m_type_size[lhs_type];
		auto rhs_size = m_type_size[rhs_type];

		bool rhs_unsigned = is_unsigned(rhs_type);
		auto rhs_ptr_offset = m_stack.at(value_identifier->Name()).second;

		// If rhs is a byte or a word, use movzx/movsx to extend it to a dword or qword, otherwise - regular mov
		if (rhs_size <= 2)
			m_asm << (rhs_unsigned ? "movzx " : "movsx ");
		else
			m_asm << "mov ";

		// If rhs is a byte or a word, use the data size of lhs (because movzx/movsx already extended it)
		m_asm << bytes_to_reg(rhs_size <= 2 ? lhs_size : rhs_size) << ", " << bytes_to_data_size(rhs_size) << " [rbp-"
			  << rhs_ptr_offset << "]\n";

		// If rhs is a dword and lhs is a qword, Convert Doubleword toQuadword
		if (lhs_size == 8 && rhs_size == 4)
			m_asm << "cdqe\n";

		add_to_stack(lhs); // Increment bp
		m_asm << "mov " << bytes_to_data_size(lhs_size) << " [rbp-" << bp << "], " << bytes_to_reg(lhs_size) << "\n";

		// e.g. long a = 123; int b = a;
		if (m_type_size[lhs_type] < m_type_size[rhs_type])
			warning("Narrowing conversion from {} to {}", token_to_string(rhs_type), token_to_string(lhs_type));
		return;
	}

	ASSERT_NOT_REACHABLE();
}

}