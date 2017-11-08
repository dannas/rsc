#include "vm.h"

#include <cstdint>
#include <sys/mman.h>
#include <cstring>
#include <cstdio>

#include <gtest/gtest.h>

using namespace std;

#define EXEC_AND_COMPARE(expected, buf) \
    do { \
        stringstream as(buf);   \
        auto bytecode = assemble(as);   \
        auto code = compile(bytecode);  \
        void *mem = mmap(nullptr, code.size(), PROT_WRITE | PROT_EXEC,  \
                MAP_ANON | MAP_PRIVATE, -1, 0); \
        memcpy(mem, code.data(), code.size());  \
        int (*func)() = (int (*)())mem; \
        int sum = func();   \
        ASSERT_EQ(expected, sum);   \
        munmap(mem, code.size());   \
    } while (0)

TEST(Compiler, AddTwoConstants) {
    char buf[] = R"(
        iconst 3
        iconst 5
        iadd
        ret
        halt
    )";
    EXEC_AND_COMPARE(8, buf);
}

TEST(Compiler, SubtractTwoConstants) {
    char buf[] = R"(
        iconst 5
        iconst 3
        isub
        ret
        halt
    )";
    EXEC_AND_COMPARE(2, buf);
}

TEST(Compiler, MultiplyTwoConstants) {
    char buf[] = R"(
        iconst 5
        iconst 3
        imult
        ret
        halt
    )";
    EXEC_AND_COMPARE(15, buf);
}

TEST(Compiler, DivideTwoConstants) {
    char buf[] = R"(
        iconst 24
        iconst 4
        idiv
        ret
        halt
    )";
    EXEC_AND_COMPARE(6, buf);
}

TEST(Compiler, RemainderOfTwoConstants) {
    char buf[] = R"(
        iconst 11
        iconst 7
        imod
        ret
        halt
    )";
    EXEC_AND_COMPARE(4, buf);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
