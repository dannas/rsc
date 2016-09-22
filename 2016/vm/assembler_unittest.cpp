#include "assembler.h"
#include "opcodes.h"

#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

using namespace std;

#define ASSEMBLE_AND_COMPARE(buf, expected) \
    do { \
        stringstream ss(buf); \
        auto actual = assemble(ss); \
        ASSERT_EQ(actual, expected); \
    } while (0)


TEST(Assembler, add) {
    char buf[] =
        "iconst 1"   "\n"
        "iconst 2"   "\n"
        "iadd"       "\n"
        "print"      "\n"
        "halt"       "\n";

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
    char buf[] =
        "iconst 1"   "\n"
        "brt .end"   "\n"
        ".end"       "\n"
        "halt"       "\n";

    vector<int32_t> expected = {
        OP_ICONST, 1,
        OP_BRT, 4,
        OP_HALT
    };

    ASSEMBLE_AND_COMPARE(buf, expected);
}

TEST(Assembler, function) {
    char buf[] =
        "call .f"                  "\n"
        "halt"                     "\n"
        ".def .f args=0, locals=0" "\n"
        "ret"                      "\n";

    vector<int32_t> expected = {
        OP_CALL, 3,
        OP_HALT,
        OP_LOAD, 0,
        OP_RET,
    };

    ASSEMBLE_AND_COMPARE(buf, expected);
}

TEST(Assembler, comment) {
    char buf[] =
        "halt ; This is a comment" "\n";
    vector<int32_t> expected = {
        OP_HALT,
    };

    ASSEMBLE_AND_COMPARE(buf, expected);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
