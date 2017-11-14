#include "vm.h"
#include "codegenerator.h"

#include <cassert>
#include <map>

#define CASE break;case
#define UNKNOWN_OPCODE() assert(false && "unknown opcode")

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
    // TODO(dannas): Shouldn't this be the other way around?
    masm.mov(RBP, RSP);
    masm.push(RBX);
}

static void emitEpilogue(CodeGenerator& masm) {
    masm.pop(RBX);
    masm.pop(RBP);
    masm.ret();
}

MachineCode compile(const Bytecode &code) {
    CodeGenerator masm;
    map<int32_t, Label> labels;

    emitPrologue(masm);

    size_t pos = 0;

    while (pos < code.size()) {
        OpCode op = static_cast<OpCode>(code[pos]);
        int32_t x, addr;

        pos++; // Move to next opcode or operand

        switch (op) {
        CASE OP_POP:
            UNKNOWN_OPCODE();
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
            x = code[pos++];
            masm.push(Imm32(x));
        CASE OP_LABEL:
            masm.bind(labels[pos]);
        CASE OP_ILT:
            // ### cmpSet(rax, rbx, Condition::Less)
            // ### push rax
            UNKNOWN_OPCODE();
        CASE OP_IEQ:
            // ### cmpSet(rax, rbx, Condition::Equal)
            // ### push rax
            UNKNOWN_OPCODE();
        CASE OP_BR:
            addr = code[pos++];
            masm.jump(labels[addr]);
        CASE OP_BRT:
            // ### pop rax
            // ### branch if rax is true
            UNKNOWN_OPCODE();
        CASE OP_LOAD:
            // ### mov rax, [rbp+index]
            // ### push rax
            UNKNOWN_OPCODE();
        CASE OP_STORE:
            // ### pop rax
            // ### mov [rbp+index], rax
            UNKNOWN_OPCODE();
        CASE OP_PRINT:
            UNKNOWN_OPCODE();
        CASE OP_CALL:
            UNKNOWN_OPCODE();
        CASE OP_RET:
            masm.pop(RAX);
            masm.ret();
        CASE OP_HALT:
            // TODO(dannas): Consider adding a jmp here for another location
            // where emitEpilogue is placed.
            masm.pop(RAX);
            emitEpilogue(masm);
            break;
        default:
            UNKNOWN_OPCODE();
        }
    }

    return masm.buf();
}

