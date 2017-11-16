#include "vm.h"
#include "codegenerator.h"

#include <cassert>
#include <map>

#define CASE break;case
#define UNKNOWN_OPCODE() assert(false && "unknown opcode")

using namespace std;
using BytecodePos = int32_t;

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
    masm.push(rbp);
    // TODO(dannas): Shouldn't this be the other way around?
    masm.mov(rbp, rsp);
    masm.push(rbx);
}

static void emitEpilogue(CodeGenerator& masm) {
    masm.pop(rbx);
    masm.pop(rbp);
    masm.ret();
}

MachineCode compile(const Bytecode &code) {
    CodeGenerator masm;
    map<BytecodePos, Label> labels;

    emitPrologue(masm);

    size_t pos = 0;

    while (pos < code.size()) {
        OpCode op = static_cast<OpCode>(code[pos]);
        Imm32 imm;
        BytecodePos addr;

        pos++; // Move to next opcode or operand

        switch (op) {
        CASE OP_POP:
            UNKNOWN_OPCODE();
        CASE OP_IADD:
            masm.pop(rax);
            masm.pop(rbx);
            masm.add(rax, rbx);
            masm.push(rax);
        CASE OP_ISUB:
            masm.pop(rbx);
            masm.pop(rax);
            masm.sub(rax, rbx);
            masm.push(rax);
        CASE OP_IMULT:
            masm.pop(rbx);
            masm.pop(rax);
            masm.imul(rbx);
            masm.push(rax);
        CASE OP_IDIV:
            masm.pop(rbx);
            masm.pop(rax);
            masm.cqo();
            masm.idiv(rbx);
            masm.push(rax);
        CASE OP_IMOD:
            masm.pop(rbx);
            masm.pop(rax);
            masm.cqo();
            masm.idiv(rbx);
            masm.push(rdx);
        CASE OP_ICONST:
            imm = code[pos++];
            masm.push(imm);
        CASE OP_LABEL:
            masm.bind(labels[pos]);
        CASE OP_ILT:
            masm.pop(rbx);
            masm.pop(rax);
            masm.cmpSet(LessThan, rax, rbx, rax);
            masm.push(rax);
        CASE OP_IEQ:
            masm.pop(rbx);
            masm.pop(rax);
            masm.cmpSet(Equal, rax, rbx, rax);
            masm.push(rax);
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
            masm.pop(rax);
            masm.ret();
        CASE OP_HALT:
            // TODO(dannas): Consider adding a jmp here for another location
            // where emitEpilogue is placed.
            masm.pop(rax);
            emitEpilogue(masm);
            break;
        default:
            UNKNOWN_OPCODE();
        }
    }

    return masm.buf();
}

