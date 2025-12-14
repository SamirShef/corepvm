#include "../../vm/include/opcodes.h"
#include "../include/exception.h"
#include <algorithm>
#include <iterator>
#include "../include/codegen.h"

std::unordered_map<TypeValue, std::vector<TypeValue>> CodeGen::implicitly_cast_allowed_types {
    {TYPE_BOOL, {TYPE_BOOL}},
    {TYPE_CHAR, {TYPE_CHAR, TYPE_SHORT, TYPE_INT, TYPE_LONG, TYPE_FLOAT, TYPE_DOUBLE}},
    {TYPE_SHORT, {TYPE_SHORT, TYPE_INT, TYPE_LONG, TYPE_FLOAT, TYPE_DOUBLE}},
    {TYPE_INT, {TYPE_INT, TYPE_LONG, TYPE_FLOAT, TYPE_DOUBLE}},
    {TYPE_LONG, {TYPE_LONG, TYPE_FLOAT, TYPE_DOUBLE}},
    {TYPE_FLOAT, {TYPE_FLOAT, TYPE_DOUBLE}}
};

Chunk *CodeGen::generate() {
    Chunk *chunk = new Chunk();
    c_chunk = chunk;

    for (auto& stmt : stmts) {
        generate_stmt(*stmt);
    }

    return chunk;
}

void CodeGen::generate_stmt(const ASTNode& stmt) {
    if (auto vds = stmt.as<VDSNode>()) {
        generate_vds_stmt(*vds);
    }
    else {
        error(file_name, "Unsupported statement", stmt.pos);
    }
}

void CodeGen::generate_vds_stmt(const VDSNode& vds) {
    Type type = vds.type;
    if (vds.expr != nullptr) {
        type = generate_expr(*vds.expr);
    }
    else {
        c_chunk->constants.push_back({0});
        uint32_t index = c_chunk->constants.size() - 1;
        int minor2 = index & 0xFF;
        int minor1 = (index >> 8) & 0xFF;
        int major = (index >> 16) & 0xFF;
        c_chunk->code.push_back(OP_PCONST);
        c_chunk->code.push_back(major);
        c_chunk->code.push_back(minor1);
        c_chunk->code.push_back(minor2);
    }
    c_chunk->code.push_back(OP_DEFGLOB);
    c_chunk->code.push_back(OP_STGLOB);
    uint32_t index = global_vars.size();
    int minor2 = index & 0xFF;
    int minor1 = (index >> 8) & 0xFF;
    int major = (index >> 16) & 0xFF;
    c_chunk->code.push_back(major);
    c_chunk->code.push_back(minor1);
    c_chunk->code.push_back(minor2);
    global_vars.emplace(vds.name, GlobVar{type, StackSlot{}, index});
}

Type CodeGen::generate_expr(const ASTNode& expr) {
    if (auto be = expr.as<BENode>()) {
        return generate_be_expr(*be);
    }
    else if (auto ue = expr.as<UENode>()) {
        return generate_ue_expr(*ue);
    }
    else if (auto le = expr.as<LENode>()) {
        return generate_le_expr(*le);
    }
    else if (auto ve = expr.as<VENode>()) {
        return generate_ve_expr(*ve);
    }
    error(file_name, "Unsupported expression", expr.pos);
}

Type CodeGen::generate_be_expr(const BENode& be) {
    Type RHS = generate_expr(*be.RHS);
    Type LHS = generate_expr(*be.LHS);
    Type common_type = get_common_type(RHS, LHS, be.pos);
    switch (be.op) {
        #define PUSH_CODE(i, f) \
        if (common_type.type <= TYPE_LONG) c_chunk->code.push_back(i); \
        else c_chunk->code.push_back(f);

        case TOK_PLUS:
            PUSH_CODE(OP_IADD, OP_FADD);
            break;
        case TOK_MINUS:
            PUSH_CODE(OP_ISUB, OP_FSUB);
            break;
        case TOK_STAR:
            PUSH_CODE(OP_IMUL, OP_FMUL);
            break;
        case TOK_SLASH:
            PUSH_CODE(OP_IDIV, OP_FDIV);
            break;
        case TOK_PRECENT:
            PUSH_CODE(OP_IREM, OP_FREM);
            break;
        default:
            error(file_name, "Unsupported binary operator", be.pos);
        #undef PUSH_CODE
    }
    return common_type;
}

