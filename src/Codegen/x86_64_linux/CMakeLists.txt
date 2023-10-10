cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)

add_library(Codegen BlockGenerator.cpp
        BinaryExpression.cpp
        VariableDeclaration.cpp
        ProgramGenerator.cpp
        IfStatement.cpp
        UnaryExpression.cpp
        LoopGenerator.cpp
        Structs.cpp)