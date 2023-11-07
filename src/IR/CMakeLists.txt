cmake_minimum_required(VERSION 3.24)


add_library(IR Ir.cpp
        PrintNode.cpp
        Variables.h
        Types.h
        Attributes.h
        Instructions.h
        Values.h
        Lowering/Variables.cpp
        Lowering/BinaryExpression.cpp
        Lowering/Function.cpp
        Lowering/ReturnStatement.cpp
        Lowering/UnaryExpression.cpp
        Lowering/Conditionals.cpp
        Lowering/InstructionSelection/InstructionSelector.cpp
        Lowering/InstructionSelection/InstructionSelector.h
        Lowering/InstructionSelection/Targets/X86.h
)
