#include "vm.h"
#include "codegenerator.h"

#include <cstdint>
#include <sys/mman.h>
#include <cstring>
#include <cstdio>

#include <gtest/gtest.h>

using namespace std;

#define EXEC_AND_COMPARE(expected, code) \
    do { \
        void *mem = mmap(nullptr, code.size(), PROT_WRITE | PROT_EXEC,  \
                MAP_ANON | MAP_PRIVATE, -1, 0); \
        memcpy(mem, code.data(), code.size());  \
        int (*func)() = (int (*)())mem; \
        int actual = func();   \
        ASSERT_EQ(expected, actual);   \
        munmap(mem, code.size());   \
    } while (0)

TEST(CodeGenerator, AddTwoConstants) {
    CodeGenerator masm;

    masm.push(Imm32(3));
    masm.push(Imm32(5));
    masm.pop(rax);
    masm.pop(rbx);
    masm.add(rax, rbx);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(8, code);
}

TEST(CodeGenerator, SubTwoConstants) {
    CodeGenerator masm;

    masm.push(Imm32(17));
    masm.push(Imm32(11));
    masm.pop(rbx);
    masm.pop(rax);
    masm.sub(rax, rbx);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(6, code);
}

TEST(CodeGenerator, RemainderOfTwoConstants) {
    CodeGenerator masm;

    masm.push(Imm32(5));
    masm.push(Imm32(3));
    masm.pop(rbx);
    masm.pop(rax);
    masm.cqo();
    masm.idiv(rbx);
    masm.mov(rax, rdx);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(2, code);
}

TEST(CodeGenerator, JmpForward) {
    CodeGenerator masm;

    masm.push(Imm32(1));
    masm.jmp(Imm32(2));
    masm.pop(rax);
    masm.ret();
    masm.pop(rax);
    masm.push(Imm32(2));
    masm.pop(rax);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(2, code);
}

TEST(CodeGenerator, JumpForward) {
    CodeGenerator masm;

    masm.push(Imm32(1));
    Label label;
    masm.jump(label);

    masm.pop(rax);
    masm.ret();

    masm.bind(label);
    masm.pop(rax);
    masm.push(Imm32(2));
    masm.pop(rax);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(2, code);
}

TEST(CodeGenerator, JumpForwardAndBackward) {
    CodeGenerator masm;

    masm.push(Imm32(1));
    Label forward;
    masm.jump(forward);

    Label back;
    masm.bind(back);
    masm.pop(rax);
    masm.ret();

    masm.bind(forward);
    masm.push(Imm32(1));
    masm.pop(rax);
    masm.pop(rbx);
    masm.add(rax, rbx);
    masm.push(rax);
    masm.jump(back);

    masm.push(Imm32(1));
    masm.pop(rax);
    masm.pop(rbx);
    masm.add(rax, rbx);
    masm.push(rax);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(2, code);
}

TEST(CodeGenerator, CmpSetEqual_InputEqual) {
    CodeGenerator masm;

    masm.mov(rax, 42);
    masm.mov(rbx, 42);
    masm.cmpSet(Equal, rax, rbx, rax);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(1, code);
}

TEST(CodeGenerator, CmpSetEqual_InputLessThan) {
    CodeGenerator masm;

    masm.mov(rax, 41);
    masm.mov(rbx, 42);
    masm.cmpSet(Equal, rax, rbx, rax);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(0, code);
}

TEST(CodeGenerator, CmpSetLessThan_InputLessThan) {
    CodeGenerator masm;

    masm.mov(rax, 41);
    masm.mov(rbx, 42);
    masm.cmpSet(LessThan, rax, rbx, rax);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(1, code);
}

TEST(CodeGenerator, CmpSetLessThan_InputEqual) {
    CodeGenerator masm;

    masm.mov(rax, 42);
    masm.mov(rbx, 42);
    masm.cmpSet(LessThan, rax, rbx, rax);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(0, code);
}

TEST(CodeGenerator, MovWithDisplacement_LoadTwoStackLocations) {
    CodeGenerator masm;

    // Prologue
    masm.push(rbx);
    masm.push(rbp);
    masm.mov(rbp, rsp);

    masm.push(Imm32(1));
    masm.push(Imm32(2));

    // Move with displacement is not yet supported for rsp,
    // use another reg instead.
    masm.mov(rcx, rsp);
    masm.mov(rax, rcx, 0);
    masm.mov(rbx, rcx, 8);
    masm.add(rax, rbx);

    // Epilogue
    masm.mov(rsp, rbp);
    masm.pop(rbp);
    masm.pop(rbx);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(3, code);
}

TEST(CodeGenerator, MovWithDisplacement_StoreTwoStackLocations) {
    CodeGenerator masm;

    // Prologue
    masm.push(rbx);
    masm.push(rbp);
    masm.mov(rbp, rsp);
    masm.sub(rsp, Imm32(2 * 8)); // TODO(dannas): Replace '8' with worrdsize constant

    masm.mov(rax, 1);
    masm.mov(rbx, 2);

    masm.mov(rbp, -8, rax);
    masm.mov(rbp, -16, rbx);

    masm.pop(rcx);
    masm.pop(rdx);
    masm.add(rcx, rdx);
    masm.mov(rax, rcx);

    // Epilogue
    masm.mov(rsp, rbp);
    masm.pop(rbp);
    masm.pop(rbx);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(3, code);
}
int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
