#pragma once
#include "location.h"
#include <iostream>
#include <sstream>

void error(std::string_view file_name, std::string_view msg, Location loc) {
    std::stringstream ss;
    ss << "\033[31mCompilation error at: \033[0m" << file_name << ':' << loc.to_str() << ":\033[31m\n" << msg << "\033[0m\n";
    std::cerr << ss.str();
    exit(1);
}