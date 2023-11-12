//
// Created by aelliixx on 2023-11-11.
//

#pragma once

namespace alx::ir::x86 {

struct InstructionMatcher {
	void operator()(LabelType) const {}
	void operator()(ReturnInst) const {}
	void operator()(Variable) const {}
	void operator()(StoreInst) const {}
	void operator()(BranchInst) const {}
};

}