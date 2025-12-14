#include "compiler/include/ast.h"
#include "compiler/include/codegen.h"
#include "compiler/include/lexer.h"
#include "compiler/include/parser.h"
#include "vm/include/vm.h"
#include <filesystem>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "\033[31mUsage: psharp path/to/src\033[0m\n";
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "\033[31mError openning file: does not exist!\033[0m\n";
        return 1;
    }
    auto file_name = std::filesystem::absolute(argv[1]).string();
    std::ostringstream content;
    content << file.rdbuf();
    Lexer lex(content.str(), file_name);
    file.close();

    std::vector<Token> tokens(lex.tokenize());
    
    Parser parser(file_name, tokens);
    std::vector<ASTNodePtr> stmts(parser.parse());

    CodeGen codegen(file_name, stmts);
    VM vm(codegen.generate());
    //vm.print_disassembly();
    vm.execute();
}