/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-11-06.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
#pragma once

#include "../../../Ir.h"
#include "../InstructionSelector.h"


namespace alx::ir {

template<size_t N>
struct StringLiteral {
	constexpr StringLiteral(const char (&str)[N]) // NOLINT(*-explicit-constructor)
	{
		std::copy_n(str, N, Value);
	}

	char Value[N]{};
};

class Register
{
};

template<StringLiteral n, int16_t enc>
class X86Reg final : public Register
{
public:
	static constexpr const char* Name = n;
	[[maybe_unused]] static constexpr int16_t Encoding = enc;
	[[maybe_unused]] Register* LowRegister;
	[[maybe_unused]] Register* HighRegister;
	X86Reg() = default;
	[[maybe_unused]] X86Reg(Register* lowRegister, Register* highRegister)
	  : LowRegister(lowRegister),
		HighRegister(highRegister)
	{}
	[[maybe_unused]] explicit X86Reg(Register* lowRegister) : LowRegister(lowRegister), HighRegister(nullptr) {}
};

struct X86RegisterInfo {
	// 8-bit registers
	X86Reg<"al", 0b000> AL{};
	X86Reg<"cl", 0b001> CL{};
	X86Reg<"dl", 0b010> DL{};
	X86Reg<"bl", 0b011> BL{};
	// High registers. On x86-64, these cannot be used in any instruction with a REX prefix.
	X86Reg<"ah", 0b100> AH{};
	X86Reg<"ch", 0b101> CH{};
	X86Reg<"dh", 0b110> DH{};
	X86Reg<"bh", 0b111> BH{};
	// X86-64 only, requires REX prefix.
	X86Reg<"spl", 0b1000> SPL{};
	X86Reg<"bpl", 0b1001> BPL{};
	X86Reg<"sil", 0b1010> SIL{};
	X86Reg<"dil", 0b1011> DIL{};
	X86Reg<"r8b", 0b1011> R8B{};
	X86Reg<"r9b", 0b1011> R9B{};
	X86Reg<"r10b", 0b1011> R10B{};
	X86Reg<"r11b", 0b1011> R11B{};
	X86Reg<"r12b", 0b1011> R12B{};
	X86Reg<"r13b", 0b1011> R13B{};
	X86Reg<"r14b", 0b1011> R14B{};
	X86Reg<"r15b", 0b1011> R15B{};
	// High byte of the low 16 bits of the super-registers.
	X86Reg<"", -1> SIH{};
	X86Reg<"", -1> DIH{};
	X86Reg<"", -1> SPH{};
	X86Reg<"", -1> BPH{};
	X86Reg<"", -1> R8BH{};
	X86Reg<"", -1> R9BH{};
	X86Reg<"", -1> R10BH{};
	X86Reg<"", -1> R11BH{};
	X86Reg<"", -1> R12BH{};
	X86Reg<"", -1> R13BH{};
	X86Reg<"", -1> R14BH{};
	X86Reg<"", -1> R15BH{};
	// High word of the low 32 bits of the super-registers.
	X86Reg<"", -1> HAX{};
	X86Reg<"", -1> HCX{};
	X86Reg<"", -1> HDX{};
	X86Reg<"", -1> HBX{};
	X86Reg<"", -1> HSI{};
	X86Reg<"", -1> HDI{};
	X86Reg<"", -1> HSP{};
	X86Reg<"", -1> HBP{};
	X86Reg<"", -1> R8WH{};
	X86Reg<"", -1> R9WH{};
	X86Reg<"", -1> R10WH{};
	X86Reg<"", -1> R11WH{};
	X86Reg<"", -1> R12WH{};
	X86Reg<"", -1> R13WH{};
	X86Reg<"", -1> R14WH{};
	X86Reg<"", -1> R15WH{};

