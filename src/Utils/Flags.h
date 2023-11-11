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
};

struct Flags {
	FilePath output_file{};
	bool mno_red_zone{};
	bool fdiagnostics_colour{};
	bool werror{};
};

inline DebugFlags resolveDebugFlags(const argparse::ArgumentParser& argParser)
{
	auto irPipeline = argParser.get<std::string>("--dump-ir");
	std::vector<std::string> irPipelineFlags;
	if (!irPipeline.empty()) {
		// Split the string by commas
		std::stringstream ss(irPipeline);
		std::string flag;
		while (std::getline(ss, flag, ',')) {
			irPipelineFlags.push_back(flag);
		}
	}
	auto findFlagString = [&irPipelineFlags](const std::string& flag) {
		return std::find_if(irPipelineFlags.begin(), irPipelineFlags.end(), [&flag](const std::string& str) {
			return str == flag;
		}) != irPipelineFlags.end();
	};

	return {
		.show_timing = argParser.get<bool>("-t") && !argParser.get<bool>("-q"),
		.dump_ast = argParser.get<bool>("-d") && !argParser.get<bool>("-q"),
		.dump_asm = argParser.get<bool>("-a") && !argParser.get<bool>("-q"),
		.dump_unformatted_asm = argParser.get<bool>("--asm-no-format") && !argParser.get<bool>("-q"),
		.quiet_mode = argParser.get<bool>("-q"),
		.no_assemble = argParser.get<bool>("-S"),
		.dump_ir_all = findFlagString("all"),
		.dump_ir_initial = findFlagString("initial") || findFlagString("all"),
		.dump_ir_isel = findFlagString("isel") || findFlagString("all"),
	};
}

inline Flags resolveFlags(const argparse::ArgumentParser& argParser)
{
	FilePath outputFilePath(argParser.get<std::string>("-o"));
	return { .output_file = outputFilePath,
			 .mno_red_zone = argParser.get<bool>("-mno-red-zone"),
			 .fdiagnostics_colour = argParser.get<bool>("-fdiagnostics-colour"),
			 .werror = argParser.get<bool>("-Werror") };
}

}
