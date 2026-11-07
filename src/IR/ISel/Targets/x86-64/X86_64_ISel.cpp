//
// Created by aelliixx on 2023-11-13.
//

#include "X86_64_ISel.h"

namespace alx::ir::x86_64 {

void X86_64_ISel::SelectFunction(const Function& func)
{
	for (const auto& block : func.Blocks) {
		for (const auto& inst : block.Body) {
			struct BodyVisitor {
				void operator()(const LabelType&) const {}
				void operator()(const ReturnInst&) const {}
				void operator()(const Variable&) const {}
				void operator()(const StoreInst&) const {}
				void operator()(const BranchInst&) const {}
			};
			std::visit(BodyVisitor{}, inst);
		}
	}
}

void X86_64_ISel::SelectInstruction(const Instruction& inst) {}


void X86_64_ISel::SelectInstructions()
{
	println("Selecting instructions:");
	for (const auto& node : m_ir) {
		if (std::holds_alternative<Function>(node)) {
			SelectFunction(std::get<Function>(node));
		}
	}


	println("Done selecting instructions");
}


}