#include "vm.h"

#include <cassert>
#include <vector>

#define CASE break;case

using namespace std;

const int kMaxStackSize = 64;	// TODO(dannas): Max stack size is hardcoded

vector<uint8_t> compile(const vector<int32_t>& byteCode) {

    vector<uint8_t> buf;
    int ip = 0;

    while (true) {
        OpCode op = static_cast<OpCode>(byteCode[ip]);

        ip++; // Move to next opcode or operand

        switch (op) {
        CASE OP_POP:
        CASE OP_IADD:
        CASE OP_ISUB:
        CASE OP_IMULT:
        CASE OP_IDIV:
        CASE OP_IMOD:
        CASE OP_ICONST:
        CASE OP_ILT:
        CASE OP_IEQ:
        CASE OP_BR:
        CASE OP_BRT:
        CASE OP_GLOAD:
        CASE OP_GSTORE:
        CASE OP_LOAD:
        CASE OP_STORE:
        CASE OP_PRINT:
        CASE OP_CALL:
        CASE OP_RET:
        CASE OP_HALT:
            assert(false && "unhandled opcode");
        default:
            assert(false && "unknown opcode");
        }
    }
    return buf;
}
