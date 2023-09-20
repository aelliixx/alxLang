/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-19.
//

#include <string>
#include "../src/Compiler.h"

int main(int, char* argv[])
{
	using namespace alx;
	const DebugFlags df{ .quiet_mode = true };
	std::string arg = argv[1];
	if (arg == "0")
	{
		auto code =
			"int main() {\n"
			"   int a = 10;\n"
			"   int b = a + 10;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov DWORD [rbp-4], 10\n"
						"mov eax, DWORD [rbp-4]\n"
						"add eax, 10\n"
						"mov DWORD [rbp-8], eax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "1")
	{
		auto code =
			"int main() {\n"
			"   int a = 10;\n"
			"   int b = a - 10;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov DWORD [rbp-4], 10\n"
						"mov eax, DWORD [rbp-4]\n"
						"sub eax, 10\n"
						"mov DWORD [rbp-8], eax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "2")
	{
		auto code =
			"int main() {\n"
			"    short a = 10;\n"
			"    long b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov WORD [rbp-2], 10\n"
						"movsx rax, WORD [rbp-2]\n"
						"mov QWORD [rbp-16], rax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "3")
	{
		auto code =
			"int main() {\n"
			"    int a = 10;\n"
			"    long b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov DWORD [rbp-4], 10\n"
						"mov eax, DWORD [rbp-4]\n"
						"cdqe\n"
						"mov QWORD [rbp-16], rax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "4")
	{
		auto code =
			"int main() {\n"
			"    long a = 10;\n"
			"    int b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov QWORD [rbp-8], 10\n"
						"mov rax, QWORD [rbp-8]\n"
						"mov DWORD [rbp-12], eax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "5")
	{
		auto code =
			"int main() {\n"
			"    char a = 10;\n"
			"    long b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 10\n"
						"movsx rax, BYTE [rbp-1]\n"
						"mov QWORD [rbp-16], rax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "6")
	{
		auto code =
			"int main() {\n"
			"    bool a = 10;\n"
			"    long b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 1\n"
						"movzx rax, BYTE [rbp-1]\n"
						"mov QWORD [rbp-16], rax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "7")
	{
		auto code =
			"int main() {\n"
			"    int a = 10;\n"
			"    char b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov DWORD [rbp-4], 10\n"
						"mov eax, DWORD [rbp-4]\n"
						"mov BYTE [rbp-5], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "8")
	{
		auto code =
			"int main() {\n"
			"    bool a = 10;\n"
			"    int b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 1\n"
						"movzx eax, BYTE [rbp-1]\n"
						"mov DWORD [rbp-8], eax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "9")
	{
		auto code =
			"int main() {\n"
			"    int a = 10;\n"
			"    bool b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov DWORD [rbp-4], 10\n"
						"cmp DWORD [rbp-4], 0\n"
						"setne al\n"
						"mov BYTE [rbp-5], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "10")
	{
		auto code =
			"int main() {\n"
			"    char a = 10;\n"
			"    int b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 10\n"
						"movsx eax, BYTE [rbp-1]\n"
						"mov DWORD [rbp-8], eax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "11")
	{
		auto code =
			"int main() {\n"
			"    char a = 10;\n"
			"    bool b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 10\n"
						"cmp BYTE [rbp-1], 0\n"
						"setne al\n"
						"mov BYTE [rbp-2], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "12")
	{
		auto code =
			"int main() {\n"
			"    short a = 10;\n"
			"    int b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov WORD [rbp-2], 10\n"
						"movsx eax, WORD [rbp-2]\n"
						"mov DWORD [rbp-8], eax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "13")
	{
		auto code =
			"int main() {\n"
			"    short a = 10;\n"
			"    bool b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov WORD [rbp-2], 10\n"
						"cmp WORD [rbp-2], 0\n"
						"setne al\n"
						"mov BYTE [rbp-3], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "14")
	{
		auto code =
			"int main() {\n"
			"    long a = 10;\n"
			"    bool b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov QWORD [rbp-8], 10\n"
						"cmp QWORD [rbp-8], 0\n"
						"setne al\n"
						"mov BYTE [rbp-9], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "15")
	{
		auto code =
			"int main() {\n"
			"    bool a = 10;\n"
			"    char b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 1\n"
						"movzx eax, BYTE [rbp-1]\n"
						"mov BYTE [rbp-2], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "16")
	{
		auto code =
			"int main() {\n"
			"    short a = 10;\n"
			"    char b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov WORD [rbp-2], 10\n"
						"movzx eax, WORD [rbp-2]\n"
						"mov BYTE [rbp-3], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "17")
	{
		auto code =
			"int main() {\n"
			"    long a = 10;\n"
			"    char b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov QWORD [rbp-8], 10\n"
						"mov rax, QWORD [rbp-8]\n"
						"mov BYTE [rbp-9], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "18")
	{
		auto code =
			"int main() {\n"
			"    char a = 10;\n"
			"    char b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 10\n"
						"movzx eax, BYTE [rbp-1]\n"
						"mov BYTE [rbp-2], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "19")
	{
		auto code =
			"int main() {\n"
			"    bool a = 10;\n"
			"    bool b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 1\n"
						"movzx eax, BYTE [rbp-1]\n"
						"mov BYTE [rbp-2], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "20")
	{
		auto code =
			"int main() {\n"
			"    bool a = 10;\n"
			"    short b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 1\n"
						"movzx eax, BYTE [rbp-1]\n"
						"mov WORD [rbp-4], ax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "21")
	{
		auto code =
			"int main() {\n"
			"    char a = 10;\n"
			"    short b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 10\n"
						"movzx ax, BYTE [rbp-1]\n"
						"mov WORD [rbp-4], ax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "22")
	{
		auto code =
			"int main() {\n"
			"    short a = 10;\n"
			"    short b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov WORD [rbp-2], 10\n"
						"movzx eax, WORD [rbp-2]\n"
						"mov WORD [rbp-4], ax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "23")
	{
		auto code =
			"int main() {\n"
			"    int a = 10;\n"
			"    short b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov DWORD [rbp-4], 10\n"
						"mov eax, DWORD [rbp-4]\n"
						"mov WORD [rbp-6], ax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "24")
	{
		auto code =
			"int main() {\n"
			"    long a = 10;\n"
			"    short b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov QWORD [rbp-8], 10\n"
						"mov rax, QWORD [rbp-8]\n"
						"mov WORD [rbp-10], ax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "25")
	{
		auto code =
			"int main() {\n"
			"    int a = 10;\n"
			"    int b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov DWORD [rbp-4], 10\n"
						"mov eax, DWORD [rbp-4]\n"
						"mov DWORD [rbp-8], eax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "26")
	{
		auto code =
			"int main() {\n"
			"    long a = 10;\n"
			"    long b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov QWORD [rbp-8], 10\n"
						"mov rax, QWORD [rbp-8]\n"
						"mov QWORD [rbp-16], rax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}

	else if (arg == "2_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    short a = 10;\n"
			"    long b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov WORD [rbp-2], 10\n"
						"movsx rax, WORD [rbp-2]\n"
						"mov QWORD [rbp-16], rax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "3_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    int a = 10;\n"
			"    long b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov DWORD [rbp-4], 10\n"
						"mov eax, DWORD [rbp-4]\n"
						"cdqe\n"
						"mov QWORD [rbp-16], rax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "4_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    long a = 10;\n"
			"    int b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov QWORD [rbp-8], 10\n"
						"mov rax, QWORD [rbp-8]\n"
						"mov DWORD [rbp-12], eax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "5_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    char a = 10;\n"
			"    long b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 10\n"
						"movsx rax, BYTE [rbp-1]\n"
						"mov QWORD [rbp-16], rax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "6_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    bool a = 10;\n"
			"    long b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 1\n"
						"movzx rax, BYTE [rbp-1]\n"
						"mov QWORD [rbp-16], rax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "7_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    int a = 10;\n"
			"    char b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov DWORD [rbp-4], 10\n"
						"mov eax, DWORD [rbp-4]\n"
						"mov BYTE [rbp-5], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "8_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    bool a = 10;\n"
			"    int b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 1\n"
						"movzx eax, BYTE [rbp-1]\n"
						"mov DWORD [rbp-8], eax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "9_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    int a = 10;\n"
			"    bool b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov DWORD [rbp-4], 10\n"
						"cmp DWORD [rbp-4], 0\n"
						"setne al\n"
						"mov BYTE [rbp-5], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "10_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    char a = 10;\n"
			"    int b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 10\n"
						"movsx eax, BYTE [rbp-1]\n"
						"mov DWORD [rbp-8], eax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "11_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    char a = 10;\n"
			"    bool b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 10\n"
						"cmp BYTE [rbp-1], 0\n"
						"setne al\n"
						"mov BYTE [rbp-2], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "12_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    short a = 10;\n"
			"    int b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov WORD [rbp-2], 10\n"
						"movsx eax, WORD [rbp-2]\n"
						"mov DWORD [rbp-8], eax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "13_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    short a = 10;\n"
			"    bool b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov WORD [rbp-2], 10\n"
						"cmp WORD [rbp-2], 0\n"
						"setne al\n"
						"mov BYTE [rbp-3], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "14_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    long a = 10;\n"
			"    bool b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov QWORD [rbp-8], 10\n"
						"cmp QWORD [rbp-8], 0\n"
						"setne al\n"
						"mov BYTE [rbp-9], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "15_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    bool a = 10;\n"
			"    char b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 1\n"
						"movzx eax, BYTE [rbp-1]\n"
						"mov BYTE [rbp-2], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "16_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    short a = 10;\n"
			"    char b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov WORD [rbp-2], 10\n"
						"movzx eax, WORD [rbp-2]\n"
						"mov BYTE [rbp-3], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "17_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    long a = 10;\n"
			"    char b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov QWORD [rbp-8], 10\n"
						"mov rax, QWORD [rbp-8]\n"
						"mov BYTE [rbp-9], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "18_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    char a = 10;\n"
			"    char b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 10\n"
						"movzx eax, BYTE [rbp-1]\n"
						"mov BYTE [rbp-2], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "19_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    bool a = 10;\n"
			"    bool b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 1\n"
						"movzx eax, BYTE [rbp-1]\n"
						"mov BYTE [rbp-2], al\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "20_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    bool a = 10;\n"
			"    short b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 1\n"
						"movzx eax, BYTE [rbp-1]\n"
						"mov WORD [rbp-4], ax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "21_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    char a = 10;\n"
			"    short b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov BYTE [rbp-1], 10\n"
						"movzx ax, BYTE [rbp-1]\n"
						"mov WORD [rbp-4], ax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "22_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    short a = 10;\n"
			"    short b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov WORD [rbp-2], 10\n"
						"movzx eax, WORD [rbp-2]\n"
						"mov WORD [rbp-4], ax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "23_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    int a = 10;\n"
			"    short b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov DWORD [rbp-4], 10\n"
						"mov eax, DWORD [rbp-4]\n"
						"mov WORD [rbp-6], ax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "24_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    long a = 10;\n"
			"    short b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov QWORD [rbp-8], 10\n"
						"mov rax, QWORD [rbp-8]\n"
						"mov WORD [rbp-10], ax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "25_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    int a = 10;\n"
			"    int b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov DWORD [rbp-4], 10\n"
						"mov eax, DWORD [rbp-4]\n"
						"mov DWORD [rbp-8], eax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
	else if (arg == "26_binary_assignment")
	{
		auto code =
			"int main() {\n"
			"    long a = 10;\n"
			"    long b;\n"
			"    b = a;\n"
			"}";
		Compiler compiler{ code, "Addition", {}, df };
		compiler.Compile();
		auto generatedAsm = compiler.GetAsm();
		auto expected = "global _start\n"
						"section .bss\n"
						"section .data\n"
						"section .text\n"
						"\n"
						"_start:\n"
						"xor ebp, ebp\n"
						"call main\n"
						"mov rdi, rax\n"
						"mov rax, 60\n"
						"syscall\n"
						"\n"
						"main:\n"
						"push rbp\n"
						"mov rbp, rsp\n"
						"mov QWORD [rbp-8], 10\n"
						"mov rax, QWORD [rbp-8]\n"
						"mov QWORD [rbp-16], rax\n"
						"xor eax, eax\n"
						"pop rbp\n"
						"ret\n";

		return !(expected == generatedAsm);
	}
}
