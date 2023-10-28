cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)

add_library(IR Ir.cpp
        PrintNode.cpp
        Types.h
        Attributes.h
        Instructions.h
        Values.h)
