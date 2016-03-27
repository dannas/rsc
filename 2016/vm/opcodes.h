#pragma once

#include <iosfwd>
#include <cassert>
#include <cstdint>

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

bool InstrExists(const std::string& instr);

std::ostream& operator<< (std::ostream& os, OpCode code);
