/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-11-06.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
#pragma once


#include "../../../IR/Ir.h"
#include "../InstructionSelection.h"


namespace alx::ir {

struct X86Reg final {
	const std::string Name;
	const int16_t Encoding;
	const X86Reg* LowRegister = nullptr;
	const X86Reg* HighRegister = nullptr;
};

struct X86RegisterInfo {
	// 8-bit registers
	static constexpr X86Reg AL{ "al", 0b000 };
	static constexpr X86Reg CL{ "cl", 0b001 };
	static constexpr X86Reg DL{ "dl", 0b010 };
	static constexpr X86Reg BL{ "bl", 0b011 };
	// High registers. On x86-64, these cannot be used in any instruction with a REX prefix.
	static constexpr X86Reg AH{ "ah", 0b100 };
	static constexpr X86Reg CH{ "ch", 0b101 };
	static constexpr X86Reg DH{ "dh", 0b110 };
	static constexpr X86Reg BH{ "bh", 0b111 };
	// X86-64 only, requires REX prefix.
	static constexpr X86Reg SPL{ "spl", 0b1000 };
	static constexpr X86Reg BPL{ "bpl", 0b1001 };
	static constexpr X86Reg SIL{ "sil", 0b1010 };
	static constexpr X86Reg DIL{ "dil", 0b1011 };
	static constexpr X86Reg R8B{ "r8b", 0b1011 };
	static constexpr X86Reg R9B{ "r9b", 0b1011 };
	static constexpr X86Reg R10B{ "r10b", 0b1011 };
	static constexpr X86Reg R11B{ "r11b", 0b1011 };
	static constexpr X86Reg R12B{ "r12b", 0b1011 };
	static constexpr X86Reg R13B{ "r13b", 0b1011 };
	static constexpr X86Reg R14B{ "r14b", 0b1011 };
	static constexpr X86Reg R15B{ "r15b", 0b1011 };
	// High byte of the low 16 bits of the super-registers.
	static constexpr X86Reg SIH{ "", -1 };
	static constexpr X86Reg DIH{ "", -1 };
	static constexpr X86Reg SPH{ "", -1 };
	static constexpr X86Reg BPH{ "", -1 };
	static constexpr X86Reg R8BH{ "", -1 };
	static constexpr X86Reg R9BH{ "", -1 };
	static constexpr X86Reg R10BH{ "", -1 };
	static constexpr X86Reg R11BH{ "", -1 };
	static constexpr X86Reg R12BH{ "", -1 };
	static constexpr X86Reg R13BH{ "", -1 };
	static constexpr X86Reg R14BH{ "", -1 };
	static constexpr X86Reg R15BH{ "", -1 };
	// High word of the low 32 bits of the super-registers.
	static constexpr X86Reg HAX{ "", -1 };
	static constexpr X86Reg HCX{ "", -1 };
	static constexpr X86Reg HDX{ "", -1 };
	static constexpr X86Reg HBX{ "", -1 };
	static constexpr X86Reg HSI{ "", -1 };
	static constexpr X86Reg HDI{ "", -1 };
	static constexpr X86Reg HSP{ "", -1 };
	static constexpr X86Reg HBP{ "", -1 };
	static constexpr X86Reg R8WH{ "", -1 };
	static constexpr X86Reg R9WH{ "", -1 };
	static constexpr X86Reg R10WH{ "", -1 };
	static constexpr X86Reg R11WH{ "", -1 };
	static constexpr X86Reg R12WH{ "", -1 };
	static constexpr X86Reg R13WH{ "", -1 };
	static constexpr X86Reg R14WH{ "", -1 };
	static constexpr X86Reg R15WH{ "", -1 };