Type CodeGen::generate_ue_expr(const UENode& ue) {
    Type type = generate_expr(*ue.expr);
    switch (ue.op) {
        case TOK_MINUS:
            if (type.type <= TYPE_LONG && type.type > TYPE_BOOL) {
                c_chunk->code.push_back(OP_UIMINUS);
                
            }
            else if (type.type <= TYPE_DOUBLE) {
                c_chunk->code.push_back(OP_UFMINUS);
            }
            else {
                error(file_name, "Unary minus does not supported this type", ue.pos);
            }
            break;
        case TOK_NOT:
            if (type.type == TYPE_BOOL) {
                c_chunk->code.push_back(OP_UNOT);
            }
            else {
                error(file_name, "Unary logical not does not supported this type", ue.pos);
            }
            break;
        default:
            error(file_name, "Unsupported unary operator", ue.pos);
    }
    return type;
}

Type CodeGen::generate_le_expr(const LENode& le) {
    switch (le.val.type.type) {
        #define PUSH_CONST(name, val) \
        c_chunk->constants.push_back({name = val}); \
        uint32_t index = c_chunk->constants.size() - 1; \
        int minor2 = index & 0xFF; \
        int minor1 = (index >> 8) & 0xFF; \
        int major = (index >> 16) & 0xFF; \
        c_chunk->code.push_back(OP_PCONST); \
        c_chunk->code.push_back(major); \
        c_chunk->code.push_back(minor1); \
        c_chunk->code.push_back(minor2);
        
        case TYPE_BOOL: {
            PUSH_CONST(.ival, le.val.b);
            break;
        }
        case TYPE_CHAR: {
            PUSH_CONST(.ival, le.val.c);
            break;
        }
        case TYPE_SHORT: {
            PUSH_CONST(.ival, le.val.s);
            break;
        }
        case TYPE_INT: {
            PUSH_CONST(.ival, le.val.i);
            break;
        }
        case TYPE_LONG: {
            PUSH_CONST(.ival, le.val.l);
            break;
        }
        case TYPE_FLOAT: {
            PUSH_CONST(.fval, le.val.f);
            break;
        }
        case TYPE_DOUBLE: {
            PUSH_CONST(.fval, le.val.d);
            break;
        }
        default:
            error(file_name, "Literal does not supported", le.pos);
        #undef PUSH_CONST
    }
    return le.val.type;
}

Type CodeGen::generate_ve_expr(const VENode& ve) {
    auto it = global_vars.find(ve.name);
    uint32_t index = it->second.index;
    int minor2 = index & 0xFF;
    int minor1 = (index >> 8) & 0xFF;
    int major = (index >> 16) & 0xFF;
    c_chunk->code.push_back(OP_LDGLOB);
    c_chunk->code.push_back(major);
    c_chunk->code.push_back(minor1);
    c_chunk->code.push_back(minor2);
    return it->second.type;
}

bool CodeGen::has_common_type(Type LHS, Type RHS) {
    if (LHS == RHS) {
        return true;
    }
    if (implicitly_cast_allowed_types.find(LHS.type) != implicitly_cast_allowed_types.end() &&
        std::find(implicitly_cast_allowed_types[LHS.type].begin(), implicitly_cast_allowed_types[LHS.type].end(), RHS.type) != implicitly_cast_allowed_types[LHS.type].end()) {
        return true;
    }
    return false;
}

Type CodeGen::get_common_type(Type LHS, Type RHS, Location pos) {
    if (LHS.type == RHS.type) {
        return LHS;
    }

    if (has_common_type(LHS, RHS)) {
        return Type(*std::find(implicitly_cast_allowed_types[LHS.type].begin(), implicitly_cast_allowed_types[LHS.type].end(), RHS.type).base(), RHS.name, RHS.is_const);
    }
    if (has_common_type(RHS, LHS)) {
        return Type(*std::find(implicitly_cast_allowed_types[RHS.type].begin(), implicitly_cast_allowed_types[RHS.type].end(), LHS.type).base(), LHS.name, LHS.is_const);
    }
    
    error(file_name, "Does not have common type", pos);
}