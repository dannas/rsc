#include <gtest/gtest.h>

#include "util.h"

TEST(UtilTest, ChiSquare) {
    int observed[5] = {0, 0, 0, 0, 0}; 
    int expected[5] = {0, 0, 0, 0, 0};
    EXPECT_EQ(chisquare(observed, expected), 0);
}

TEST(UtilTest, EscapeString) {
    ByteVector bytes = { 0x00, 0x61, 0x62, 0x63, 0x19 };
    std::string expected = "\\x00abc\\x19";
    EXPECT_EQ(escapeString(bytes), expected);
}

TEST(UtilTest, Transpose) {
    ByteVector bytes = { 
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    ByteVector expected = {1, 4, 7};
    auto actual = transpose(bytes, 3);

    EXPECT_EQ(actual[0], expected);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