	// 16-bit registers
	X86Reg<"ax", 0b000> AX{ &AL, &AH };
	X86Reg<"cx", 0b001> CX{ &CL, &CH };
	X86Reg<"dx", 0b010> DX{ &DL, &DH };
	X86Reg<"bx", 0b011> BX{ &BL, &BH };
	X86Reg<"si", 0b100> SI{ &SIL, &SIH };
	X86Reg<"di", 0b101> DI{ &DIL, &DIH };
	X86Reg<"sp", 0b110> SP{ &SPL, &SPH };
	X86Reg<"bp", 0b111> BP{ &BPL, &BPH };
	// X86-64 only, requires REX prefix.
	X86Reg<"r8w", 0b1000> R8W{ &R8B, &R8BH };
	X86Reg<"r9w", 0b1001> R9W{ &R9B, &R9BH };
	X86Reg<"r10w", 0b1010> R10W{ &R10B, &R10BH };
	X86Reg<"r11w", 0b1011> R11W{ &R11B, &R11BH };
	X86Reg<"r12w", 0b1100> R12W{ &R12B, &R12BH };
	X86Reg<"r13w", 0b1101> R13W{ &R13B, &R13BH };
	X86Reg<"r14w", 0b1110> R14W{ &R14B, &R14BH };
	X86Reg<"r15w", 0b1111> R15W{ &R15B, &R15BH };

	// 32-bit registers
	X86Reg<"eax", 0b000> EAX{ &AX, &HAX };
	X86Reg<"ecx", 0b001> ECX{ &CX, &HCX };
	X86Reg<"edx", 0b010> EDX{ &DX, &HDX };
	X86Reg<"ebx", 0b011> EBX{ &BX, &HBX };
	X86Reg<"esi", 0b100> ESI{ &SI, &HSI };
	X86Reg<"edi", 0b101> EDI{ &DI, &HDI };
	X86Reg<"esp", 0b110> ESP{ &SP, &HSP };
	X86Reg<"ebp", 0b111> EBP{ &BP, &HBP };
	// X86-64 only, requires REX prefix.
	X86Reg<"r8d", 0b1000> R8D{ &R8W, &R8WH };
	X86Reg<"r9d", 0b1001> R9D{ &R9W, &R9WH };
	X86Reg<"r10d", 0b1010> R10D{ &R10W, &R10WH };
	X86Reg<"r11d", 0b1011> R11D{ &R11W, &R11WH };
	X86Reg<"r12d", 0b1100> R12D{ &R12W, &R12WH };
	X86Reg<"r13d", 0b1101> R13D{ &R13W, &R13WH };
	X86Reg<"r14d", 0b1110> R14D{ &R14W, &R14WH };
	X86Reg<"r15d", 0b1111> R15D{ &R15W, &R15WH };

	// 64-bit registers
	[[maybe_unused]] X86Reg<"rax", 0b000> RAX{ &EAX };
	[[maybe_unused]] X86Reg<"rcx", 0b001> RCX{ &ECX };
	[[maybe_unused]] X86Reg<"rdx", 0b010> RDX{ &EDX };
	[[maybe_unused]] X86Reg<"rbx", 0b011> RBX{ &EBX };
	[[maybe_unused]] X86Reg<"rsi", 0b100> RSI{ &ESI };
	[[maybe_unused]] X86Reg<"rdi", 0b101> RDI{ &EDI };
	[[maybe_unused]] X86Reg<"rsp", 0b110> RSP{ &ESP };
	[[maybe_unused]] X86Reg<"rbp", 0b111> RBP{ &EBP };
	// X86-64 only, requires REX prefix.
	[[maybe_unused]] X86Reg<"r8", 0b1000> R8{ &R8D };
	[[maybe_unused]] X86Reg<"r9", 0b1001> R9{ &R9D };
	[[maybe_unused]] X86Reg<"r10", 0b1010> R10{ &R10D };
	[[maybe_unused]] X86Reg<"r11", 0b1011> R11{ &R11D };
	[[maybe_unused]] X86Reg<"r12", 0b1100> R12{ &R12D };
	[[maybe_unused]] X86Reg<"r13", 0b1101> R13{ &R13D };
	[[maybe_unused]] X86Reg<"r14", 0b1110> R14{ &R14D };
	[[maybe_unused]] X86Reg<"r15", 0b1111> R15{ &R15D };

