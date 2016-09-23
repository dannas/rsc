#pragma once

#include <iosfwd>
#include <cstdint>
#include <vector>

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

bool instrExists(const std::string& instr);

OpCode opCodeForInstr(const std::string& instr);

std::ostream& operator<< (std::ostream& os, OpCode code);

// Assemble the assembly code in the stream provided by |in| into bytecode.
std::vector<int32_t> assemble(std::istream& in);

// Interpret the bytecode in |code| using global variables stored in |globals|.
void interpret(int32_t* code, int32_t* globals, std::ostream& out);
