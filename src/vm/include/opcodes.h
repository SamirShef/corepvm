#pragma once
#include <cstdint>

enum OpCodes : uint8_t {
    OP_HALT,
    OP_PCONST,
    OP_IADD,
    OP_FADD,
    OP_ISUB,
    OP_FSUB,
    OP_IMUL,
    OP_FMUL,
    OP_IDIV,
    OP_FDIV,
    OP_IREM,
    OP_FREM,
    OP_UIMINUS,
    OP_UFMINUS,
    OP_UNOT,
    OP_PRINTI,
    OP_PRINTF,
    OP_PRINTO,
    OP_DEFGLOB,
    OP_LDGLOB,
    OP_STGLOB,
    OP_RET,
    OP_CALL
};