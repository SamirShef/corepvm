#pragma once
#include "location.h"
#include "to_str.h"
#include <string_view>

enum TokenType {
    // literals
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
    TOK_BOOL,
    TOK_CHAR,
    TOK_SHORT,
    TOK_INT,
    TOK_LONG,
    TOK_FLOAT,
    TOK_DOUBLE,
    TOK_LET,
    TOK_FUN,
    TOK_RET,
    TOK_CONST,

    // operators
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_PRECENT,
    TOK_EQ,
    TOK_EQ_EQ,
    TOK_NOT_EQ,
    TOK_NOT,
    TOK_GT,
    TOK_GT_EQ,
    TOK_LS,
    TOK_LS_EQ,
    TOK_LAND,
    TOK_LOR,
    TOK_DOT,
    TOK_COMMA,
    TOK_SEMICOLON,
    TOK_COLON,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,
};

struct Token : ToStringable {
    TokenType type;
    std::string val;
    Location pos;

    Token(TokenType t, Location p) : type(t), val(""), pos(p) {}
    Token(TokenType t, const std::string& v, Location p) : type(t), val(v), pos(p) {}

    std::string to_str() const override {
        return std::to_string(type) + " : '" + val + "' (" + pos.to_str() + ")";
    }
};