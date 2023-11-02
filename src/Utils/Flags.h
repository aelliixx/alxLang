/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-17.
//

#pragma once

#include "../libs/argparse.hpp"

namespace alx {

#ifdef __clang__
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
#endif
struct DebugFlags
{
	bool show_timing{};
	bool dump_ast{};
	bool dump_asm{};
	bool dump_unformatted_asm{};
	bool quiet_mode{};
	bool dump_ir{};
};

struct Flags
{
	bool mno_red_zone;
};
#ifdef __clang__
#pragma clang diagnostic pop
#endif

inline DebugFlags resolveDebugFlags(const argparse::ArgumentParser& argParser)
{
	return {
		.show_timing = argParser.get<bool>("-t") && !argParser.get<bool>("-q"),
		.dump_ast = argParser.get<bool>("-d") && !argParser.get<bool>("-q"),
		.dump_asm = argParser.get<bool>("-a") && !argParser.get<bool>("-q"),
		.dump_unformatted_asm = argParser.get<bool>("--asm-no-format") && !argParser.get<bool>("-q"),
		.quiet_mode = argParser.get<bool>("-q"),
		.dump_ir = argParser.get<bool>("--dump-ir")
	};
}

inline Flags resolveFlags(const argparse::ArgumentParser& argParser)
{
	return {
		.mno_red_zone = argParser.get<bool>("-mno-red-zone")
	};
}

}
