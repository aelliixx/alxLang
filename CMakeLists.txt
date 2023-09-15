cmake_minimum_required(VERSION 3.24)
project(alxlang)
set(CMAKE_CXX_STANDARD 20)
#add_compile_options(-Wextra)

add_library(Colour src/libs/Colours.cpp)
add_library(Print src/libs/Println.h)
add_library(AST src/AST/Ast.cpp)
add_library(Tokeniser src/Tokeniser/Tokeniser.cpp)
add_library(Utils src/libs/Utils.h src/AST/Types.h)
add_library(PatternMatcher src/libs/PatternMatcher.h)

set_target_properties(Utils PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(Print PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(PatternMatcher PROPERTIES LINKER_LANGUAGE CXX)

add_library(Codegen src/Codegen/x86_64_linux/BlockGenerator.cpp
        src/Codegen/x86_64_linux/BinaryExpression.cpp
        src/Codegen/x86_64_linux/VariableDeclaration.cpp
        src/Codegen/x86_64_linux/ProgramGenerator.cpp
        src/Codegen/x86_64_linux/IfStatement.cpp
        src/Codegen/x86_64_linux/UnaryExpression.cpp
        src/Codegen/x86_64_linux/LoopGenerator.cpp)

add_library(Parser src/Parser/Parser.cpp src/Parser/ParseFunction.cpp
        src/Parser/ParseVariable.cpp
        src/Parser/ParseStatement.cpp
        src/Parser/ParseReturn.cpp
        src/Parser/ParseLiteral.cpp
        src/Parser/ParseBinaryExpression.cpp
        src/Parser/ParseConditionalStatement.cpp
        src/Parser/ParseUnaryExpression.cpp)

set(SOURCE_FILES src/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} Codegen Parser Tokeniser AST Print Colour Utils)
target_include_directories(${PROJECT_NAME} PRIVATE src)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "acc")
