#pragma once
#include "../../vm/include/vm.h"
#include "ast.h"
#include <unordered_map>
#include <utility>
#include <vector>

class CodeGen {
    std::string_view file_name;
    std::vector<ASTNodePtr> stmts;
    Chunk *c_chunk;
    
    struct GlobVar {
        Type type;
        StackSlot val;
        size_t index;
    };
    std::unordered_map<std::string, GlobVar> global_vars;
    static std::unordered_map<TypeValue, std::vector<TypeValue>> implicitly_cast_allowed_types;

public:
    CodeGen(std::string_view fn, std::vector<ASTNodePtr>& s) : file_name(fn), stmts(s) {}

    Chunk *generate();

private:
    void generate_stmt(const ASTNode& stmt);
    void generate_vds_stmt(const VDSNode& vds);

    Type generate_expr(const ASTNode& expr);
    Type generate_be_expr(const BENode& be);
    Type generate_ue_expr(const UENode& ue);
    Type generate_le_expr(const LENode& le);
    Type generate_ve_expr(const VENode& ve);

    bool has_common_type(Type LHS, Type RHS);
    Type get_common_type(Type LHS, Type RHS, Location pos);
};