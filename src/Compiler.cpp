/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-19.
//

#include "Compiler.h"

#include <chrono>
#include <fstream>
#include <utility>

namespace alx {

Compiler::Compiler(const std::string& code, const std::string& filename, Flags flags, const DebugFlags debug_flags)
  : m_flags(std::move(flags)),
	m_code(code),
	m_filename(filename),
	m_debug_flags(debug_flags)
{
	m_error_handler = std::make_shared<ErrorHandler>(code, filename, m_flags.werror);
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
		println(Colour::LightGreen, "Tokenised in {}ms", duration.count() * 1000);
	}

	const auto parseStart = SysClock::now();
	m_parser = std::make_unique<Parser>(tokens, m_error_handler);
	const auto& ast = m_parser->Parse();

	if (m_debug_flags.show_timing) {
		const Seconds duration = SysClock::now() - parseStart;
		println(Colour::LightGreen, "Built AST in {}ms", duration.count() * 1000);
	}

	const auto irStart = SysClock::now();
	m_intermediate_representation = std::make_unique<ir::IR>(ast->GetChildren());
	try {
		m_intermediate_representation->Generate();
	}
	catch (std::runtime_error& err) {
		if (!m_debug_flags.quiet_mode) {
			println(Colour::LightRed, "Something went wrong when generating IR: {;255;255;255}", err.what());
			if (m_debug_flags.dump_ast) {
				println(Colour::LightRed, "Current AST:");
				ast->PrintNode(0);
			}
		}
	}
	if (m_debug_flags.show_timing) {
		const Seconds duration = SysClock::now() - irStart;
		println(Colour::LightGreen, "Generated IR in {}ms", duration.count() * 1000);
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
				println(Colour::LightRed, "Something went wrong when generating assembly: {;255;255;255}", err.what());
				if (m_debug_flags.dump_ast) {
					println(Colour::LightRed, "Current AST:");
					ast->PrintNode(0);
				}
				if (m_debug_flags.dump_ir_initial) {
					println(Colour::LightRed, "\nIR:");
					m_intermediate_representation->Dump();
				}
				m_error_handler->EmitErrorCount();
			}
			exit(1);
		}
		const Seconds duration = SysClock::now() - generateStart;
		const Seconds totalDuration = SysClock::now() - start;
		if (m_debug_flags.show_timing)
			println(Colour::LightGreen, "Generated assembly in {}ms", duration.count() * 1000);

		if (!m_debug_flags.no_assemble) {
			try {
				Assemble();
			}
			catch (const std::runtime_error& err) {
				println(err.what());
			}
		}
		else {
			const FilePath& outputFilePath = m_flags.output_file;
			{
				std::ofstream out(getFormatted("{}", outputFilePath.GetFullPath()));
				out << ProgramGenerator::FormatAsm(m_generator->Asm());
				out.close(); // FIXME: is this necessary?
			}
		}


		println(Colour::LightGreen, "Total compilation time {}ms", totalDuration.count() * 1000);

#if OUTPUT_IR_TO_STRING
		m_intermediate_representation->Dump();
#endif
	}
	if (m_debug_flags.quiet_mode)
		return;

	m_error_handler->EmitErrorCount();

	if (m_debug_flags.dump_ast) {
		println();
		ast->PrintNode(0);
	}
	if (m_debug_flags.dump_ir_initial) {
		println();
		m_intermediate_representation->Dump();
	}
	if (m_debug_flags.dump_asm || m_debug_flags.dump_unformatted_asm) {
		println();
		if (m_debug_flags.dump_asm)
			println(ProgramGenerator::FormatAsm(m_generator->Asm()));
		else
			println(m_generator->Asm());
	}
}

void Compiler::Assemble()
{
	const FilePath& outputFilePath = m_flags.output_file;
	{
		std::ofstream out(getFormatted("/tmp/{}.s", outputFilePath.GetNameWithoutExtension()));
		out << m_generator->Asm();
		out.close(); // FIXME: ensure the file was written to and closed successfully
	}
	auto nasmStatus = system(getFormatted("nasm -f elf64 /tmp/{}.s -o /tmp/{}.o",
										  outputFilePath.GetNameWithoutExtension(),
										  outputFilePath.GetNameWithoutExtension())
								 .c_str());
	if (nasmStatus)
		throw std::runtime_error("nasm exited with status code: " + std::to_string(nasmStatus));
	auto ldStatus = system(
		getFormatted("ld /tmp/{}.o -o {}", outputFilePath.GetNameWithoutExtension(), outputFilePath.GetFullPath())
			.c_str());
	if (ldStatus)
		throw std::runtime_error("ld exited with status code: " + std::to_string(ldStatus));
}

std::string Compiler::GetAsm() { return m_generator->Asm(); }

const Program& Compiler::GetAst() { return m_parser->GetAst(); }

std::string Compiler::GetFormattedAsm() { return ProgramGenerator::FormatAsm(GetAsm()); }

#if OUTPUT_IR_TO_STRING
std::string Compiler::GetIr() { return m_intermediate_representation->GetIRString(); }
#endif

} // namespace alx