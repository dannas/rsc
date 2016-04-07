#pragma once

#include <iosfwd>
#include <cassert>
#include <cstdint>

#define FOR_EACH_OPCODE(macro)      \
    macro(OP_IADD,    "iadd",   0)  \
    macro(OP_ISUB,    "isub",   0)  \
    macro(OP_ICONST,  "iconst", 1)  \
    macro(OP_BRT,     "brt",    1)  \
    macro(OP_GLOAD,   "gload",  1)  \
    macro(OP_GSTORE,  "gstore", 1)  \
    macro(OP_LOAD,    "load",   1)  \
    macro(OP_STORE,   "store",  1)  \
    macro(OP_PRINT,   "print",  0)  \
    macro(OP_CALL,    "call",   1)  \
    macro(OP_RET,     "ret",    0)  \
    macro(OP_HALT,    "halt",   0)  \

enum OpCode : int32_t {
#define macro(op, desc, nargs) op,
    FOR_EACH_OPCODE(macro)
#undef macro
};

bool InstrExists(const std::string& instr);

OpCode OpCodeForInstr(const std::string& instr);

std::ostream& operator<< (std::ostream& os, OpCode code);
