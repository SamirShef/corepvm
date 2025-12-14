#include "../include/exception.h"
#include "../include/parser.h"

std::vector<ASTNodePtr> Parser::parse() {
    std::vector<ASTNodePtr> stmts;
    while (pos < tokens.size()) {
        stmts.push_back(parse_stmt());
    }
    return stmts;
}

ASTNodePtr Parser::parse_stmt() {
    if (match(TOK_LET)) {
        return parse_vds_stmt();
    }
    else {
        error(file_name, "Unsupproted statement", peek().pos);
    }
}

ASTNodePtr Parser::parse_vds_stmt() {
    Location pos = peek(-1).pos;
    Type type = consume_type();
    std::string name = consume(TOK_ID, "Expected identifier", peek().pos).val;
    ASTNodePtr expr = nullptr;
    if (match(TOK_EQ)) {
        expr = parse_expr();
    }
    consume_semicolon();
    return std::make_shared<VDSNode>(name, expr, pos);
}

ASTNodePtr Parser::parse_expr() {
    return parse_land_expr();
}

ASTNodePtr Parser::parse_land_expr() {
    ASTNodePtr expr = parse_lor_expr();
    while (match(TOK_LAND)) {
        Token tok = peek(-1);
        expr = std::make_shared<BENode>(tok.type, expr, parse_lor_expr(), tok.pos);
    }
    return expr;
}

ASTNodePtr Parser::parse_lor_expr() {
    ASTNodePtr expr = parse_eq_expr();
    while (match(TOK_LOR)) {
        Token tok = peek(-1);
        expr = std::make_shared<BENode>(tok.type, expr, parse_eq_expr(), tok.pos);
    }
    return expr;
}

ASTNodePtr Parser::parse_eq_expr() {
    ASTNodePtr expr = parse_comp_expr();
    while (match(TOK_EQ_EQ) || match(TOK_NOT_EQ)) {
        Token tok = peek(-1);
        expr = std::make_shared<BENode>(tok.type, expr, parse_comp_expr(), tok.pos);
    }
    return expr;
}

ASTNodePtr Parser::parse_comp_expr() {
    ASTNodePtr expr = parse_additive_expr();
    while (match(TOK_GT) || match(TOK_GT_EQ) || match(TOK_LS) || match(TOK_LS_EQ)) {
        Token tok = peek(-1);
        expr = std::make_shared<BENode>(tok.type, expr, parse_additive_expr(), tok.pos);
    }
    return expr;
}

ASTNodePtr Parser::parse_additive_expr() {
    ASTNodePtr expr = parse_multiplicative_expr();
    while (match(TOK_PLUS) || match(TOK_MINUS)) {
        Token tok = peek(-1);
        expr = std::make_shared<BENode>(tok.type, expr, parse_multiplicative_expr(), tok.pos);
    }
    return expr;
}

ASTNodePtr Parser::parse_multiplicative_expr() {
    ASTNodePtr expr = parse_unary_expr();
    while (match(TOK_STAR) || match(TOK_SLASH) || match(TOK_PRECENT)) {
        Token tok = peek(-1);
        expr = std::make_shared<BENode>(tok.type, expr, parse_unary_expr(), tok.pos);
    }
    return expr;
}

ASTNodePtr Parser::parse_unary_expr() {
    while (match(TOK_NOT) || match(TOK_MINUS) || match(TOK_PRECENT)) {
        Token tok = peek(-1);
        return std::make_shared<UENode>(tok.type, parse_primary_expr(), tok.pos);
    }
    return parse_primary_expr();
}

ASTNodePtr Parser::parse_primary_expr() {
    Token tok = peek();
    pos++;
    switch (tok.type) {
        case TOK_LPAREN: {
            ASTNodePtr expr = parse_expr();
            consume(TOK_RPAREN, "Expected \033[0m')'\033[31m", tok.pos);
            return expr;
        }
        #define LIT(val) std::make_shared<LENode>(val, tok.pos)
        case TOK_BOOL_L:
            return LIT(tok.val == "true");
        case TOK_CHAR_L:
            return LIT((char8_t)tok.val[0]);
        case TOK_SHORT_L:
            return LIT((int16_t)std::stoll(tok.val));
        case TOK_INT_L:
            return LIT((int32_t)std::stoll(tok.val));
        case TOK_LONG_L:
            return LIT((int64_t)std::stoll(tok.val));
        case TOK_FLOAT_L:
            return LIT((float_t)std::stold(tok.val));
        case TOK_DOUBLE_L:
            return LIT((double_t)std::stold(tok.val));
        #undef LIT
        case TOK_ID:
            return std::make_shared<VENode>(tok.val, tok.pos);
        default:
            error(file_name, "Unsupported expression", tok.pos);
    }
}

const Token Parser::peek(uint32_t rpos) const {
    if (pos + rpos < tokens.size() && pos + rpos >= 0) {
        return tokens[pos + rpos];
    }
    error(file_name, "Index out of range", {0, 0});
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        pos++;
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, std::string_view err, Location pos) {
    Token tok = peek();
    if (tok.type == type) {
        this->pos++;
        return tok;
    }
    error(file_name, err, pos);
}

void Parser::consume_semicolon() {
    consume(TOK_SEMICOLON, "Expected semicolon", peek().pos);
}

Type Parser::consume_type() {
    Token tok = peek();
    bool is_const = false;
    if (tok.type == TOK_CONST) {
        is_const = true;
        pos++;
        tok = peek();
    }
    pos++;
    switch (tok.type) {
        #define TYPE(type) Type(type, tok.val, is_const)
        case TOK_BOOL:
            return TYPE(TYPE_BOOL);
        case TOK_CHAR:
            return TYPE(TYPE_CHAR);
        case TOK_SHORT:
            return TYPE(TYPE_SHORT);
        case TOK_INT:
            return TYPE(TYPE_INT);
        case TOK_LONG:
            return TYPE(TYPE_LONG);
        case TOK_FLOAT:
            return TYPE(TYPE_FLOAT);
        case TOK_DOUBLE:
            return TYPE(TYPE_DOUBLE);
        case TOK_ID:
            return TYPE(TYPE_CLASS);
        #undef TYPE
        default:
            error(file_name, "Expected type", tok.pos);
    }
}