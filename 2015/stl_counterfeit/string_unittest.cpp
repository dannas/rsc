#include <gtest/gtest.h>
#include <cstring>

#include "string.h"

TEST(String, construct_EmptyShouldHaveSizeZero) {
    danstd::string s;
    ASSERT_EQ(0, s.size());
}

TEST(String, construct_AssignFromExistingString) {
    danstd::string s;
    //char input[] = "hello world";
    char input[] = "h";
    s = input;
    //ASSERT_EQ(strlen(input), s.size());
    ASSERT_STREQ(input, s.data());
    //ASSERT_LE(strlen(input), s.capacity());
}

TEST(String, reserve_ReallocateForLargerNewCap) {
    danstd::string s;
    ASSERT_EQ(0, s.size());
    size_t oldCap = s.capacity();
    s.reserve(oldCap + 1);
    ASSERT_LT(oldCap, s.capacity());
}

TEST(String, reserve_DoNothingForSmallerNewCap) {
    danstd::string s;
    ASSERT_EQ(0, s.size());
    s.reserve(16);
    ASSERT_LE(16, s.capacity());
    size_t oldCap = s.capacity();
    s.reserve(oldCap - 1);
    ASSERT_EQ(oldCap, s.capacity());
}

TEST(String, push_back_AppendOneCharToEmptyString) {
    danstd::string s;
    ASSERT_EQ(0, s.size());
    ASSERT_STREQ("", s.data());
    s.push_back('x');
    ASSERT_STREQ("x", s.data());
    s.push_back('y');
    ASSERT_STREQ("xy", s.data());
    s.push_back('z');
    ASSERT_STREQ("xyz", s.data());
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
