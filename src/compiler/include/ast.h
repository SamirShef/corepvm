#pragma once
#include "location.h"
#include "to_str.h"
#include "token.h"
#include <cmath>
#include <memory>
#include <uchar.h>

#define LOC Location p
#define AST ASTNode(p, get_type())

enum NodeType : uint8_t {
    NODE_UNKNOWN,

    NODE_VDS,           // variable definition statement

    NODE_BE,            // binary expression
    NODE_UE,            // unary expression
    NODE_LE,            // literal expression
    NODE_VE,            // variable expression
};

enum TypeValue {
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_SHORT,
    TYPE_INT,
    TYPE_LONG,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_NOTH,
    TYPE_STR,
    TYPE_CLASS
};

struct Type : ToStringable {
    TypeValue type;
    std::string name;
    bool is_const;

    Type(TypeValue t, std::string n, bool ic) : type(t), name(n), is_const(ic) {}

    bool operator==(Type& other) {
        return type == other.type && name == other.name && is_const == other.is_const;
    }

    bool operator!=(Type& other) {
        return !(*this == other);
    }

    std::string to_str() const override {
        return (is_const ? "const " : "") + name;
    }
};

struct Value {
    Type type;
    union {
        bool     b;
        char8_t  c;
        int16_t  s;
        int32_t  i;
        int64_t  l;
        float_t  f;
        double_t d;
    };
    std::string str;
    
    Value(bool v)               : type(TYPE_BOOL, "bool", false),   b(v) {}
    Value(char8_t v)            : type(TYPE_CHAR, "char", false),   c(v) {}
    Value(int16_t v)            : type(TYPE_SHORT, "i16", false),  s(v) {}
    Value(int32_t v)            : type(TYPE_INT, "i32", false),    i(v) {}
    Value(int64_t v)            : type(TYPE_LONG, "i64", false),   l(v) {}
    Value(float_t v)            : type(TYPE_FLOAT, "f32", false),  f(v) {}
    Value(double_t v)           : type(TYPE_DOUBLE, "f64", false), d(v) {}
    Value(const std::string& v) : type(TYPE_STR, "str", false),    str(v) {}
    ~Value() = default;
};

struct ASTNode {
    Location pos;
    NodeType type;
    
    ASTNode(LOC, NodeType t) : pos(p), type(t) {}
    virtual ~ASTNode() = default;

    template<typename T>
    T *as() {
        if (T::get_type() == type) {
            return static_cast<T*>(this);
        }
        return nullptr;
    }
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

struct VDSNode : ASTNode {
    std::string name;
    ASTNodePtr expr;

    static NodeType get_type() { return NODE_VDS; }

    VDSNode(std::string n, ASTNodePtr e, LOC) : name(n), expr(e), AST {}
    ~VDSNode() override = default;
};

struct BENode : ASTNode {
    TokenType op;
    ASTNodePtr LHS;
    ASTNodePtr RHS;

    static NodeType get_type() { return NODE_BE; }

    BENode(TokenType op, ASTNodePtr LHS, ASTNodePtr RHS, LOC) : op(op), LHS(LHS), RHS(RHS), AST {}
    ~BENode() override = default;
};

struct UENode : ASTNode {
    TokenType op;
    ASTNodePtr expr;

    static NodeType get_type() { return NODE_UE; }

    UENode(TokenType op, ASTNodePtr e, LOC) : op(op), expr(e), AST {}
    ~UENode() override = default;
};

struct LENode : ASTNode {
    Value val;

    static NodeType get_type() { return NODE_LE; }

    LENode(Value v, LOC) : val(v), AST {}
    ~LENode() override = default;
};

struct VENode : ASTNode {
    std::string name;

    static NodeType get_type() { return NODE_VE; }

    VENode(std::string n, LOC) : name(n), AST {}
    ~VENode() override = default;
};