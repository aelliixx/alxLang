/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-17.
//

#pragma once

#include "../libs/argparse.hpp"

namespace alx {

struct DebugFlags {
	bool show_timing{};
	bool dump_ast{};
	bool dump_asm{};
	bool dump_unformatted_asm{};
	bool quiet_mode{};
	bool dump_ir{};
	bool no_assemble{};
};

struct Flags {
	bool mno_red_zone;
	bool fdiagnostics_colour;
	bool werror;
};

inline DebugFlags resolveDebugFlags(const argparse::ArgumentParser& argParser)
{
	return { .show_timing = argParser.get<bool>("-t") && !argParser.get<bool>("-q"),
			 .dump_ast = argParser.get<bool>("-d") && !argParser.get<bool>("-q"),
			 .dump_asm = argParser.get<bool>("-a") && !argParser.get<bool>("-q"),
			 .dump_unformatted_asm = argParser.get<bool>("--asm-no-format") && !argParser.get<bool>("-q"),
			 .quiet_mode = argParser.get<bool>("-q"),
			 .dump_ir = argParser.get<bool>("--dump-ir"),
			 .no_assemble = argParser.get<bool>("-S") };
}

inline Flags resolveFlags(const argparse::ArgumentParser& argParser)
{
	return { .mno_red_zone = argParser.get<bool>("-mno-red-zone"),
			 .fdiagnostics_colour = argParser.get<bool>("-fdiagnostics-colour"),
			 .werror = argParser.get<bool>("-Werror") };
}

}
