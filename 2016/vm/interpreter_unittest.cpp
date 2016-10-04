#include "vm.h"

#include <gtest/gtest.h>

using namespace std;

#define INTERPRET_AND_COMPARE(code, expected) \
    do { \
        int32_t globals[] = {}; \
        stringstream ss; \
        interpret(code, globals, ss); \
        ss.seekg(0); \
        string actual = ss.str(); \
        ASSERT_EQ(expected, actual); \
    } while (0)


TEST(Interpreter, halt) {
    int32_t code[] = { OP_HALT };
    string expected = "";

    INTERPRET_AND_COMPARE(code, expected);
}

TEST(Interpreter, add) {
    int32_t code[] = {
        OP_ICONST, 1,
        OP_ICONST, 1,
        OP_IADD,
        OP_PRINT,
        OP_HALT
    };
    string expected = "2\n";

    INTERPRET_AND_COMPARE(code, expected);
}

TEST(Interpreter, jump) {
    int32_t code[] = {
        OP_ICONST, 3,
        OP_ICONST, 1,
        OP_ISUB,
        OP_BRT, 2,
        OP_HALT
    };
    string expected = "";

    INTERPRET_AND_COMPARE(code, expected);
}

TEST(Interpreter, callZeroParams) {
    int32_t code[] = {
        OP_CALL, 6, 0, 0,
        OP_PRINT,
        OP_HALT,
        OP_ICONST, 42,
        OP_RET
    };
    string expected = "42\n";

    INTERPRET_AND_COMPARE(code, expected);
}

TEST(Interpreter, callOneParam) {
    int32_t code[] = {
        OP_ICONST, 1,
        OP_CALL, 8, 1, 0,
        OP_PRINT,
        OP_HALT,
        OP_LOAD, 0,
        OP_RET
    };
    string expected = "1\n";

    INTERPRET_AND_COMPARE(code, expected);
}

TEST(Interpreter, callTwoParams) {
    int32_t code[] = {
        OP_ICONST, 1,
        OP_ICONST, 2,
        OP_CALL, 10, 2, 0,
        OP_PRINT,
        OP_HALT,
        OP_LOAD, 0,
        OP_LOAD, 1,
        OP_IADD,
        OP_RET
    };
    string expected = "3\n";

    INTERPRET_AND_COMPARE(code, expected);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
