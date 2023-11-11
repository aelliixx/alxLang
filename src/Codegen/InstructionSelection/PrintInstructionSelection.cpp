//
// Created by aelliixx on 2023-11-11.
//

#include "Target.h"
#include "InstructionSelection.h"

namespace alx {

std::string Target::TargetToString(const std::variant<ArchType, OSType, ObjectFormatType>& target)
{
	struct Visitor {
		std::string operator()(const ArchType arch) const
		{
			switch (arch) {
			case ArchType::X86_64:
				return "x86_64";
			}
			return "Unknown";
		}
		std::string operator()(const OSType os) const
		{
			switch (os) {
			case OSType::Linux:
				return "Linux";
			}
			return "Unknown";
		}
		std::string operator()(const ObjectFormatType objectFormat) const
		{
			switch (objectFormat) {
			case ObjectFormatType::ELF:
				return "ELF";
			}
			return "Unknown";
		}
	} visitor;
	return std::visit(visitor, target);
}
namespace ir {

void InstructionSelection::PrintInstructions() const
{
	println("Opt-pipeline IR Instruction Selection:");
	println("Target: {} {}", Target::TargetToString(m_target.Arch()), Target::TargetToString(m_target.OS()));
}

}
}
