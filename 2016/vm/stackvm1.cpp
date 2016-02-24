#include <cstdio>
#include <cstdint>

// NAME
//      stackvm1 - minimal stackbased bytecode interpreter
//
// SYNOPSIS
//      ./stackvm1
//      pointer::interpret=6
//      array::interpret=6
//
// DESCRIPTION
//      Interprets |bytecode| and prints the result.
//      One interpret() uses pointers internally, the other, arrays to demonstrate
//      the tradeoffs in readability; the references to the ip is cleaner with
//      pointers but the stack is more readable with indexes.
//
// LIMITATIONS
//      Only handles the add operation
//      No jumps or function calls.
//      No checks for stack overflows.

// The opcodes
enum : uint8_t {
    OP_ADD,
    OP_CONST,
    OP_HALT,
};

namespace array {
int interpret(uint8_t* bytecode) {

    int stack[64];

    int ip = 0;
    int sp = 0;

    while (true) {
        switch (bytecode[ip]) {
            int x, y;
        case OP_ADD:
            x = stack[--sp];
            y = stack[--sp];
            stack[sp++] = x + y;
            break;
        case OP_HALT:
            goto out;
            break;
        case OP_CONST:
            x = bytecode[ip+1] << 24 | bytecode[ip+2] << 16 | bytecode[ip+3] << 8 | bytecode[ip+4];
            stack[sp++] = x;
            ip += 4;
            break;
        }
        ip++;
    }
out:
    return stack[--sp];
}
}

namespace pointer {
int interpret(uint8_t* bytecode) {

    int stack[64];

    uint8_t* ip = bytecode;
    int* sp = stack;

    while (true) {
        switch (*ip) {
            int x, y;
        case OP_ADD:
            x = *--sp;
            y = *--sp;
            *sp++ = x + y;
            break;
        case OP_HALT:
            goto out;
            break;
        case OP_CONST:
            x = *(ip+1) << 24 | *(ip+2) << 16 | *(ip+3) << 8 | *(ip+4);
            *sp++ = x;
            ip += 4;
            break;
        }
        ip++;
    }
out:
    return *--sp;
}
}

int main() {
    uint8_t bytecode[] = { OP_CONST, 0x00, 0x00, 0x00, 0x02, 
                           OP_CONST, 0x00, 0x00, 0x00, 0x04,
                           OP_ADD, 
                           OP_HALT };

    printf("pointer::interpret=%d\n", pointer::interpret(bytecode));
    printf("array::interpret=%d\n",     array::interpret(bytecode));
}
