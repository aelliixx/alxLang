/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-13.
//

#pragma once
#include <string>

namespace alx {


/// Returns true if the string to Match matches a passed in pattern
/// @param to_match string to Match
/// @param matcher pattern to match with. Syntax: \
					a -> alpha character \
					d -> numeric character \
					a* -> match 0 or more of a \
					
/// @return true if to_match matches the matcher pattern
bool Match(const std::string& to_match, const std::string& matcher) {
	
	for (const char c : matcher) {
		
	}
	
}

}
