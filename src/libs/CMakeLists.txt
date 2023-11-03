cmake_minimum_required(VERSION 3.24)


add_library(Colour Colours.cpp
        ErrorHandler.h
        Println.cpp)
add_library(Print Println.h)
add_library(Argparse argparse.hpp)
add_library(ctre ctre.hpp)

set_target_properties(Print PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(Argparse PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(ctre PROPERTIES LINKER_LANGUAGE CXX)
