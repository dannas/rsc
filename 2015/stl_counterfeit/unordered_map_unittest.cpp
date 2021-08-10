#include <gtest/gtest.h>

#include "unordered_map.h"


TEST(UnorderedMap, Construct_EmptySize) {
    danstd::unordered_map<int> map;
    ASSERT_EQ(0, map.size());
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
