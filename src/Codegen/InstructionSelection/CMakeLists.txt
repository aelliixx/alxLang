cmake_minimum_required(VERSION 3.24)

add_library(InstSel
        InstructionSelection.cpp
        Target.h
        MachineInstruction.cpp
        MachineInstruction.h
        PrintInstructionSelection.cpp
)

add_library(X86
        X86/X86ISel.cpp
)

target_link_libraries(InstSel X86)