#include <gtest/gtest.h>
#include <utility>

#include "map.h"

TEST(Map, Construct_ConstructAndDelete) {
    danstd::map<int, int> m; 
    m[1] = 42;
    ASSERT_EQ(42, m[1]);
    ASSERT_EQ(0, m[0]);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
