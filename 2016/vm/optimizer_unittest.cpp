#include "vm.h"

#include <gtest/gtest.h>

using namespace std;

TEST(FoldConstants, add) {
    vector<int32_t> code = {
        OP_ICONST, 1,
        OP_ICONST, 2,
        OP_IADD,
        OP_PRINT,
        OP_HALT

    };

    vector<int32_t> expected = {
        OP_ICONST, 3,
        OP_PRINT,
        OP_HALT
    };

    auto actual = foldConstants(code);
    ASSERT_EQ(expected, actual);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
