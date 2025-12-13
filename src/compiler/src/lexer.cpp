#include "../include/exception.h"
#include "../include/lexer.h"
#include <cassert>
#include <cctype>

std::unordered_map<std::string_view, TokenType> Lexer::spec_symbols {
    {"true", TOK_BOOL_L},
    {"false", TOK_BOOL_L},
    {"let", TOK_LET},
    {"fun", TOK_FUN},
    {"return", TOK_RET},
    {"+", TOK_PLUS},
    {"-", TOK_MIUS},
    {"*", TOK_STAR},
    {"/", TOK_SLASH},
    {"%", TOK_PRECENT},
    {".", TOK_DOT},
    {",", TOK_COMMA},
    {";", TOK_SEMICOLON},
    {":", TOK_COLON},
};

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (pos < src.length()) {
        const char c = peek();
        if (c == '/' && (peek(1) == '/' || peek(1) == '*')) {
            skip_comments();
        }
        else if (std::isspace(c)) {
            advance();
        }
        else if (std::isdigit(c)) {
            tokens.push_back(tokenize_num());
        }
        else if (std::isalpha(c) || c == '_') {
            tokens.push_back(tokenize_id());
        }
        else if (c == '"') {
            tokens.push_back(tokenize_str());
        }
        else if (c == '\'') {
            tokens.push_back(tokenize_char());
        }
        else {
            tokens.push_back(tokenize_op());
        }
    }

    return tokens;
}

Token Lexer::tokenize_num() {
    Location loc = this->loc;
    std::string val;
    bool has_dot = false;
    while (pos < src.length() && (std::isdigit(peek()) || peek() == '.')) {
        if (peek() == '.') {
            if (has_dot) {
                error(file_name, "Invalid number literal: twice dot", loc);
            }
            else {
                has_dot = true;
            }
        }
        val += std::string{advance()};
    }
    #define TOK(type) Token(type, val, loc)
    const char suffix = pos < src.length() ? peek() : '\0';
    switch (tolower(suffix)) {
        case 'l':
            return TOK(TOK_LONG_L);
        case 'f':
            return TOK(TOK_FLOAT_L);
        case 's':
            return TOK(TOK_SHORT_L);
    }
    if (has_dot) {
        return TOK(TOK_DOUBLE_L);
    }
    return TOK(TOK_INT_L);
    #undef TOK
}

Token Lexer::tokenize_str() {
    Location loc = this->loc;
    std::string val;
    while (pos < src.length() && peek() != '"') {
        val += std::string{advance()};
    }
    if (pos == src.length() - 1) {
        error(file_name, "Invalid string literal: there is no closing double quotation mark", loc);
    }
    return Token(TOK_STR_L, val, loc);
}

Token Lexer::tokenize_char() {
    Location loc = this->loc;
    std::string val;
    while (pos < src.length() && peek() != '\'') {
        val += std::string{advance()};
    }
    if (pos == src.length() - 1) {
        error(file_name, "Invalid character literal: there is no closing single quotation mark", loc);
    }
    if (val.size() != 1) {
        error(file_name, "Invalid character literal: the length of the literal must be 1", loc);
    }
    return Token(TOK_CHAR_L, val, loc);
}

Token Lexer::tokenize_id() {
    Location loc = this->loc;
    std::string val;
    while (pos < src.length() && (std::isalpha(peek()) || std::isdigit(peek()) || peek() == '_')) {
        val += std::string{advance()};
    }
    auto it = spec_symbols.find(val);
    if (it != spec_symbols.end()) {
        if (it->second == TOK_BOOL_L) {
            return Token(it->second, val, loc);
        }
        return Token(it->second, loc);
    }
    return Token(TOK_ID, val, loc);
}

Token Lexer::tokenize_op() {
    Location loc = this->loc;
    auto it = spec_symbols.find(std::string{advance()});
    if (it != spec_symbols.end()) {
        return Token(it->second, loc);
    }
    error(file_name, "Unsupported operator: \033[0m'" + std::string{peek(-1)} + "'\033[31m", loc);
}

void Lexer::skip_comments() {
    advance();  // skip '/'
    if (advance() == '/') {
        while (advance() != '\n');
    }
    else {
        while (advance() != '*' || advance() != '/');
    }
}

const char Lexer::peek(uint64_t rpos) const {
    if (pos + rpos < src.length() && pos + rpos >= 0) {
        return src[pos + rpos];
    }
    error(file_name, "Index out of range", loc);
}

const char Lexer::advance() {
    const char c = peek();
    pos++;
    if (c == '\n') {
        loc.column = 1;
        loc.line++;
    }
    else {
        loc.column++;
    }
    return c;
}