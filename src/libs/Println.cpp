/*
 * Copyright (c) 2023 Donatas Mockus.
 */

//
// Created by aelliixx on 2023-09-16.
//

#include "Error.h"
namespace alx {

size_t Error::ErrorCount{};
size_t Error::WarningCount{};
size_t Error::NoteCount{};

std::vector<std::string> Error::ErrorCodes = {
	"E000", "E001", "E002", "E003", "E004", "E005", "E006", "E007", "E008", "E009", "E010", "E011", "E012", "E013",
	"E014", "E015", "E016", "E017", "W500"
};

}
