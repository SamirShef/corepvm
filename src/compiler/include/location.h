#pragma once
#include <cstdint>
#include <string>

struct Location {
    uint64_t line, column;

    std::string to_str() const {
        return std::to_string(line) + ":" + std::to_string(column);
    }
};