#include "vm.h"

#include <gtest/gtest.h>

using namespace std;

TEST(Compiler, dummy) {
    ASSERT_TRUE(true);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
