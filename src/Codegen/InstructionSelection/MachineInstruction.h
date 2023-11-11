//
// Created by aelliixx on 2023-11-11.
//

#pragma once
#include <string>


namespace alx::ir {

struct MachineInstruction {
	virtual ~MachineInstruction() = default;
	virtual std::string Name() = 0;
};

} // namespace alx::ir
