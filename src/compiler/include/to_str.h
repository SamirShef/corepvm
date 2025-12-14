#pragma once
#include <string>

struct ToStringable {
    virtual std::string to_str() const = 0;
};