#include "opcodes.h"

#include <iostream>
#include <algorithm>

using namespace std;

struct KeyVal {
    const char* name;
    OpCode val;
};

static const KeyVal instructions[] = {
#define macro(op, desc, nargs) {desc, op},
    FOR_EACH_OPCODE(macro)
#undef macro
};

bool instrExists(const std::string& instr) {
    auto b = begin(instructions);
    auto e = end(instructions);
    auto i = find_if(b, e, [=] (const KeyVal& kv) { return kv.name == instr; });
    return i != e;
}

OpCode opCodeForInstr(const std::string& instr) {
    auto b = begin(instructions);
    auto e = end(instructions);
    auto i = find_if(b, e, [=] (const KeyVal& kv) { return kv.name == instr; });
    return i->val;
}

std::ostream& operator<< (std::ostream& os, OpCode code) {
    switch (code) {
#define macro(op, desc, nargs) case op: os << desc; break;
        FOR_EACH_OPCODE(macro)
#undef macro
    default:
        assert(false && "unknown opcode");
    }
    return os;
}
