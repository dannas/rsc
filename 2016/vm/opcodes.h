#pragma once

#include <iostream>
#include <cassert>

#define FOR_EACH_OPCODE(macro)   \
    macro(OP_IADD,    "iadd")    \
    macro(OP_ISUB,    "isub")    \
    macro(OP_ICONST,  "iconst")  \
    macro(OP_BRT,     "brt")     \
    macro(OP_GLOAD,   "gload")   \
    macro(OP_GSTORE,  "gstore")  \
    macro(OP_LOAD,    "load")    \
    macro(OP_STORE,   "store")   \
    macro(OP_PRINT,   "print")   \
    macro(OP_CALL,    "call")    \
    macro(OP_RET,     "ret")     \
    macro(OP_HALT,    "halt")    \

enum OpCode : int32_t {
#define macro(op, desc) op,
    FOR_EACH_OPCODE(macro)
#undef macro
};

struct NameOpCode {
    const char* name;
    OpCode op;
};

static const NameOpCode nameOpCodeMapping[] = {
#define macro(op, desc) {desc, op},
    FOR_EACH_OPCODE(macro)
#undef macro
};


inline std::ostream& operator<< (std::ostream& os, OpCode code) {
    switch (code) {
#define macro(op, desc) case op: os << desc; break;
        FOR_EACH_OPCODE(macro)
#undef macro
    default:
        assert(false && "unknown opcode");
    }
    return os;
}