	// 16-bit registers
	static constexpr X86Reg AX{ "ax", 0b000, &AL, &AH };
	static constexpr X86Reg CX{ "cx", 0b001, &CL, &CH };
	static constexpr X86Reg DX{ "dx", 0b010, &DL, &DH };
	static constexpr X86Reg BX{ "bx", 0b011, &BL, &BH };
	static constexpr X86Reg SI{ "si", 0b100, &SIL, &SIH };
	static constexpr X86Reg DI{ "di", 0b101, &DIL, &DIH };
	static constexpr X86Reg SP{ "sp", 0b110, &SPL, &SPH };
	static constexpr X86Reg BP{ "bp", 0b111, &BPL, &BPH };
	// X86-64 only, requires REX prefix.
	static constexpr X86Reg R8W{ "r8w", 0b1000, &R8B, &R8BH };
	static constexpr X86Reg R9W{ "r9w", 0b1001, &R9B, &R9BH };
	static constexpr X86Reg R10W{ "r10w", 0b1010, &R10B, &R10BH };
	static constexpr X86Reg R11W{ "r11w", 0b1011, &R11B, &R11BH };
	static constexpr X86Reg R12W{ "r12w", 0b1100, &R12B, &R12BH };
	static constexpr X86Reg R13W{ "r13w", 0b1101, &R13B, &R13BH };
	static constexpr X86Reg R14W{ "r14w", 0b1110, &R14B, &R14BH };
	static constexpr X86Reg R15W{ "r15w", 0b1111, &R15B, &R15BH };

	// 32-bit registers
	static constexpr X86Reg EAX{ "eax", 0b000, &AX, &HAX };
	static constexpr X86Reg ECX{ "ecx", 0b001, &CX, &HCX };
	static constexpr X86Reg EDX{ "edx", 0b010, &DX, &HDX };
	static constexpr X86Reg EBX{ "ebx", 0b011, &BX, &HBX };
	static constexpr X86Reg ESI{ "esi", 0b100, &SI, &HSI };
	static constexpr X86Reg EDI{ "edi", 0b101, &DI, &HDI };
	static constexpr X86Reg ESP{ "esp", 0b110, &SP, &HSP };
	static constexpr X86Reg EBP{ "ebp", 0b111, &BP, &HBP };
	// X86-64 only, requires REX prefix.
	static constexpr X86Reg R8D{ "r8d", 0b1000, &R8W, &R8WH };
	static constexpr X86Reg R9D{ "r9d", 0b1001, &R9W, &R9WH };
	static constexpr X86Reg R10D{ "r10d", 0b1010, &R10W, &R10WH };
	static constexpr X86Reg R11D{ "r11d", 0b1011, &R11W, &R11WH };
	static constexpr X86Reg R12D{ "r12d", 0b1100, &R12W, &R12WH };
	static constexpr X86Reg R13D{ "r13d", 0b1101, &R13W, &R13WH };
	static constexpr X86Reg R14D{ "r14d", 0b1110, &R14W, &R14WH };
	static constexpr X86Reg R15D{ "r15d", 0b1111, &R15W, &R15WH };

	// 64-bit registers
	[[maybe_unused]] static constexpr X86Reg RAX{ "rax", 0b000, &EAX };
	[[maybe_unused]] static constexpr X86Reg RCX{ "rcx", 0b001, &ECX };
	[[maybe_unused]] static constexpr X86Reg RDX{ "rdx", 0b010, &EDX };
	[[maybe_unused]] static constexpr X86Reg RBX{ "rbx", 0b011, &EBX };
	[[maybe_unused]] static constexpr X86Reg RSI{ "rsi", 0b100, &ESI };
	[[maybe_unused]] static constexpr X86Reg RDI{ "rdi", 0b101, &EDI };
	[[maybe_unused]] static constexpr X86Reg RSP{ "rsp", 0b110, &ESP };
	[[maybe_unused]] static constexpr X86Reg RBP{ "rbp", 0b111, &EBP };
	// X86-64 only, requires REX prefix.
	[[maybe_unused]] static constexpr X86Reg R8{ "r8", 0b1000, &R8D };
	[[maybe_unused]] static constexpr X86Reg R9{ "r9", 0b1001, &R9D };
	[[maybe_unused]] static constexpr X86Reg R10{ "r10", 0b1010, &R10D };
	[[maybe_unused]] static constexpr X86Reg R11{ "r11", 0b1011, &R11D };
	[[maybe_unused]] static constexpr X86Reg R12{ "r12", 0b1100, &R12D };
	[[maybe_unused]] static constexpr X86Reg R13{ "r13", 0b1101, &R13D };
	[[maybe_unused]] static constexpr X86Reg R14{ "r14", 0b1110, &R14D };
	[[maybe_unused]] static constexpr X86Reg R15{ "r15", 0b1111, &R15D };

