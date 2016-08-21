#include <gtest/gtest.h>

#include "util.h"

TEST(UtilTest, ChiSquare) {
    int observed[5] = {0, 0, 0, 0, 0}; 
    int expected[5] = {0, 0, 0, 0, 0};
    EXPECT_EQ(chisquare(observed, expected), 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
