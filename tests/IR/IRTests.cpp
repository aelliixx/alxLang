/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-10-31.
//

#include <string>
#include "../../src/Compiler.h"

#if OUTPUT_IR_TO_STRING
using namespace alx;

const DebugFlags df{ .quiet_mode = true };

int emptyFunction()
{
	auto code = "int main() {}";
	Compiler compiler{ code, "EmptyFunction", {}, df };
	compiler.Compile();
	auto generated = compiler.GetIr();
	auto expected = R"(define internal i32 @main() {
entry:
  ret void
}
)";
	if (generated != expected)
	{
		std::cout << "Generated:\n" << generated << "\nExpected:\n" << expected << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int integerTypes()
{
	auto code = "int main() {\n"
				"    bool a;\n"
				"    short b;\n"
				"    int c;\n"
				"    long d;\n"
				"}";
	Compiler compiler{ code, "IntegerTypes", {}, df };
	compiler.Compile();
	auto generated = compiler.GetIr();
	auto expected = R"(define internal i32 @main() {
entry:
  %a = alloca i8, align 1 
  %b = alloca i16, align 2 
  %c = alloca i32, align 4 
  %d = alloca i64, align 8 
  ret void
}
)";
	if (generated != expected)
	{
		std::cout << "Generated:\n" << generated << "\nExpected:\n" << expected << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int integerAssignment()
{
	auto code = R"(int main() {
int a = 10;
int b;
b = 20;
}
)";
	Compiler compiler{ code, "IntegerTypes", {}, df };
	compiler.Compile();
	auto generated = compiler.GetIr();
	auto expected = R"(define internal i32 @main() {
entry:
  %a = alloca i32, align 4
  store i32 10, ptr %a, align 4 
  %b = alloca i32, align 4
  store i32 20, ptr %b, align 4
  ret void
}
)";
	if (generated != expected)
	{
		std::cout << "Generated:\n" << generated << "\nExpected:\n" << expected << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
	std::string arg = argv[1];

	if (arg == "EmptyFunction")
		return emptyFunction();
	else if (arg == "IntegerTypes")
		return integerTypes();
	else if (arg == "IntegerAssignment")
		return integerAssignment();
	else
		std::cout << "Unknown test: " << arg;
	return EXIT_FAILURE;
}
#else
int main()
{
	std::cout << "OUTPUT_IR_TO_STRING is set to false";
	return EXIT_FAILURE;
}
#endif


