cmake_minimum_required(VERSION 3.24)


add_library(Utils Utils.h
        Types.h
        Flags.h)

set_target_properties(Utils PROPERTIES LINKER_LANGUAGE CXX)