	// MMX registers
	[[maybe_unused]] static constexpr X86Reg MM0{ "mm0", 0b000 };
	[[maybe_unused]] static constexpr X86Reg MM1{ "mm1", 0b001 };
	[[maybe_unused]] static constexpr X86Reg MM2{ "mm2", 0b010 };
	[[maybe_unused]] static constexpr X86Reg MM3{ "mm3", 0b011 };
	[[maybe_unused]] static constexpr X86Reg MM4{ "mm4", 0b100 };
	[[maybe_unused]] static constexpr X86Reg MM5{ "mm5", 0b101 };
	[[maybe_unused]] static constexpr X86Reg MM6{ "mm6", 0b110 };
	[[maybe_unused]] static constexpr X86Reg MM7{ "mm7", 0b111 };

	// XMM registers
	[[maybe_unused]] static constexpr X86Reg XMM0{ "xmm0", 0b000 };
	[[maybe_unused]] static constexpr X86Reg XMM1{ "xmm1", 0b001 };
	[[maybe_unused]] static constexpr X86Reg XMM2{ "xmm2", 0b010 };
	[[maybe_unused]] static constexpr X86Reg XMM3{ "xmm3", 0b011 };
	[[maybe_unused]] static constexpr X86Reg XMM4{ "xmm4", 0b100 };
	[[maybe_unused]] static constexpr X86Reg XMM5{ "xmm5", 0b101 };
	[[maybe_unused]] static constexpr X86Reg XMM6{ "xmm6", 0b110 };
	[[maybe_unused]] static constexpr X86Reg XMM7{ "xmm7", 0b111 };
	// X86-64 only.
	[[maybe_unused]] static constexpr X86Reg XMM8{ "xmm8", 0b1000 };
	[[maybe_unused]] static constexpr X86Reg XMM9{ "xmm9", 0b1001 };
	[[maybe_unused]] static constexpr X86Reg XMM10{ "xmm10", 0b1010 };
	[[maybe_unused]] static constexpr X86Reg XMM11{ "xmm11", 0b1011 };
	[[maybe_unused]] static constexpr X86Reg XMM12{ "xmm12", 0b1100 };
	[[maybe_unused]] static constexpr X86Reg XMM13{ "xmm13", 0b1101 };
	[[maybe_unused]] static constexpr X86Reg XMM14{ "xmm14", 0b1110 };
	[[maybe_unused]] static constexpr X86Reg XMM15{ "xmm15", 0b1111 };
	[[maybe_unused]] static constexpr X86Reg XMM16{ "xmm16", 0b10000 };
	[[maybe_unused]] static constexpr X86Reg XMM17{ "xmm17", 0b10001 };
	[[maybe_unused]] static constexpr X86Reg XMM18{ "xmm18", 0b10010 };
	[[maybe_unused]] static constexpr X86Reg XMM19{ "xmm19", 0b10011 };
	[[maybe_unused]] static constexpr X86Reg XMM20{ "xmm20", 0b10100 };
	[[maybe_unused]] static constexpr X86Reg XMM21{ "xmm21", 0b10101 };
	[[maybe_unused]] static constexpr X86Reg XMM22{ "xmm22", 0b10110 };
	[[maybe_unused]] static constexpr X86Reg XMM23{ "xmm23", 0b10111 };
	[[maybe_unused]] static constexpr X86Reg XMM24{ "xmm24", 0b11000 };
	[[maybe_unused]] static constexpr X86Reg XMM25{ "xmm25", 0b11001 };
	[[maybe_unused]] static constexpr X86Reg XMM26{ "xmm26", 0b11010 };
	[[maybe_unused]] static constexpr X86Reg XMM27{ "xmm27", 0b11011 };
	[[maybe_unused]] static constexpr X86Reg XMM28{ "xmm28", 0b11100 };
	[[maybe_unused]] static constexpr X86Reg XMM29{ "xmm29", 0b11101 };
	[[maybe_unused]] static constexpr X86Reg XMM30{ "xmm30", 0b11110 };
	[[maybe_unused]] static constexpr X86Reg XMM31{ "xmm31", 0b11111 };

