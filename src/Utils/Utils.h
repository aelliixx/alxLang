/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-09.
//

#pragma once
#include <string>
#include <assert.h>
#include <source_location>


#define OUTPUT_IR_TO_STRING 0 // For facilitating IR tests by redirecting output to a string
#if OUTPUT_IR_TO_STRING
#define DISABLE_COLOURS
#endif

#define CRASH_ON_ASSERTION_FAILURE 0

namespace alx {
#if CRASH_ON_ASSERTION_FAILURE
#define ASSERT_NOT_IMPLEMENTED() assert(false && "Not implemented")
#define ASSERT_NOT_IMPLEMENTED_MSG(exp) assert(false && "Not implemented " #exp)
#else
#define ASSERT_NOT_IMPLEMENTED() throw std::runtime_error("Not implemented")
#define ASSERT_NOT_IMPLEMENTED_MSG(exp) throw std::runtime_error("Not implemented: " + exp)
#endif

#if CRASH_ON_ASSERTION_FAILURE
#define ASSERT_NOT_REACHABLE() assert(false && "Not reachable")
#else
#define ASSERT_NOT_REACHABLE() throw std::runtime_error("Not reachable")
#endif

#define SHOULD(expr) if (!(expr)) alx::println(alx::Colour::Orange, "{}:{}: {}", \
        std::source_location::current().file_name(), std::source_location::current().line(), "Assertion failed: " #expr)

#if CRASH_ON_ASSERTION_FAILURE
#define MUST(expr) assert(expr && "Assertion failed: " #expr)
#else
#define MUST(expr) if (!(expr)) throw std::runtime_error("Assertion failed: " #expr)
#endif
}
