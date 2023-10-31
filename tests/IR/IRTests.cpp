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
	if (generated != expected) {
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
	if (generated != expected) {
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
	if (generated != expected) {
		std::cout << "Generated:\n" << generated << "\nExpected:\n" << expected << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int integerAddition()
{
	auto code = R"(int main() {
    int a = 10;
    int b = a + 20;
    b = a + 30;
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
  %0 = load i32, ptr %a, align 4
  %1 = add i32 %0, 20
  store i32 %1, ptr %b, align 4
  %2 = load i32, ptr %a, align 4
  %3 = add i32 %2, 30
  store i32 %3, ptr %b, align 4
  ret void
}
)";

	if (generated != expected) {
		std::cout << "Generated:\n" << generated << "\nExpected:\n" << expected << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int integerSubtraction()
{
	auto code = R"(int main() {
    int a = 10;
    int b = a - 20;
    b = a - 30;
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
  %0 = load i32, ptr %a, align 4
  %1 = sub i32 %0, 20
  store i32 %1, ptr %b, align 4
  %2 = load i32, ptr %a, align 4
  %3 = sub i32 %2, 30
  store i32 %3, ptr %b, align 4
  ret void
}
)";

	if (generated != expected) {
		std::cout << "Generated:\n" << generated << "\nExpected:\n" << expected << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int integerMultiplication()
{
	auto code = R"(int main() {
    int a = 10;
    int b = a * 20;
    b = a * 30;
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
  %0 = load i32, ptr %a, align 4
  %1 = mul i32 %0, 20
  store i32 %1, ptr %b, align 4
  %2 = load i32, ptr %a, align 4
  %3 = mul i32 %2, 30
  store i32 %3, ptr %b, align 4
  ret void
}
)";

	if (generated != expected) {
		std::cout << "Generated:\n" << generated << "\nExpected:\n" << expected << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int integerSignedDivision()
{
	auto code = R"(int main() {
    int a = 10;
    int b = a / 20;
    b = a / 30;
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
  %0 = load i32, ptr %a, align 4
  %1 = sdiv i32 %0, 20
  store i32 %1, ptr %b, align 4
  %2 = load i32, ptr %a, align 4
  %3 = sdiv i32 %2, 30
  store i32 %3, ptr %b, align 4
  ret void
}
)";
	if (generated != expected) {
		std::cout << "Generated:\n" << generated << "\nExpected:\n" << expected << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
	std::string arg = argv[1];

	if (arg == "EmptyFunction") return emptyFunction();
	else if (arg == "IntegerTypes")
		return integerTypes();
	else if (arg == "IntegerAssignment")
		return integerAssignment();
	else if (arg == "IntegerAddition")
		return integerAddition();
	else if (arg == "IntegerSubtraction")
		return integerSubtraction();
	else if (arg == "IntegerMultiplication")
		return integerMultiplication();
	else if (arg == "IntegerSignedDivision")
		return integerSignedDivision();
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
