cmake_minimum_required(VERSION 3.24)


add_subdirectory(IR)
add_subdirectory(Parser)
add_subdirectory(libs)
add_subdirectory(AST)
add_subdirectory(Codegen)
add_subdirectory(Tokeniser)
add_subdirectory(Utils)

add_library(Compiler Compiler.cpp)
target_link_libraries(Compiler InstSel)