	// YMM registers
	// Used for AVX instructions.
	[[maybe_unused]] static constexpr X86Reg YMM0{ "ymm0", 0b000 };
	[[maybe_unused]] static constexpr X86Reg YMM1{ "ymm1", 0b001 };
	[[maybe_unused]] static constexpr X86Reg YMM2{ "ymm2", 0b010 };
	[[maybe_unused]] static constexpr X86Reg YMM3{ "ymm3", 0b011 };
	[[maybe_unused]] static constexpr X86Reg YMM4{ "ymm4", 0b100 };
	[[maybe_unused]] static constexpr X86Reg YMM5{ "ymm5", 0b101 };
	[[maybe_unused]] static constexpr X86Reg YMM6{ "ymm6", 0b110 };
	[[maybe_unused]] static constexpr X86Reg YMM7{ "ymm7", 0b111 };
	[[maybe_unused]] static constexpr X86Reg YMM8{ "ymm8", 0b1000 };
	[[maybe_unused]] static constexpr X86Reg YMM9{ "ymm9", 0b1001 };
	[[maybe_unused]] static constexpr X86Reg YMM10{ "ymm10", 0b1010 };
	[[maybe_unused]] static constexpr X86Reg YMM11{ "ymm11", 0b1011 };
	[[maybe_unused]] static constexpr X86Reg YMM12{ "ymm12", 0b1100 };
	[[maybe_unused]] static constexpr X86Reg YMM13{ "ymm13", 0b1101 };
	[[maybe_unused]] static constexpr X86Reg YMM14{ "ymm14", 0b1110 };
	[[maybe_unused]] static constexpr X86Reg YMM15{ "ymm15", 0b1111 };
	// Used for AVX-512 instructions.
	[[maybe_unused]] static constexpr X86Reg YMM16{ "ymm16", 0b10000 };
	[[maybe_unused]] static constexpr X86Reg YMM17{ "ymm17", 0b10001 };
	[[maybe_unused]] static constexpr X86Reg YMM18{ "ymm18", 0b10010 };
	[[maybe_unused]] static constexpr X86Reg YMM19{ "ymm19", 0b10011 };
	[[maybe_unused]] static constexpr X86Reg YMM20{ "ymm20", 0b10100 };
	[[maybe_unused]] static constexpr X86Reg YMM21{ "ymm21", 0b10101 };
	[[maybe_unused]] static constexpr X86Reg YMM22{ "ymm22", 0b10110 };
	[[maybe_unused]] static constexpr X86Reg YMM23{ "ymm23", 0b10111 };
	[[maybe_unused]] static constexpr X86Reg YMM24{ "ymm24", 0b11000 };
	[[maybe_unused]] static constexpr X86Reg YMM25{ "ymm25", 0b11001 };
	[[maybe_unused]] static constexpr X86Reg YMM26{ "ymm26", 0b11010 };
	[[maybe_unused]] static constexpr X86Reg YMM27{ "ymm27", 0b11011 };
	[[maybe_unused]] static constexpr X86Reg YMM28{ "ymm28", 0b11100 };
	[[maybe_unused]] static constexpr X86Reg YMM29{ "ymm29", 0b11101 };
	[[maybe_unused]] static constexpr X86Reg YMM30{ "ymm30", 0b11110 };
	[[maybe_unused]] static constexpr X86Reg YMM31{ "ymm31", 0b11111 };
};

struct [[maybe_unused]] X86InstructionInfo final : public InstructionInfo {
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
};

}
#pragma clang diagnostic pop