	// MMX registers
	[[maybe_unused]] X86Reg<"mm0", 0b000> MM0{};
	[[maybe_unused]] X86Reg<"mm1", 0b001> MM1{};
	[[maybe_unused]] X86Reg<"mm2", 0b010> MM2{};
	[[maybe_unused]] X86Reg<"mm3", 0b011> MM3{};
	[[maybe_unused]] X86Reg<"mm4", 0b100> MM4{};
	[[maybe_unused]] X86Reg<"mm5", 0b101> MM5{};
	[[maybe_unused]] X86Reg<"mm6", 0b110> MM6{};
	[[maybe_unused]] X86Reg<"mm7", 0b111> MM7{};

	// XMM registers
	[[maybe_unused]] X86Reg<"xmm0", 0b000> XMM0{};
	[[maybe_unused]] X86Reg<"xmm1", 0b001> XMM1{};
	[[maybe_unused]] X86Reg<"xmm2", 0b010> XMM2{};
	[[maybe_unused]] X86Reg<"xmm3", 0b011> XMM3{};
	[[maybe_unused]] X86Reg<"xmm4", 0b100> XMM4{};
	[[maybe_unused]] X86Reg<"xmm5", 0b101> XMM5{};
	[[maybe_unused]] X86Reg<"xmm6", 0b110> XMM6{};
	[[maybe_unused]] X86Reg<"xmm7", 0b111> XMM7{};
	// X86-64 only.
	[[maybe_unused]] X86Reg<"xmm8", 0b1000> XMM8{};
	[[maybe_unused]] X86Reg<"xmm9", 0b1001> XMM9{};
	[[maybe_unused]] X86Reg<"xmm10", 0b1010> XMM10{};
	[[maybe_unused]] X86Reg<"xmm11", 0b1011> XMM11{};
	[[maybe_unused]] X86Reg<"xmm12", 0b1100> XMM12{};
	[[maybe_unused]] X86Reg<"xmm13", 0b1101> XMM13{};
	[[maybe_unused]] X86Reg<"xmm14", 0b1110> XMM14{};
	[[maybe_unused]] X86Reg<"xmm15", 0b1111> XMM15{};
	[[maybe_unused]] X86Reg<"xmm16", 0b10000> XMM16{};
	[[maybe_unused]] X86Reg<"xmm17", 0b10001> XMM17{};
	[[maybe_unused]] X86Reg<"xmm18", 0b10010> XMM18{};
	[[maybe_unused]] X86Reg<"xmm19", 0b10011> XMM19{};
	[[maybe_unused]] X86Reg<"xmm20", 0b10100> XMM20{};
	[[maybe_unused]] X86Reg<"xmm21", 0b10101> XMM21{};
	[[maybe_unused]] X86Reg<"xmm22", 0b10110> XMM22{};
	[[maybe_unused]] X86Reg<"xmm23", 0b10111> XMM23{};
	[[maybe_unused]] X86Reg<"xmm24", 0b11000> XMM24{};
	[[maybe_unused]] X86Reg<"xmm25", 0b11001> XMM25{};
	[[maybe_unused]] X86Reg<"xmm26", 0b11010> XMM26{};
	[[maybe_unused]] X86Reg<"xmm27", 0b11011> XMM27{};
	[[maybe_unused]] X86Reg<"xmm28", 0b11100> XMM28{};
	[[maybe_unused]] X86Reg<"xmm29", 0b11101> XMM29{};
	[[maybe_unused]] X86Reg<"xmm30", 0b11110> XMM30{};
	[[maybe_unused]] X86Reg<"xmm31", 0b11111> XMM31{};

