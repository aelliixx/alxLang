/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-17.
//

#pragma once

#include "../libs/argparse.hpp"
#include "File.h"

namespace alx {

struct DebugFlags {
	bool show_timing{};
	bool dump_ast{};
	bool dump_asm{};
	bool dump_unformatted_asm{};
	bool quiet_mode{};
	bool no_assemble{};
	bool dump_ir_all{};
	bool dump_ir_initial{};
	bool dump_ir_isel{};
	FilePath output_file{};
};

struct Flags {
	bool mno_red_zone;
	bool fdiagnostics_colour;
	bool werror;
};

inline DebugFlags resolveDebugFlags(const argparse::ArgumentParser& argParser)
{
	auto irPipeline = argParser.get<std::vector<std::string>>("--dump-ir");
	auto findFlagString = [&irPipeline](const std::string& flag) {
		return std::find_if(irPipeline.begin(), irPipeline.end(), [&flag](const std::string& str) {
			return str == flag;
		}) != irPipeline.end();
	};

	FilePath outputFilePath(argParser.get<std::string>("-o"));

	return { .show_timing = argParser.get<bool>("-t") && !argParser.get<bool>("-q"),
			 .dump_ast = argParser.get<bool>("-d") && !argParser.get<bool>("-q"),
			 .dump_asm = argParser.get<bool>("-a") && !argParser.get<bool>("-q"),
			 .dump_unformatted_asm = argParser.get<bool>("--asm-no-format") && !argParser.get<bool>("-q"),
			 .quiet_mode = argParser.get<bool>("-q"),
			 .no_assemble = argParser.get<bool>("-S"),
			 .dump_ir_all = findFlagString("all"),
			 .dump_ir_initial = findFlagString("initial") || findFlagString("all"),
			 .dump_ir_isel = findFlagString("isel") || findFlagString("all"),
			 .output_file = outputFilePath };
}

inline Flags resolveFlags(const argparse::ArgumentParser& argParser)
{
	return { .mno_red_zone = argParser.get<bool>("-mno-red-zone"),
			 .fdiagnostics_colour = argParser.get<bool>("-fdiagnostics-colour"),
			 .werror = argParser.get<bool>("-Werror") };
}

}
