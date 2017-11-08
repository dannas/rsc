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
    masm.pop(RAX);
    masm.pop(RBX);
    masm.add(RAX, RBX);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(8, code);
}

TEST(CodeGenerator, SubTwoConstants) {
    CodeGenerator masm;

    masm.push(Imm32(17));
    masm.push(Imm32(11));
    masm.pop(RBX);
    masm.pop(RAX);
    masm.sub(RAX, RBX);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(6, code);
}

TEST(CodeGenerator, RemainderOfTwoConstants) {
    CodeGenerator masm;

    masm.push(Imm32(5));
    masm.push(Imm32(3));
    masm.pop(RBX);
    masm.pop(RAX);
    masm.cqo();
    masm.idiv(RBX);
    masm.mov(RAX, RDX);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(2, code);
}

TEST(CodeGenerator, JumpAheadTwoInstructions) {
    CodeGenerator masm;

    masm.push(Imm32(1));
    masm.jmp(Imm32(2));
    masm.pop(RAX);
    masm.ret();
    masm.pop(RAX);
    masm.push(Imm32(2));
    masm.pop(RAX);
    masm.ret();

    auto code = masm.buf();

    EXEC_AND_COMPARE(2, code);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
