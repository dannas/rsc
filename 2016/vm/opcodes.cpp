#include "opcodes.h"

#include <iostream>
#include <algorithm>

using namespace std;

static const char* instructions[] = {
#define macro(op, desc) desc,
    FOR_EACH_OPCODE(macro)
#undef macro
};

bool InstrExists(const std::string& instr) {
    auto b = begin(instructions);
    auto e = end(instructions);
    auto i = find(b, e, instr);
    return i != e;
}

std::ostream& operator<< (std::ostream& os, OpCode code) {
    switch (code) {
#define macro(op, desc) case op: os << desc; break;
        FOR_EACH_OPCODE(macro)
#undef macro
    default:
        assert(false && "unknown opcode");
    }
    return os;
}
