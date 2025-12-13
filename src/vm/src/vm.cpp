#include "../include/opcodes.h"
#include "../include/vm.h"
#include <cmath>
#include <iostream>

void VM::push_byte(Chunk *chunk, uint8_t byte) {
    chunk->code.push_back(byte);
    ip++;
}

uint8_t VM::pop_byte(Chunk *chunk) {
    auto byte = *ip++;
    return byte;
}

void VM::push_val(StackSlot slot) {
    stack.push(slot);
}

StackSlot VM::pop_val() {
    auto val = stack.top();
    stack.pop();
    return val;
}

uint32_t VM::add_const(Chunk *chunk, StackSlot slot) {
    uint32_t index = chunk->constants.size();
    chunk->constants.push_back(slot);
    int minor2 = index & 0xFF;
    int minor1 = (index >> 8) & 0xFF;
    int major = (index >> 16) & 0xFF;
    push_byte(chunk, OP_PCONST);
    push_byte(chunk, major);
    push_byte(chunk, minor1);
    push_byte(chunk, minor2);
    return index;
}

void VM::execute() {
    ip = &chunk->code.front();
    while (*ip != OP_HALT) {
        switch (static_cast<OpCodes>(*(ip++))) {
            case OP_PCONST: {
                uint8_t minor2 = pop_byte(chunk);
                uint8_t minor1 = pop_byte(chunk);
                uint8_t major = pop_byte(chunk);
                uint32_t index = (major) | (minor1 << 8) | (minor2 << 16);
                push_val(chunk->constants[index]);
                break;
            }
            case OP_IADD: {
                int64_t b = pop_val().ival;
                int64_t a = pop_val().ival;
                push_val({.ival = a + b});
                break;
            }
            case OP_FADD: {
                double b = pop_val().fval;
                double a = pop_val().fval;
                push_val({.fval = a + b});
                break;
            }
            case OP_ISUB: {
                int64_t b = pop_val().ival;
                int64_t a = pop_val().ival;
                push_val({.ival = a - b});
                break;
            }
            case OP_FSUB: {
                double b = pop_val().fval;
                double a = pop_val().fval;
                push_val({.fval = a - b});
                break;
            }
            case OP_IMUL: {
                int64_t b = pop_val().ival;
                int64_t a = pop_val().ival;
                push_val({.ival = a * b});
                break;
            }
            case OP_FMUL: {
                double b = pop_val().fval;
                double a = pop_val().fval;
                push_val({.fval = a * b});
                break;
            }
            case OP_IDIV: {
                int64_t b = pop_val().ival;
                int64_t a = pop_val().ival;
                push_val({.ival = a / b});
                break;
            }
            case OP_FDIV: {
                double b = pop_val().fval;
                double a = pop_val().fval;
                push_val({.fval = a / b});
                break;
            }
            case OP_IREM: {
                int64_t b = pop_val().ival;
                int64_t a = pop_val().ival;
                push_val({.ival = a % b});
                break;
            }
            case OP_FREM: {
                double b = pop_val().fval;
                double a = pop_val().fval;
                push_val({.fval = std::fmod(a, b)});
                break;
            }
            case OP_PRINTI:
                std::cout << pop_val().ival << '\n';
                break;
            case OP_PRINTF:
                std::cout << pop_val().fval << '\n';
                break;
            case OP_PRINTO:
                std::cout << pop_val().objval << '\n';
                break;
        }
    }
}