#include <iostream>
#include <cstdint>
#include <cassert>

// NAME
//      stackvm3 - minimal stackbased bytecode interpreter
//
// SYNOPSIS
//      ./stackvm3
//      0
//
// DESCRIPTION
//      Decrements a variable with initial value 5, until it's zero, then print
//      the result.
//
// LIMITATIONS
//      Only add, branch and subtract operations.
//      No function calls.
//      Only integers as operands.

// The opcodes
enum : uint32_t {
    OP_IADD,         // Pop lval and rval, push lval + rval
    OP_ISUB,         // Pop lval and rval, push lval - rval
    OP_ICONST,       // Push constant onto stack
    OP_BRT,          // Branch to offset if top of stack is true
    OP_GLOAD,        // Load constant from global memory
    OP_GSTORE,       // Store constant to global memory
    OP_PRINT,        // Print top of stack to stdout
    OP_HALT,         // Halt execution
};

class Stack {
public:
    Stack() : sp(0)  {}
    void push(uint32_t val) { checkRep(); arr[sp++] = val; }
    uint32_t pop()          { checkRep(); return arr[--sp]; }
    uint32_t top()          { checkRep(); return arr[sp - 1]; }

private:
    void checkRep()         { assert(sp >= 0 && sp < 64); }
    uint32_t arr[64];
    int sp;
};

void interpret(uint32_t* bytecode, uint32_t* globals) {

    Stack stack;

    int ip = 0;

    while (true) {
        switch (bytecode[ip]) {
            int x, y, a;
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
            x = bytecode[++ip];
            stack.push(x);
            break;
        case OP_BRT:
            a = bytecode[++ip];
            if (stack.top())
                ip = a - 1;  // -1 to accomandate end of loop ip++
            break;
        case OP_GLOAD:
            x = bytecode[++ip];
            y = globals[x];
            stack.push(y);
            break;
        case OP_GSTORE:
            x = bytecode[++ip];
            y = stack.pop();
            globals[x] = y;
            break;
        case OP_PRINT:
            std::cout << stack.top() << "\n";
            break;
        case OP_HALT:
            return;
        }
        ip++;
    }
}

int main() {

    // .top:
    // gload 0
    // iconst 1
    // isub
    // brt top
    // print
    // halt
    uint32_t bytecode[] = { OP_GLOAD,  0x00000000,
                           OP_ICONST,  0x00000001,
                           OP_ISUB,
                           OP_BRT,    0x00000002,
                           OP_PRINT,
                           OP_HALT };

    uint32_t globals[] = {
        0x05,
    };
    interpret(bytecode, globals);
}
