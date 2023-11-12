cmake_minimum_required(VERSION 3.24)

add_library(InstSel
        Target.h
        InstructionSelection.cpp
        MachineInstruction.cpp
        MachineInstruction.h
        PrintInstructionSelection.cpp
)

add_library(X86
        X86/X86ISel.cpp
        X86/X86InstructionMatcher.h
        X86/X86Instructions.h
)


target_link_libraries(InstSel X86)
