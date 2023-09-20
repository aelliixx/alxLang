cmake_minimum_required(VERSION 3.24)
project(alxlang)
set(CMAKE_CXX_STANDARD 20)
#add_compile_options(-Wextra -Wall -Wpedantic -Wimplicit-fallthrough=0)

option(ALX_BUILD_TESTS "Build tests" ON)

if(ALX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

add_library(Colour src/libs/Colours.cpp
        src/libs/ErrorHandler.h
        src/libs/Println.cpp)
add_library(Print src/libs/Println.h)
add_library(AST src/AST/Ast.cpp
        src/AST/PrintNode.cpp)
add_library(Tokeniser src/Tokeniser/Tokeniser.cpp)
add_library(Utils src/Utils/Utils.h
        src/Utils/Types.h
        src/Utils/Flags.h
        src/libs/argparse.hpp)
add_library(Compiler src/Compiler.h src/Compiler.cpp)

set_target_properties(Utils PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(Print PROPERTIES LINKER_LANGUAGE CXX)

add_library(Codegen src/Codegen/x86_64_linux/BlockGenerator.cpp
        src/Codegen/x86_64_linux/BinaryExpression.cpp
        src/Codegen/x86_64_linux/VariableDeclaration.cpp
        src/Codegen/x86_64_linux/ProgramGenerator.cpp
        src/Codegen/x86_64_linux/IfStatement.cpp
        src/Codegen/x86_64_linux/UnaryExpression.cpp
        src/Codegen/x86_64_linux/LoopGenerator.cpp
        src/Codegen/x86_64_linux/Structs.cpp)

add_library(Parser src/Parser/Parser.cpp src/Parser/ParseFunction.cpp
        src/Parser/ParseVariable.cpp
        src/Parser/ParseStatement.cpp
        src/Parser/ParseReturn.cpp
        src/Parser/ParseLiteral.cpp
        src/Parser/ParseBinaryExpression.cpp
        src/Parser/ParseConditionalStatement.cpp
        src/Parser/ParseUnaryExpression.cpp
        src/Parser/ParseClass.cpp
        src/Parser/ParseMemberExpression.cpp)

set(SOURCE_FILES src/main.cpp)


#include(FetchContent)
#FetchContent_Declare(
#        argparse
#        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
#)
#FetchContent_MakeAvailable(argparse)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} Colour Compiler Codegen Parser Tokeniser AST Print Utils)
target_include_directories(${PROJECT_NAME} PRIVATE src)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "acc")
