#include "vm.h"

#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

using namespace std;

#define ASSEMBLE_AND_COMPARE(buf, expected) \
    do { \
        stringstream ss(buf); \
        auto actual = assemble(ss); \
        ASSERT_EQ(expected, actual); \
    } while (0)


TEST(Assembler, add) {
    char buf[] = R"(
        iconst 1
        iconst 2
        iadd
        print
        halt
    )";

    vector<int32_t> expected = {
        OP_ICONST, 1,
        OP_ICONST, 2,
        OP_IADD,
        OP_PRINT,
        OP_HALT
    };

    ASSEMBLE_AND_COMPARE(buf, expected);
}

TEST(Assembler, jump) {
    char buf[] = R"(
        iconst 1
        brt .end
        .end
        halt
    )";

    vector<int32_t> expected = {
        OP_ICONST, 1,
        OP_BRT, 4,
        OP_HALT
    };

    ASSEMBLE_AND_COMPARE(buf, expected);
}

TEST(Assembler, function) {
    char buf[] = R"(
        call .f
        halt
        .def .f args=0, locals=0
        ret
    )";

    vector<int32_t> expected = {
        OP_CALL, 5, 0, 0,
        OP_HALT,
        OP_RET,
    };

    ASSEMBLE_AND_COMPARE(buf, expected);
}

TEST(Assembler, comment) {
    char buf[] = R"(
        ; The first comment
        halt ; The second  comment
        ; The third comment
    )";
    vector<int32_t> expected = {
        OP_HALT,
    };

    ASSEMBLE_AND_COMPARE(buf, expected);
}

#define DISASSEMBLE_AND_COMPARE(buf, expected) \
    do { \
        stringstream ss; \
        ss.write(reinterpret_cast<char*>(buf.data()), buf.size() * sizeof(buf[0])); \
        ss.seekg(0); \
        auto actual = disassemble(ss); \
        ASSERT_EQ(expected, actual); \
    } while (0)


TEST(Disassembler, add) {
    vector<int32_t> buf = {
        OP_ICONST, 1,
        OP_ICONST, 2,
        OP_IADD,
        OP_PRINT,
        OP_HALT
    };
    string expected =
        "iconst 1\n"
        "iconst 2\n"
        "iadd\n"
        "print\n"
        "halt\n";
    DISASSEMBLE_AND_COMPARE(buf, expected);
}

TEST(Disassembler, addFunction) {
    vector<int32_t> buf = {
        OP_ICONST, 1,
        OP_ICONST, 2,
        OP_CALL, 7, 2, 1,
        OP_PRINT,
        OP_HALT,
        OP_LOAD, 0,
        OP_LOAD, 1,
        OP_IADD,
        OP_STORE, 2,    // just for exercising the store instruction
        OP_LOAD,  2,
        OP_RET
    };
    string expected =
        "iconst 1\n"
        "iconst 2\n"
        "call 7 2 1\n"
        "print\n"
        "halt\n"
        "load 0\n"
        "load 1\n"
        "iadd\n"
        "store 2\n"
        "load 2\n"
        "ret\n";

    DISASSEMBLE_AND_COMPARE(buf, expected);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
