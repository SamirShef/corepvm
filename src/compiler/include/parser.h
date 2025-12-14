#pragma once
#include "location.h"
#include "token.h"
#include "ast.h"
#include <vector>

class Parser {
    std::string_view file_name;
    std::vector<Token> tokens;
    uint32_t pos;

public:
    Parser(std::string_view fn, std::vector<Token> t) : file_name(fn), tokens(std::move(t)), pos(0) {}

    std::vector<ASTNodePtr> parse();

private:
    ASTNodePtr parse_stmt();
    ASTNodePtr parse_vds_stmt();

    ASTNodePtr parse_expr();
    ASTNodePtr parse_land_expr();
    ASTNodePtr parse_lor_expr();
    ASTNodePtr parse_eq_expr();
    ASTNodePtr parse_comp_expr();
    ASTNodePtr parse_additive_expr();
    ASTNodePtr parse_multiplicative_expr();
    ASTNodePtr parse_unary_expr();
    ASTNodePtr parse_primary_expr();

    const Token peek(uint32_t rpos = 0) const;
    bool match(TokenType type);
    Token consume(TokenType type, std::string_view err, Location pos);
    void consume_semicolon();
    Type consume_type();
};