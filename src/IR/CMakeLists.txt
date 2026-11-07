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
)

add_library(ISel
        ISel/ISel.cpp
        ISel/ISel.h
        ISel/Targets/x86-64/Instructions.h
        ISel/Targets/x86-64/X86_64_ISel.cpp
        ISel/Targets/x86-64/X86_64_ISel.h
        ISel/PrintMIR.cpp
)
