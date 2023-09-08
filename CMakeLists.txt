cmake_minimum_required(VERSION 3.24)
project(alxlang)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wextra)

add_library(Colour src/libs/Colours.cpp)
add_library(Print src/libs/Println.cpp)
add_library(AST src/AST/Ast.cpp src/AST/Types.h)
add_library(Tokeniser src/Tokeniser/Tokeniser.cpp)
add_library(Codegen src/Codegen/Generator.cpp)
add_library(Parser src/Parser/Parser.cpp src/Parser/ParseFunction.cpp
        src/Parser/ParseVariable.cpp
        src/Parser/ParseStatement.cpp
        src/Parser/ParseReturn.cpp
        src/Parser/ParseLiteral.cpp
        src/Parser/ParseBinaryExpression.cpp)

set(SOURCE_FILES src/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} Codegen Parser Tokeniser AST Print Colour)
target_include_directories(${PROJECT_NAME} PRIVATE src)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "acc")
