/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-17.
//

#pragma once

#include "../libs/argparse.hpp"

namespace alx {

struct Flags {
	bool mno_red_zone;
};

static Flags ResolveFlags(const argparse::ArgumentParser& argParser) {
	return {
		.mno_red_zone = argParser.get<bool>("-mno-red-zone")
	};
}


}
