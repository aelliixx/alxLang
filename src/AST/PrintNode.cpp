/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-16.
//

#include "Ast.h"

namespace alx {
static std::string access_mode_to_string(AccessModeType mode)
{
	switch (mode)
	{
	case AccessModeType::a_global:
		return "global";
	case AccessModeType::a_scoped:
		return "scoped";
	case AccessModeType::a_public:
		return "public";
	case AccessModeType::a_protected:
		return "protected";
	case AccessModeType::a_private:
		return "private";
	default:
		ASSERT_NOT_REACHABLE();
	}
}

void ScopeNode::dump_nodes(const std::vector<std::unique_ptr<ASTNode>>& Children, int indent)
{
	for (const auto& child : Children)
	{
		child->PrintNode(indent);
	}
}
void ScopeNode::PrintNode(int indent) const
{
	dump_nodes(m_children, indent);
}

void Program::PrintNode(int indent) const
{
	print(Colour::LightPurple, "{>}Program", indent);
	println(": {");
	dump_nodes(Children(), 2);
	println("}");
}

void Identifier::PrintNode(int indent) const
{
	println("{>}Variable: {}", indent, m_name);
}
void NumberLiteral::PrintNode(int indent) const
{
	println("{>}NumberLiteral: ", indent);
	switch (m_type)
	{
	case TokenType::T_INT_L:
		println("{>}type: {}", indent + 2, "int");
		println("{>}value: {}", indent + 2, m_value);
		break;
	case TokenType::T_FLOAT_L:
		println("{>}type: {}", indent + 2, "float");
		println("{>}value: {}", indent + 2, m_value);
		break;
	case TokenType::T_DOUBLE_L:
		println("{>}type: {}", indent + 2, "double");
		println("{>}value: {}", indent + 2, m_value);
		break;
	case TokenType::T_CHAR_L:
		println("{>}type: {}", indent + 2, "char");
		println("{>}value: {}", indent + 2, m_value);
		break;
	case TokenType::T_FALSE:
	case TokenType::T_TRUE:
		println("{>}type: {}", indent + 2, "bool");
		println("{>}value: {}", indent + 2, AsBool());
		break;
	default:
		assert(false && "Invalid literal type");
	}
}
void VariableDeclaration::PrintNode(int indent) const
{
	println("{>}VariableDeclaration: {", indent);
	println("{>}type: {}", indent + 2, TypeName());
	println("{>}identifier: {}", indent + 2, m_identifier->Name());
	println("{>}value:", indent + 2);
	if (m_value)
		m_value->PrintNode(indent + 4);
	else
		println("{>}null", indent + 4);
	println("{>}}", indent);
}
void BlockStatement::PrintNode(int indent) const
{
	println("{>}BlockStatement: {", indent);
	dump_nodes(Children(), indent + 2);
	println("{>}}", indent);
}

void FunctionDeclaration::PrintNode(int indent) const
{
	println("{>}FunctionDeclaration: {", indent);
	println("{>}name: {}", indent + 2, Name());
	println("{>}return_type: {}", indent + 2, token_to_string(m_return_type));
	println("{>}parameters:", indent + 2);
	if (m_parameters.empty())
		println("{>}[none]", indent + 4);
	else
		for (const auto& param : m_parameters)
			println("{>}type: {}, identifier: {}",
					indent + 4,
					token_to_string(param->TypeAsPrimitive()),
					param->Name());
	println("{>}body:", indent + 2);
	m_body->PrintNode(indent + 4);
	println("{>}}", indent);
}
void BinaryExpression::PrintNode(int indent) const
{
	println("{>}BinaryExpression: {", indent);
	println("{>}constexpr: {}", indent + 2, m_constexpr);
	println("{>}lhs:", indent + 2);
	m_lhs->PrintNode(indent + 4);
	println("{>}operator: {}", indent + 2, token_to_string(m_binary_op));
	println("{>}rhs:", indent + 2);
	m_rhs->PrintNode(indent + 4);
	println("{>}}", indent);
}

void StringLiteral::PrintNode(int indent) const
{
	println("{>}ReturnStatement: {\n{>}value: {}\n{>}length: ", indent, indent + 2, m_value, indent + 2, Length());
	println("{>}}", indent);
}
void ReturnStatement::PrintNode(int indent) const
{
	println("{>}ReturnStatement: {\n{>}argument:", indent, indent + 2);
	m_argument->PrintNode(indent + 4);
	println("{>}}", indent);
}
void Expression::PrintNode(int indent) const
{

}

void IfStatement::PrintNode(int indent) const
{
	println("{>}IfStatement: {", indent);
	println("{>}condition:", indent + 2);
	m_condition->PrintNode(indent + 4);
	println("{>}body:", indent + 2);
	m_body->PrintNode(indent + 4);
	if (HasAlternate())
	{
		println("{>}alternate:", indent + 2);
		m_alternate.value()->PrintNode(indent + 4);
	}
	println("{>}}", indent);

}

void UnaryExpression::PrintNode(int indent) const
{
	println("{>}UnaryExpression: {", indent);
	println("{>}operator: {}", indent + 2, token_to_string(m_unary_op));
	println("{>}rhs:", indent + 2);
	m_rhs->PrintNode(indent + 4);
	println("{>}}", indent);
}

void WhileStatement::PrintNode(int indent) const
{
	println("{>}WhileStatement: {", indent);
	println("{>}condition:", indent + 2);
	m_condition->PrintNode(indent + 4);
	println("{>}body:", indent + 2);
	m_body->PrintNode(indent + 4);
	println("{>}}", indent);
}
void StructDeclaration::PrintNode(int indent) const
{
	println("{>}StructDeclaration: {", indent);
	println("{>}member variables:", indent + 2);
	if (!m_members.empty())
	{
		for (const auto& var : m_members)
		{
			auto member = static_cast<VariableDeclaration*>(var.get());
			println("{>}type: {}, identifier: {}, access mode: {}, default initialised?: {}",
					indent + 4,
					token_to_string(member->TypeAsPrimitive()),
					member->Name(),
					access_mode_to_string(member->AccessMode()),
					member->Value() != nullptr);
		}
	}
	if (!m_methods.empty())
	{
		println("{>}member methods:", indent + 2);
		for (const auto& var : m_methods)
		{
			auto method = static_cast<FunctionDeclaration*>(var.get());
			println("{>}return type: {}, identifier: {}, default initialised?: {}",
					indent + 4,
					token_to_string(method->ReturnType()),
					method->Name());
		}
	}
	println("{>}}", indent);
}

void MemberExpression::PrintNode(int indent) const
{
	println("{>}MemberExpression: {", indent);
	println("{>}object: {}", indent + 2, m_object->Name());
	println("{>}accessor: {}", indent + 2, token_to_string(m_accessor));
	println("{>}property: {}", indent + 2, m_member->Name());
	println("{>}}", indent);
}

}