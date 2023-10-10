cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(IR)
add_subdirectory(Parser)
add_subdirectory(libs)
add_subdirectory(AST)
add_subdirectory(Codegen)
add_subdirectory(Tokeniser)

add_library(Compiler Compiler.cpp)