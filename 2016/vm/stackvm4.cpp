#include <iostream>
#include <cstdint>
#include <cassert>

using namespace std;

// NAME
//      stackvm4 - minimal stackbased bytecode interpreter
//
// SYNOPSIS
//      TODO
//
// DESCRIPTION
//      TODO
//
// LIMITATIONS
//      TODO

#define FOR_EACH_OPCODE(macro)   \
    macro(OP_IADD,    "iadd")    \
    macro(OP_ISUB,    "isub")    \
    macro(OP_ICONST,  "iconst")  \
    macro(OP_BRT,     "brt")     \
    macro(OP_GLOAD,   "gload")   \
    macro(OP_GSTORE,  "gstore")  \
    macro(OP_PRINT,   "print")   \
    macro(OP_CALL,    "call")    \
    macro(OP_RET,     "ret")     \
    macro(OP_HALT,    "halt")    \

enum OpCode : uint32_t {
#define macro(op, desc) op,
    FOR_EACH_OPCODE(macro)
#undef macro
};

ostream& operator<< (ostream& os, OpCode code) {
    switch (code) {
#define macro(op, desc) case op: os << desc; break;
        FOR_EACH_OPCODE(macro)
#undef macro
    default:
        assert(false && "unknown opcode");
    }
    return os;
}

class Stack {
public:
    Stack() : sp(0), fp(-1)  {
    }

    void push(uint32_t val) {
        checkRep();
        arr[sp++] = val;
    }
    void pushfp() {
        arr[sp++] = fp;
        fp = sp;
    }

    void popfp() {
        fp = pop();
    }

    uint32_t pop() {
        checkRep();
        return arr[--sp];
    }

    uint32_t top() {
        checkRep();
        return arr[sp - 1];
    }

private:
    void checkRep() {
        assert(sp >= 0 && sp < 64 && fp >= -1 && fp <= sp);
    }
    friend ostream& operator<< (ostream& os, const Stack& stack);

    uint32_t arr[64];
    int sp;
    int fp;
};

ostream& operator<< (ostream& os, const Stack& stack) {
    string delim = "";
    os << "[";
    for (int i = 0; i < stack.sp; i++) {
        if (i > 0)
            delim = ", ";
        os << delim << stack.arr[i];
    }
    os << "]";
    return os;
}

void interpret(uint32_t* bytecode, uint32_t* globals) {

    Stack stack;

    int ip = 0;

    while (true) {
        OpCode op = static_cast<OpCode>(bytecode[ip]);

        cout << op << "\t" << stack << "\n";

        ip++; // Move to next opcode or operand

        switch (op) {
            int x, y, addr, ret;
        case OP_IADD:
            x = stack.pop();
            y = stack.pop();
            stack.push(y + x);
            break;
        case OP_ISUB:
            x = stack.pop();
            y = stack.pop();
            stack.push(y - x);
            break;
        case OP_ICONST:
            x = bytecode[ip++];
            stack.push(x);
            break;
        case OP_BRT:
            addr = bytecode[ip++];
            if (stack.top())
                ip = addr;
            break;
        case OP_GLOAD:
            x = bytecode[ip++];
            y = globals[x];
            stack.push(y);
            break;
        case OP_GSTORE:
            x = bytecode[ip++];
            y = stack.pop();
            globals[x] = y;
            break;
        case OP_PRINT:
            cout << stack.pop() << "\n";
            break;
        case OP_CALL:
            addr = bytecode[ip++];
            stack.push(ip);
            stack.pushfp();
            ip = addr;
            break;
        case OP_RET:
            ret = stack.pop();
            stack.popfp();
            ip = stack.pop();
            stack.push(ret);
            break;
        case OP_HALT:
            return;
        default:
            assert(false && "unknown opcode");
        }
    }
}

int main() {

    // .start
    //    call .hello
    //    halt
    // .hello
    //    iconst 42
    //    print
    //    ret

    uint32_t bytecode[] = {
        OP_CALL, 4,
        OP_PRINT,
        OP_HALT,
        OP_ICONST, 1,
        OP_ICONST, 1,
        OP_IADD,
        OP_RET,
    };
    uint32_t globals[] = {};

    interpret(bytecode, globals);
}
