/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-19.
//

#include <chrono>
#include "Compiler.h"

namespace alx {

Compiler::Compiler(const std::string& code,
				   const std::string& filename,
				   Flags flags,
				   DebugFlags debugFlags)
	: m_flags(flags),
	  m_code(code),
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

	if (m_debug_flags.show_timing)
	{
		const Seconds duration = SysClock::now() - start;
		alx::println(alx::Colour::LightGreen, "Tokenised in {}ms", duration.count() * 1000);
	}

	const auto parseStart = SysClock::now();
	m_parser = std::make_unique<Parser>(tokens, m_error_handler);
	const auto& ast = m_parser->Parse();
	exit(0);
	if (m_debug_flags.show_timing)
	{
		const Seconds duration = SysClock::now() - parseStart;
		alx::println(alx::Colour::LightGreen, "Built AST in {}ms", duration.count() * 1000);
	}
#if 0
	if (m_error_handler->ErrorCount() == 0)
	{
		const auto generateStart = SysClock::now();
		try
		{
			m_generator = std::make_unique<ProgramGenerator>(ast->GetChildren(), m_flags);
			m_generator->Generate();
		}
		catch (std::runtime_error&)
		{
			alx::println(alx::Colour::LightRed, "Something went wrong when generating assembly. AST:");
			ast->PrintNode(0);
			m_error_handler->EmitErrorCount();
			exit(1);
		}
		const Seconds duration = SysClock::now() - generateStart;
		const Seconds totalDuration = SysClock::now() - start;
		if (m_debug_flags.show_timing)
			alx::println(alx::Colour::LightGreen, "Generated assembly in {}ms", duration.count() * 1000);
		alx::println(alx::Colour::LightGreen, "Total compilation time {}ms", totalDuration.count() * 1000);
	}
	if (m_debug_flags.quiet_mode)
		return;
#endif
	m_error_handler->EmitErrorCount();

	if (m_debug_flags.dump_ast)
	{
		alx::println();
		ast->PrintNode(0);
	}
#if 0
	if (m_debug_flags.dump_asm || m_debug_flags.dump_unformatted_asm)
	{
		alx::println();
		if (m_debug_flags.dump_asm)
			alx::println(ProgramGenerator::FormatAsm(m_generator->Asm()));
		else if (m_debug_flags.dump_unformatted_asm)
			alx::println(m_generator->Asm());
	}
#endif
}
std::string Compiler::GetAsm()
{
//	return m_generator->Asm();
}
const Program& Compiler::GetAst()
{
	return m_parser->GetAst();
}
std::string Compiler::GetFormattedAsm()
{
	return ProgramGenerator::FormatAsm(GetAsm());
}

} // alx