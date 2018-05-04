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

#define EXEC_AND_COMPARE2(expected, buf) \
    do { \
        stringstream as(buf);   \
        auto bytecode = assemble(as);   \
        auto code = compile2(bytecode);  \
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
        halt
    )";
    EXEC_AND_COMPARE(8, buf);
    EXEC_AND_COMPARE2(8, buf);
}

TEST(Compiler, SubtractTwoConstants) {
    char buf[] = R"(
        iconst 5
        iconst 3
        isub
        halt
    )";
    EXEC_AND_COMPARE(2, buf);
    EXEC_AND_COMPARE2(2, buf);
}

TEST(Compiler, MultiplyTwoConstants) {
    char buf[] = R"(
        iconst 5
        iconst 3
        imult
        halt
    )";
    EXEC_AND_COMPARE(15, buf);
    EXEC_AND_COMPARE2(15, buf);
}

TEST(Compiler, DivideTwoConstants) {
    char buf[] = R"(
        iconst 24
        iconst 4
        idiv
        halt
    )";
    EXEC_AND_COMPARE(6, buf);
    EXEC_AND_COMPARE2(6, buf);
}

TEST(Compiler, RemainderOfTwoConstants) {
    char buf[] = R"(
        iconst 11
        iconst 7
        imod
        halt
    )";
    EXEC_AND_COMPARE(4, buf);
    EXEC_AND_COMPARE2(4, buf);
}

TEST(Compiler, ForwardBranch) {
    char buf[] = R"(
        iconst 1
        br .end
        halt
    .end
        iconst 1
        iadd
        halt
    )";
    EXEC_AND_COMPARE(2, buf);
}

TEST(Compiler, ForwarAndBackwarddBranch) {
    char buf[] = R"(
        iconst 1
        br .forward
    .back
        iconst 1
        iadd
        halt
    .forward
        iconst 1
        iadd
        br .back
        halt
    )";
    EXEC_AND_COMPARE(3, buf);
}

TEST(Compiler, CompareForLessThan_WhenLess) {
    char buf[] = R"(
        iconst 1
        iconst 2
        ilt
        halt
    )";
    EXEC_AND_COMPARE(1, buf);
}

TEST(Compiler, CompareForLessThan_WhenEqual) {
    char buf[] = R"(
        iconst 2
        iconst 2
        ilt
        halt
    )";
    EXEC_AND_COMPARE(0, buf);
}

TEST(Compiler, CompareForLessThan_WhenGreater) {
    char buf[] = R"(
        iconst 2
        iconst 1
        ilt
        halt
    )";
    EXEC_AND_COMPARE(0, buf);
}

TEST(Compiler, CompareForEqual_WhenEqual) {
    char buf[] = R"(
        iconst 1
        iconst 1
        ieq
        halt
    )";
    EXEC_AND_COMPARE(1, buf);
}

TEST(Compiler, CompareForEqual_WhenLess) {
    char buf[] = R"(
        iconst 1
        iconst 2
        ieq
        halt
    )";
    EXEC_AND_COMPARE(0, buf);
}

TEST(Compiler, CompareForEqual_WhenGreater) {
    char buf[] = R"(
        iconst 2
        iconst 1
        ieq
        halt
    )";
    EXEC_AND_COMPARE(0, buf);
}

TEST(Compiler, JumpIfTrue_CompareForEqual) {
    char buf[] = R"(
        iconst 1
        iconst 1
        ieq
        brt .true
        iconst 42
        iadd
        halt
    .true
        iconst 2
        halt
    )";
    EXEC_AND_COMPARE(2, buf);
}

TEST(Compiler, Store_StoreTwoValues) {
    char buf[] = R"(
        call .f
        halt
    .def .f args=0, locals=2
        iconst 2
        store 0
        iconst 3
        store 1
        load 0
        load 1
        iadd
        ret
    )";
    EXEC_AND_COMPARE(5, buf);

}

TEST(Compiler, FunctionEmpty) {
    char buf[] = R"(
        call .f
        halt
    .def .f args=0, locals=0
        iconst 42
        ret
    )";
    EXEC_AND_COMPARE(42, buf);
}

TEST(Compiler, Function_AddTwoParameters) {
    char buf[] = R"(
        iconst 1
        iconst 2
        call .f
        halt
        .def .f args=2, locals=0
        load 0
        load 1
        iadd
        ret
    )";

    EXEC_AND_COMPARE(3, buf);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
