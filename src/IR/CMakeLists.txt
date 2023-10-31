cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)

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
)
