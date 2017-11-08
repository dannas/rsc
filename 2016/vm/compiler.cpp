#include "vm.h"
#include "codegenerator.h"

#include <cassert>
#include <vector>

#define CASE break;case

using namespace std;

vector<uint8_t> compile(const vector<int32_t>& code) {
    CodeGenerator masm;
    // TODO(dannas): Add prologue for storing callee saved regs.
    // TODO(dannas): Add epilogue for restoring callee saved regs.
    // TODO(dannas): Convert rdi, rsi, rdx, rcx, r8, r9, xmm0-xmm7 to stack variables.

    size_t ip = 0;

    while (ip < code.size()) {
        OpCode op = static_cast<OpCode>(code[ip]);
        int32_t x;

        ip++; // Move to next opcode or operand

        switch (op) {
        CASE OP_POP:
            assert(false && "unhandled opcode");
        CASE OP_IADD:
            masm.pop(RAX);
            masm.pop(RBX);
            masm.add(RAX, RBX);
            masm.push(RAX);
        CASE OP_ISUB:
            masm.pop(RBX);
            masm.pop(RAX);
            masm.sub(RAX, RBX);
            masm.push(RAX);
        CASE OP_IMULT:
            masm.pop(RBX);
            masm.pop(RAX);
            masm.imul(RBX);
            masm.push(RAX);
        CASE OP_IDIV:
            masm.pop(RBX);
            masm.pop(RAX);
            masm.cqo();
            masm.idiv(RBX);
            masm.push(RAX);
        CASE OP_IMOD:
            masm.pop(RBX);
            masm.pop(RAX);
            masm.cqo();
            masm.idiv(RBX);
            masm.push(RDX);
        CASE OP_ICONST:
            x = code[ip++];
            masm.push(Imm32(x));
        CASE OP_ILT:
            assert(false && "unhandled opcode");
        CASE OP_IEQ:
            assert(false && "unhandled opcode");
        CASE OP_BR:
            assert(false && "unhandled opcode");
        CASE OP_BRT:
            assert(false && "unhandled opcode");
        CASE OP_LOAD:
            // ### mov rax, [rbp+index]
            // ### push rax
            assert(false && "unhandled opcode");
        CASE OP_STORE:
            // ### pop rax
            // ### mov [rbp+index], rax
            assert(false && "unhandled opcode");
        CASE OP_PRINT:
            assert(false && "unhandled opcode");
        CASE OP_CALL:
            assert(false && "unhandled opcode");
        CASE OP_RET:
            // TODO(dannas): Restore stack before exiting.
            masm.pop(RAX);
            masm.ret();
        CASE OP_HALT:
            // TODO(dannas): How should ret and halt interact?
            // ### pop rax
            // ### ret
            ;
            //assert(false && "unhandled opcode");
            break;
        default:
            assert(false && "unknown opcode");
        }
    }
    return masm.buf();
}

