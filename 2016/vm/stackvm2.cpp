#include <cstdio>
#include <cstdint>

// NAME
//      stackvm2 - minimal stackbased bytecode interpreter
//
// SYNOPSIS
//      ./stackvm2
//      TODO
//
// DESCRIPTION
//      TODO
//
// LIMITATIONS
//      TODO

// The opcodes
enum : uint8_t {
    OP_IADD,         // Pop lval and rval, push lval + rval
    OP_ISUB,         // Pop lval and rval, push lval - rval
    OP_ICONST,       // Push constant onto stack
    OP_BRT,          // Branch to offset if top of stack is true
    OP_HALT,         // Halt execution
};

int interpret(uint8_t* bytecode) {

    int32_t stack[64];

    int ip = 0;
    int sp = 0;

    while (true) {
            printf("\t%d %d\n", bytecode[ip], stack[sp-1]);
        switch (bytecode[ip]) {
            int x, y, a;
        case OP_IADD:
            x = stack[--sp];
            y = stack[--sp];
            stack[sp++] = x + y;
            break;
        case OP_ISUB:
            x = stack[--sp];
            y = stack[--sp];
            stack[sp++] = y - x; // TODO: right order?
            break;
        case OP_ICONST:
            // TODO: Ugly. fix!
            x = bytecode[ip+1] << 24 | bytecode[ip+2] << 16 | bytecode[ip+3] << 8 | bytecode[ip+4];
            stack[sp++] = x;
            ip += 4;
            break;
        case OP_BRT:
            a = bytecode[ip+1] << 24 | bytecode[ip+2] << 16 | bytecode[ip+3] << 8 | bytecode[ip+4];
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
    uint8_t bytecode[] = { OP_ICONST, 0x00, 0x00, 0x00, 0x05, 
                           OP_ICONST, 0x00, 0x00, 0x00, 0x01,
                           OP_ISUB,
                           OP_BRT,    0x00, 0x00, 0x00, 0x05,
                           OP_HALT };

    printf("%d\n", interpret(bytecode));
}
