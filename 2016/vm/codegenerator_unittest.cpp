#include "vm.h"
#include "codegenerator.h"

#include <gtest/gtest.h>

using namespace std;

TEST(CodeGenerator, AddTwoConstants) {
    CodeGenerator masm;

#if 0
    masm.mov(RAX, RSI);
    masm.push(RAX);
    masm.pop(RAX);
    masm.cqo();
    masm.idiv(RDI);
    masm.ret();

    masm.push(RDI);
    masm.push(RSI);
#endif
    masm.push(Imm32(3));
    masm.push(Imm32(5));
    masm.pop(RAX);
    masm.pop(RBX);
    masm.add(RAX, RBX);
    masm.ret();

    // rwx protection per page boundary, hence can't use malloc allocated memory
    // which may span pages in unpredictable ways.
    // TODO(dannas): Introduce W^X via mprotect
    // TODO(dannas): Call munmap.
    void *mem = mmap(nullptr, masm.size(), PROT_WRITE | PROT_EXEC,
            MAP_ANON | MAP_PRIVATE, -1, 0);

    memcpy(mem, masm.data(), masm.size());

    int (*func)() = (int (*)())mem;

    int sum = func();
    ASSERT_EQ(8, sum);
    munmap(mem, masm.size());
}

TEST(CodeGenerator, SubTwoConstants) {
    CodeGenerator masm;

    masm.push(Imm32(17));
    masm.push(Imm32(11));
    masm.pop(RBX);
    masm.pop(RAX);
    masm.sub(RAX, RBX);
    masm.ret();

    // rwx protection per page boundary, hence can't use malloc allocated memory
    // which may span pages in unpredictable ways.
    // TODO(dannas): Introduce W^X via mprotect
    // TODO(dannas): Call munmap.
    void *mem = mmap(nullptr, masm.size(), PROT_WRITE | PROT_EXEC,
            MAP_ANON | MAP_PRIVATE, -1, 0);

    memcpy(mem, masm.data(), masm.size());

    int (*func)() = (int (*)())mem;

    int diff = func();
    ASSERT_EQ(6, diff);
    munmap(mem, masm.size());
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
