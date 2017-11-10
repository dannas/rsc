#include "vm.h"
#include "codegenerator.h"

#include <cassert>
#include <vector>

#define CASE break;case

using namespace std;

// Discussion of prologues/epilogues for JITs
// ### https://nickdesaulniers.github.io/blog/2015/05/25/interpreter-compiler-jit/
// ### https://nickdesaulniers.github.io/blog/2014/04/18/lets-write-some-x86-64/

static void emitPrologue(CodeGenerator& masm) {
    // The callee-saved registers on x64 are
    // r12-r15, rbx, rsp, rbp 
    // We currently don't use registers beyond the first 8, so
    // ignore r12-r15.
    // We rely on the code being run with -fno-omit-framepointer
    // so rbp will be used as frame pointer inside and outside the jit.
    // TODO(dannas): Push parameters from rdi, rsi, rdx, rcx, r8, r9 to stack,
    // to allow passing in parameters to the JITted code.
    masm.push(RBP);
    masm.mov(RSP, RBP);
    masm.push(RBX);
}

static void emitEpilogue(CodeGenerator& masm) {
    masm.pop(RBX);
    masm.pop(RBP);
    masm.ret();
}

vector<uint8_t> compile(const vector<int32_t>& code) {
    CodeGenerator masm;

    // TODO(dannas): Record destination offsets for the labels.
    // BR instructions has an absolute address operand.
    // We want relative offsets for the generated asm code.
    //
    // Scan through the bytecode
    // Record target addresses for BR instructions
    // Do the regular loop
    // When we reach one of the addresses, record the current asm pos
    //
    // That works for back branches, What about forward branches?

    emitPrologue(masm);

    size_t ip = 0;

    while (ip < code.size()) {
        OpCode op = static_cast<OpCode>(code[ip]);
        int32_t x, addr;

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
        CASE OP_LABEL:
            assert(false && "unhandled opcode");
        CASE OP_ILT:
            assert(false && "unhandled opcode");
        CASE OP_IEQ:
            assert(false && "unhandled opcode");
        CASE OP_BR:
            addr = code[ip++];
            masm.jmp(Imm32(addr));
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
            masm.pop(RAX);
            masm.ret();
        CASE OP_HALT:
            masm.pop(RAX);
            // TODO(dannas): How should ret and halt interact?
            break;
        default:
            assert(false && "unknown opcode");
        }
    }

    emitEpilogue(masm);

    return masm.buf();
}

