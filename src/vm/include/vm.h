#pragma once
#include <cstdint>
#include <stack>
#include <vector>

union StackSlot {
    int64_t ival;
    double fval;
    void *objval;
};

struct Chunk {
    std::vector<StackSlot> constants;
    std::vector<uint8_t> code;
};

struct VM {
    std::stack<StackSlot> stack;
    Chunk *chunk;
    uint8_t *ip;

    VM(Chunk *c) : chunk(c) {}

    void push_byte(Chunk *chunk, uint8_t byte);
    uint8_t pop_byte(Chunk *chunk);
    void push_val(StackSlot slot);
    StackSlot pop_val();
    uint32_t add_const(Chunk *chunk, StackSlot slot);
    void execute();
};