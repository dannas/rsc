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
    Stack() : sp(0), fp(0)  {
    }

    void push(int32_t val) {
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

    int32_t pop() {
        checkRep();
        return arr[--sp];
    }

    int32_t top() {
        checkRep();
        return arr[sp - 1];
    }

    void load(int offset) {
        checkRep();
        push(arr[fp+offset]);
    }

    void store(int offset) {
        checkRep();
        arr[fp+offset] = pop();
    }

private:
    void checkRep() {
        assert(sp >= 0 && sp < 64 && fp >= -1 && fp <= sp);
    }
    friend ostream& operator<< (ostream& os, const Stack& stack);

    int32_t arr[64];
    int sp;
    int fp;
};

ostream& operator<< (ostream& os, const Stack& stack) {
    os << "[";

    if (stack.sp > 0)
        os << stack.arr[0];

    for (int i = 1; i < stack.sp; i++)
        os << ", " << stack.arr[i];

    os << "]";
    return os;
}

// interpret the bytecode in |code| using global variables storied in |globals|.
void interpret(int32_t* code, int32_t* globals) {

    Stack stack;

    int ip = 0;

    while (true) {
        OpCode op = static_cast<OpCode>(code[ip]);

        cout << op << "\t" << stack << "\n";

        ip++; // Move to next opcode or operand

        switch (op) {
            int x, y, addr, nargs, ret;
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
            x = code[ip++];
            stack.push(x);
            break;
        case OP_BRT:
            addr = code[ip++];
            if (stack.top())
                ip = addr;
            break;
        case OP_GLOAD:
            x = code[ip++];
            y = globals[x];
            stack.push(y);
            break;
        case OP_GSTORE:
            x = code[ip++];
            y = stack.pop();
            globals[x] = y;
            break;
        case OP_LOAD:
            x = code[ip++];
            stack.load(x);
            break;
        case OP_STORE:
            x = code[ip++];
            stack.store(x);
            break;
        case OP_PRINT:
            cout << stack.pop() << "\n";
            break;
        case OP_CALL:
            addr = code[ip++];
            nargs = code[ip++];
            stack.push(nargs);
            stack.pushfp();
            stack.push(ip);
            ip = addr;
            break;
        case OP_RET:
            ret = stack.pop();
            ip = stack.pop();
            stack.popfp();
            nargs = stack.pop();
            while (nargs--)
                stack.pop();
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

    int32_t code[] = {
        OP_ICONST, 1,
        OP_ICONST, 2,
        OP_CALL,   9, 2,
        OP_PRINT,
        OP_HALT,
        OP_LOAD, -1,
        OP_LOAD, -2,
        OP_IADD,
        OP_RET,
    };
    int32_t globals[] = {};

    interpret(code, globals);
}
