/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-19.
//

#include "Compiler.h"

#include <chrono>
#include <fstream>

namespace alx {

Compiler::Compiler(const std::string& code, const std::string& filename, Flags flags, DebugFlags debugFlags)
  : m_flags(flags),
	m_code(code),
	m_filename(filename),
	m_debug_flags(debugFlags)
{
	m_error_handler = std::make_shared<ErrorHandler>(code, filename);
	if (code.empty())
		exit(1);
}
void Compiler::Compile()
{
	using SysClock = std::chrono::system_clock;
	using Seconds = std::chrono::duration<double>;
	const auto start = SysClock::now();

	m_tokeniser = std::make_unique<Tokeniser>(m_code, m_error_handler);
	auto tokens = m_tokeniser->Tokenise();

	if (m_debug_flags.show_timing) {
		const Seconds duration = SysClock::now() - start;
		alx::println(alx::Colour::LightGreen, "Tokenised in {}ms", duration.count() * 1000);
	}

	const auto parseStart = SysClock::now();
	m_parser = std::make_unique<Parser>(tokens, m_error_handler);
	const auto& ast = m_parser->Parse();

	if (m_debug_flags.show_timing) {
		const Seconds duration = SysClock::now() - parseStart;
		alx::println(alx::Colour::LightGreen, "Built AST in {}ms", duration.count() * 1000);
	}

	const auto irStart = SysClock::now();
	m_intermediate_representation = std::make_unique<ir::IR>(ast->GetChildren());
	try {
		m_intermediate_representation->Generate();
	}
	catch (std::runtime_error& err) {
		if (!m_debug_flags.quiet_mode) {
			alx::println(alx::Colour::LightRed, "Something went wrong when generating IR: {;255;255;255}", err.what());
			if (m_debug_flags.dump_ast) {
				alx::println(alx::Colour::LightRed, "Current AST:");
				ast->PrintNode(0);
			}
		}
	}

	if (m_debug_flags.show_timing) {
		const Seconds duration = SysClock::now() - irStart;
		alx::println(alx::Colour::LightGreen, "Generated IR in {}ms", duration.count() * 1000);
	}

	if (m_error_handler->ErrorCount() == 0) {
		const auto generateStart = SysClock::now();
		try {
			m_generator = std::make_unique<ProgramGenerator>(ast->GetChildren(), m_flags);
			m_generator->Generate();
		}
		catch (std::runtime_error& err) {
#if OUTPUT_IR_TO_STRING
			m_intermediate_representation->Dump();
			return;
#endif
			if (!m_debug_flags.quiet_mode) {
				alx::println(
					alx::Colour::LightRed, "Something went wrong when generating assembly: {;255;255;255}", err.what());
				if (m_debug_flags.dump_ast) {
					alx::println(alx::Colour::LightRed, "Current AST:");
					ast->PrintNode(0);
				}
				if (m_debug_flags.dump_ir) {
					alx::println(alx::Colour::LightRed, "\nIR:");
					m_intermediate_representation->Dump();
				}
				m_error_handler->EmitErrorCount();
			}
			exit(1);
		}
		const Seconds duration = SysClock::now() - generateStart;
		const Seconds totalDuration = SysClock::now() - start;
		if (m_debug_flags.show_timing)
			alx::println(alx::Colour::LightGreen, "Generated assembly in {}ms", duration.count() * 1000);
		alx::println(alx::Colour::LightGreen, "Total compilation time {}ms", totalDuration.count() * 1000);

#if OUTPUT_IR_TO_STRING
		m_intermediate_representation->Dump();
#endif
	}
	if (m_debug_flags.quiet_mode)
		return;

	m_error_handler->EmitErrorCount();

	if (m_debug_flags.dump_ast) {
		alx::println();
		ast->PrintNode(0);
	}
	if (m_debug_flags.dump_ir) {
		alx::println();
		m_intermediate_representation->Dump();
	}
	if (m_debug_flags.dump_asm || m_debug_flags.dump_unformatted_asm) {
		alx::println();
		if (m_debug_flags.dump_asm)
			alx::println(ProgramGenerator::FormatAsm(m_generator->Asm()));
		else if (m_debug_flags.dump_unformatted_asm)
			alx::println(m_generator->Asm());
	}
}

void Compiler::Assemble() {
	std::ofstream out(getFormatted("/tmp/temp_alx.s"));
	out << m_generator->Asm();
	out.close();
	[[maybe_unused]] auto nasmStatus = system("nasm -f elf64 /tmp/temp_alx.s -o /tmp/temp_alx.o");
	[[maybe_unused]] auto ldStatus = system("ld /tmp/temp_alx.o -o ./temp_alx");
}

std::string Compiler::GetAsm() { return m_generator->Asm(); }

const Program& Compiler::GetAst() { return m_parser->GetAst(); }

std::string Compiler::GetFormattedAsm() { return ProgramGenerator::FormatAsm(GetAsm()); }

#if OUTPUT_IR_TO_STRING
std::string Compiler::GetIr() { return m_intermediate_representation->GetIRString(); }
#endif

} // namespace alx