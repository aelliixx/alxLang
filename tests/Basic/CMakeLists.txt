cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)

add_executable(Basic Basic.cpp)
target_link_libraries(Basic Compiler Codegen Parser Tokeniser IR AST Utils Print Colour)

add_test(NAME EmptyFile COMMAND Basic "0")
set_property(TEST EmptyFile PROPERTY WILL_FAIL TRUE)
add_test(NAME MainFunction COMMAND Basic "main")
