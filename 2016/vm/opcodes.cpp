#include "opcodes.h"

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

