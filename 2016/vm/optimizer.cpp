#include "vm.h"

#include <cassert>

#define CASE break;case

using namespace std;

// TODO(dannas): Add optimization steps
// ### What sort of optimizations can we do?
// ### We don't have access to function boundaries in the bytecode,
// ### must that be added?
// ### See...
// ### Saabas, Ando, and Tarmo Uustalu.
// ### "Type systems for optimizing stack-based code."
// ### Electronic Notes in Theoretical Computer Science 190.1 (2007): 103-119.
// ### .. For a suggestion on how to optimize across block boundaries.


class Stack {
public:
    Stack() : pos(0), codepos(0) {}

    void push(int32_t val) {
        assert(pos < 3);
        buf[pos++] = val;

    }

    void reset() {
        pos = 0;
        codepos = 0;
    }

    int32_t pop() {
        assert(pos > 0 && pos <= 3);
        return buf[--pos];
    }

    bool empty() {
        return pos == 0;
    }

    size_t size() {
        return pos;
    }

    size_t insertionPos() {
        return codepos;
    }

private:
    int32_t buf[2];
    size_t pos;
    size_t codepos;
};

vector<int32_t> foldConstants(vector<int32_t> code) {
    vector<int32_t> optimized;
    Stack stack;
    int32_t x, y;


    for (size_t i = 0; i < code.size(); ) {
        OpCode op = static_cast<OpCode>(code[i]);
        i++;  // Move to next opcode or operand

        switch (op) {

        CASE OP_POP:
            stack.reset();
        // TODO(dannas): Check that this logic is correct:
        // We evaluate adds etcetera if the stack.size() == 2.
        // We reset the stack if not.
        // When we reset the stack, we write the content to
        // |optimized|.
        CASE OP_IADD:
            if (stack.size() != 2) {
                stack.reset();
            } else {
                x = stack.pop();
                y = stack.pop();
                stack.push(x+y);
            }
        CASE OP_ISUB:
        CASE OP_IMULT:
        CASE OP_IDIV:
        CASE OP_IMOD:
            ; // TODO(dannas): Handle these cases
            stack.reset();
        CASE OP_ICONST:
            if (stack.empty()) {
                stack.reset();
            }
            x = code[i++];
            stack.push(x);
        CASE OP_ILT:
        CASE OP_IEQ:
            ; // TODO(dannas): Handle these cases
            stack.reset();
        CASE OP_BR:
            stack.reset();
        CASE OP_BRT:
            stack.reset();
        CASE OP_GLOAD:
            stack.reset();
        CASE OP_GSTORE:
            stack.reset();
        CASE OP_LOAD:
            x = code[i++];
            stack.reset();
        CASE OP_STORE:
            x = code[i++];
            stack.reset();
        CASE OP_PRINT:
            stack.reset();
        CASE OP_CALL:
            i++; // address
            i++; // nargs
            i++; // nlocals
            stack.reset();
        CASE OP_RET:
            stack.reset();
        CASE OP_HALT:
            stack.reset();
        default:
            assert(false && "unknown opcode");
        }

    }
    return optimized;
}
