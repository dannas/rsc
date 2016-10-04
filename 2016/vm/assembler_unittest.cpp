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

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
