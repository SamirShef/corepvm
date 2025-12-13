#pragma once
#include "location.h"
#include <string_view>

enum TokenType {
    // literal
    TOK_BOOL_L,
    TOK_CHAR_L,
    TOK_SHORT_L,
    TOK_INT_L,
    TOK_LONG_L,
    TOK_FLOAT_L,
    TOK_DOUBLE_L,
    TOK_STR_L,
    TOK_ID,
    
    // keywords
    TOK_LET,
    TOK_FUN,
    TOK_RET,

    // operators
    TOK_PLUS,
    TOK_MIUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_PRECENT,
    TOK_DOT,
    TOK_COMMA,
    TOK_SEMICOLON,
    TOK_COLON,
};

struct Token {
    TokenType type;
    std::string val;
    Location pos;

    Token(TokenType t, Location p) : type(t), val(""), pos(p) {}
    Token(TokenType t, const std::string& v, Location p) : type(t), val(v), pos(p) {}

    std::string to_str() const {
        return std::to_string(type) + " : '" + val + "' (" + pos.to_str() + ")";
    }
};