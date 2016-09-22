#include "vm.h"

#include <algorithm>
#include <cassert>
#include <iostream>

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

static const KeyVal* findInstr(const string& instr) {
    auto b = begin(instructions);
    auto e = end(instructions);
    return find_if(b, e, [=] (const KeyVal& kv) { return kv.name == instr; });
}

bool instrExists(const string& instr) {
    auto i = findInstr(instr);
    return i != end(instructions);
}

OpCode opCodeForInstr(const string& instr) {
    auto i = findInstr(instr);
    return i->val;
}

ostream& operator<< (ostream& os, OpCode code) {
    switch (code) {
#define macro(op, desc, nargs) case op: os << desc; break;
        FOR_EACH_OPCODE(macro)
#undef macro
    default:
        assert(false && "unknown opcode");
    }
    return os;
}
