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

#define SHOULD(expr)                                                                                                   \
	if (!(expr))                                                                                                       \
	alx::println(alx::Colour::Orange,                                                                                  \
				 "{}:{}: {}",                                                                                          \
				 std::source_location::current().file_name(),                                                          \
				 std::source_location::current().line(),                                                               \
				 "Assertion failed: " #expr)

#if CRASH_ON_ASSERTION_FAILURE
#define MUST(expr) assert(expr && "Assertion failed: " #expr)
#else
#define MUST(expr)                                                                                                     \
	if (!(expr))                                                                                                       \
	throw std::runtime_error("Assertion failed: " #expr)
#endif

#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && !defined(__NVCOMPILER)
#define ALX_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
#define ALX_GCC_VERSION 0
#endif

#ifndef ALWAYS_INLINE
#if ALX_GCC_VERSION
#define ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define ALWAYS_INLINE inline
#endif
#endif

#ifndef BENCHMARK_HASHMAP
#define BENCHMARK_HASHMAP(map)                                                                                         \
	do {                                                                                                               \
		size_t collisions{};                                                                                           \
		double avgCollidedBucketSize{};                                                                                \
		size_t largestBucket = 0;                                                                                      \
		for (size_t i = 0; i < map.bucket_count(); ++i) {                                                              \
			if (map.bucket_size(i) > 1) {                                                                              \
				++collisions;                                                                                          \
				avgCollidedBucketSize += map.bucket_size(i);                                                           \
			}                                                                                                          \
			largestBucket = std::max(largestBucket, map.bucket_size(i));                                               \
		}                                                                                                              \
		avgCollidedBucketSize /= collisions;                                                                           \
		println("----------------------------");                                                                       \
		println("Analysing hashmap: {;60;110;199}", #map);                                                              \
		println("There are {} buckets", map.bucket_count());                                                           \
		println("There are {} items in the hashmap", map.size());                                                      \
		if (collisions) {                                                                                              \
			println(Colour::Red, "Found {} collisions", collisions);                                                   \
			println("Average collided bucket size: {}, largest bucket: {}", avgCollidedBucketSize, largestBucket);     \
		}                                                                                                              \
		else                                                                                                           \
			println(Colour::Green, "Found no collisions!");                                                            \
		println("----------------------------");                                                                       \
	} while (false)
#endif

}
