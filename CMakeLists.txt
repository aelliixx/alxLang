cmake_minimum_required(VERSION 3.24)
project(alxlang)
set(CMAKE_CXX_STANDARD 23)
add_compile_options(-Werror -Wextra -Wall -Wpedantic)
add_compile_options(-Wno-unknown-pragmas)

option(ALX_BUILD_TESTS "Build tests" ON)

if (ALX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

add_subdirectory(src)

set(SOURCE_FILES src/main.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} Colour Compiler Codegen Parser Tokeniser IR AST Print Utils ctre)
target_include_directories(${PROJECT_NAME} PRIVATE src)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "acc")
