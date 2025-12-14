#pragma once
#include "to_str.h"
#include <cstdint>
#include <string>

struct Location : ToStringable {
    uint64_t line, column;

    Location(uint64_t l, uint64_t c) : line(l), column(c) {}

    std::string to_str() const override {
        return std::to_string(line) + ":" + std::to_string(column);
    }
};