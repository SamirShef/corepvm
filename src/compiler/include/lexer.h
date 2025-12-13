#pragma once
#include "token.h"
#include <unordered_map>
#include <cstdint>
#include <vector>

class Lexer {
    std::string src;
    std::string_view file_name;
    uint64_t pos;
    Location loc;
    static std::unordered_map<std::string_view, TokenType> spec_symbols;

public:
    Lexer(const std::string_view s, const std::string_view fn) : src(s), file_name(fn), pos(0), loc({1, 1}) {}

    std::vector<Token> tokenize();

private:
    Token tokenize_num();
    Token tokenize_str();
    Token tokenize_char();
    Token tokenize_id();
    Token tokenize_op();
    void skip_comments();
    const char peek(uint64_t rpos = 0) const;
    const char advance();
};