	// YMM registers
	// Used for AVX instructions.
	[[maybe_unused]] X86Reg<"ymm0", 0b000> YMM0{};
	[[maybe_unused]] X86Reg<"ymm1", 0b001> YMM1{};
	[[maybe_unused]] X86Reg<"ymm2", 0b010> YMM2{};
	[[maybe_unused]] X86Reg<"ymm3", 0b011> YMM3{};
	[[maybe_unused]] X86Reg<"ymm4", 0b100> YMM4{};
	[[maybe_unused]] X86Reg<"ymm5", 0b101> YMM5{};
	[[maybe_unused]] X86Reg<"ymm6", 0b110> YMM6{};
	[[maybe_unused]] X86Reg<"ymm7", 0b111> YMM7{};
	[[maybe_unused]] X86Reg<"ymm8", 0b1000> YMM8{};
	[[maybe_unused]] X86Reg<"ymm9", 0b1001> YMM9{};
	[[maybe_unused]] X86Reg<"ymm10", 0b1010> YMM10{};
	[[maybe_unused]] X86Reg<"ymm11", 0b1011> YMM11{};
	[[maybe_unused]] X86Reg<"ymm12", 0b1100> YMM12{};
	[[maybe_unused]] X86Reg<"ymm13", 0b1101> YMM13{};
	[[maybe_unused]] X86Reg<"ymm14", 0b1110> YMM14{};
	[[maybe_unused]] X86Reg<"ymm15", 0b1111> YMM15{};
	// Used for AVX-512 instructions.
	[[maybe_unused]] X86Reg<"ymm16", 0b10000> YMM16{};
	[[maybe_unused]] X86Reg<"ymm17", 0b10001> YMM17{};
	[[maybe_unused]] X86Reg<"ymm18", 0b10010> YMM18{};
	[[maybe_unused]] X86Reg<"ymm19", 0b10011> YMM19{};
	[[maybe_unused]] X86Reg<"ymm20", 0b10100> YMM20{};
	[[maybe_unused]] X86Reg<"ymm21", 0b10101> YMM21{};
	[[maybe_unused]] X86Reg<"ymm22", 0b10110> YMM22{};
	[[maybe_unused]] X86Reg<"ymm23", 0b10111> YMM23{};
	[[maybe_unused]] X86Reg<"ymm24", 0b11000> YMM24{};
	[[maybe_unused]] X86Reg<"ymm25", 0b11001> YMM25{};
	[[maybe_unused]] X86Reg<"ymm26", 0b11010> YMM26{};
	[[maybe_unused]] X86Reg<"ymm27", 0b11011> YMM27{};
	[[maybe_unused]] X86Reg<"ymm28", 0b11100> YMM28{};
	[[maybe_unused]] X86Reg<"ymm29", 0b11101> YMM29{};
	[[maybe_unused]] X86Reg<"ymm30", 0b11110> YMM30{};
	[[maybe_unused]] X86Reg<"ymm31", 0b11111> YMM31{};
};

struct [[maybe_unused]] X86InstructionInfo : public InstructionInfo {
	void GetInstruction(std::variant<BodyTypes, IdentifierInstruction> instruction) override
	{
		struct InstructionVisitor {
			struct IdentifierInstructionsVisitor {
				void operator()(const AllocaInst&) {}
				void operator()(const LoadInst&) {}
				void operator()(const AddInst&) {}
				void operator()(const SubInst&) {}
				void operator()(const MulInst&) {}
				void operator()(const SDivInst&) {}
				void operator()(const ICmpInst&) {}
			} identVisitor;
			struct BodyTypesVisitor {
				void operator()(const LabelType&) {}
				void operator()(const ReturnInst&) {}
				void operator()(const Variable&) {}
				void operator()(const StoreInst&) {}
				void operator()(const BranchInst&) {}
			} bodyVisitor;
			void operator()(const IdentifierInstruction& instruction) { std::visit(identVisitor, instruction); }
			void operator()(const BodyTypes& instruction) { std::visit(bodyVisitor, instruction); }
		} visitor{};
		std::visit(visitor, instruction);
	}
};

struct [[maybe_unused]] X86TargetInfo {
	[[maybe_unused]] X86RegisterInfo Registers;
};

}
#pragma clang diagnostic pop