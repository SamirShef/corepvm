#include "vm/include/opcodes.h"
#include "vm/include/vm.h"
#include <cstdlib>

int main(int argc, char **argv) {
    Chunk *chunk = (Chunk*)malloc(sizeof(Chunk));
    VM vm = VM(chunk);

    vm.add_const(chunk, {.ival = 2});
    vm.add_const(chunk, {.ival = 3});
    vm.push_byte(chunk, OP_IADD);
    vm.push_byte(chunk, OP_PRINTI);
    
    vm.push_byte(chunk, OP_HALT);
    vm.execute();
}