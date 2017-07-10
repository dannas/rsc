#include <gtest/gtest.h>
#include <cstring>

#include "string.h"

TEST(String, construct_EmptyShouldHaveSizeZero) {
    danstd::string s;
    ASSERT_EQ(0, s.size());
}

TEST(String, construct_AssignFromExistingString) {
    danstd::string s;
    char input[] = "hello world";
    s = input;
    ASSERT_EQ(strlen(input), s.size());
    ASSERT_STREQ(input, s.data());
    ASSERT_LE(strlen(input), s.capacity());
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

TEST(String, operatorBrackets_ThreeCharsString) {
    danstd::string s("abc");
    ASSERT_EQ('a', s[0]);
    ASSERT_EQ('b', s[1]);
    ASSERT_EQ('c', s[2]);
}

TEST(String, operatorBrackets_ThreeCharsConstString) {
    const danstd::string s("abc");
    ASSERT_EQ('a', s[0]);
    ASSERT_EQ('b', s[1]);
    ASSERT_EQ('c', s[2]);
}

TEST(String, operatorBinaryPlus_AddTwoThreeCharStrings) {
    danstd::string a = "abc";
    danstd::string b = "def";
    danstd::string s = a + b;
    ASSERT_STREQ("abcdef", s.data());
}

TEST(String, operatorBinaryPlusEquals_AddTwoThreeCharStrings) {
    danstd::string s = "abc";
    danstd::string a = "def";
    s += a;
    ASSERT_STREQ("abcdef", s.data());
}

TEST(String, operatorBinaryPlusEquals_AppendCharString) {
    danstd::string s = "abc";
    s += "def";
    ASSERT_STREQ("abcdef", s.data());
}

TEST(String, operatorBinaryPlus_AddCharString) {
    danstd::string s;
    danstd::string b = "def";
    s = "abc" + b;
    ASSERT_STREQ("abcdef", s.data());
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
