#include <cstdio>
#include <cstdint>

// NAME
//      stackvm2 - minimal stackbased bytecode interpreter
//
// SYNOPSIS
//      ./stackvm2
//      0
//
// DESCRIPTION
//      Decrements a variable with initial value 5, until it's zero, then print
//      the result.
//      
//
// LIMITATIONS
//      Only add, branch and subtract operations.
//      No function calls.
//      Only integers as operands.
//      No check for stack overflows.

// The opcodes
enum : uint8_t {
    OP_IADD,         // Pop lval and rval, push lval + rval
    OP_ISUB,         // Pop lval and rval, push lval - rval
    OP_ICONST,       // Push constant onto stack
    OP_BRT,          // Branch to offset if top of stack is true
    OP_HALT,         // Halt execution
};

uint32_t toUint32(uint8_t *buf) {
    return *buf << 24 | *(buf + 1) << 16 | *(buf+2) << 8 | *(buf + 3);
}

int interpret(uint8_t* bytecode) {

    int32_t stack[64];

    int ip = 0;
    int sp = 0;

    while (true) {
        switch (bytecode[ip]) {
            int x, y, a;
        case OP_IADD:
            x = stack[--sp];
            y = stack[--sp];
            stack[sp++] = y + x;
            break;
        case OP_ISUB:
            x = stack[--sp];
            y = stack[--sp];
            stack[sp++] = y - x;
            break;
        case OP_ICONST:
            x = toUint32(&bytecode[ip + 1]);
            stack[sp++] = x;
            ip += 4;
            break;
        case OP_BRT:
            a = toUint32(&bytecode[ip + 1]);
            if (stack[sp-1])
                ip = a - 1;  // -1 to accomandate end of loop ip++
            else
                ip += 4;
            break;
        case OP_HALT:
            goto out;
            break;
        }
        ip++;
    }
out:
    return stack[--sp];
}

int main() {

    // .top:
    // iconst 5
    // iconst 1
    // isub
    // brt top
    // halt
    uint8_t bytecode[] = { OP_ICONST, 0x00, 0x00, 0x00, 0x05, 
                           OP_ICONST, 0x00, 0x00, 0x00, 0x01,
                           OP_ISUB,
                           OP_BRT,    0x00, 0x00, 0x00, 0x05,
                           OP_HALT };

    printf("%d\n", interpret(bytecode));
}
