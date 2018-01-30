#include <gtest/gtest.h>
#include <utility>
#include <iostream> // TODO(dannas): Remove after testing

#include "map.h"

TEST(Map, Construct_ConstructAndDelete) {
    danstd::map<int, int> m; 
    m[1] = 42;
    ASSERT_EQ(42, m[1]);
    ASSERT_EQ(0, m[0]);
}

TEST(Map, Size_SizeShouldMatchInsertedElements) {
    danstd::map<int, int> m;
    ASSERT_EQ(0, m.size());
    m[2] = 3;
    ASSERT_EQ(1, m.size());
    m[1] = 2;
    ASSERT_EQ(2, m.size());
    m[3] = 5;
    ASSERT_EQ(3, m.size());
}

TEST(Map, Insert_InsertedElementsShouldBeRetrievable) {
    danstd::map<int, int> m;
    m[0] = 1;
    ASSERT_EQ(1, m[0]);
    m[1] = 3;
    ASSERT_EQ(1, m[0]);
    ASSERT_EQ(3, m[1]);
    m[2] = 5;
    ASSERT_EQ(1, m[0]);
    ASSERT_EQ(3, m[1]);
    ASSERT_EQ(5, m[2]);
    ASSERT_EQ(3, m.size());
}

TEST(Map, Iterator_EmptyMapShouldEqualEnd) {
    danstd::map<int, int> m;
    auto it = m.begin();
    ASSERT_EQ(it, m.end());
}
TEST(Map, Iterator_IncrementPastEnd) {
    danstd::map<int, int> m;
    auto it = m.begin();
    it++;
    ASSERT_EQ(it, m.end());
}

TEST(Map, Iterator_BeginShouldReturnSmallestElement)  {
    danstd::map<int, int> m;
    m[0] = 1;
    //  0

    auto it = m.begin();
    ASSERT_EQ(0, it->first);
    ASSERT_EQ(1, it->second);
    m[-2] = -3;
    it = m.begin();
    ASSERT_EQ(-2, it->first);
    ASSERT_EQ(-3, it->second);
    m[-1] = -5;
    ASSERT_EQ(-2, it->first);
    ASSERT_EQ(-3, it->second);
}

TEST(Map, Iterator_TraverseDecreasing) {
    danstd::map<int, int> m;
    m[3] = 3;
    m[2] = 2;
    m[1] = 1;
    auto it = m.begin();
    ASSERT_EQ(1, it->first);
    it++;
    ASSERT_EQ(2, it->first);
    it++;
    ASSERT_EQ(3, it->first);
}

TEST(Map, CopyConstructor_CopyEmpty) {
    danstd::map<int, int> origin;
    danstd::map<int, int> copy(origin);
    ASSERT_EQ(0, origin.size());
    ASSERT_EQ(0, copy.size());
}

TEST(Map, CopyConstructor_CopyOneElement) {
    danstd::map<int, int> origin;
    origin[1] = 2;
    danstd::map<int, int> copy(origin);
    ASSERT_EQ(1, origin.size());
    ASSERT_EQ(1, copy.size());
    ASSERT_EQ(2, origin[1]);
    ASSERT_EQ(2, copy[1]);
}

TEST(Map, CopyConstructor_CopySeveralElements) {
    danstd::map<int, int> origin;
    origin[1] = 2;
    origin[5] = 6;
    origin[3] = 4;
    origin[7] = 8;
    danstd::map<int, int> copy(origin);
    ASSERT_EQ(4, origin.size());
    ASSERT_EQ(4, copy.size());
    ASSERT_EQ(2, origin[1]);
    ASSERT_EQ(2, copy[1]);
    ASSERT_EQ(4, origin[3]);
    ASSERT_EQ(4, copy[3]);
    ASSERT_EQ(6, origin[5]);
    ASSERT_EQ(6, copy[5]);
    ASSERT_EQ(8, origin[7]);
    ASSERT_EQ(8, copy[7]);
}

TEST(Map, AssignmentOperator_CopySeveralElements) {
    danstd::map<int, int> origin;
    origin[1] = 2;
    origin[5] = 6;
    origin[3] = 4;
    origin[7] = 8;
    danstd::map<int, int> copy;
    copy = origin;
    ASSERT_EQ(4, origin.size());
    ASSERT_EQ(4, copy.size());
    ASSERT_EQ(2, origin[1]);
    ASSERT_EQ(2, copy[1]);
    ASSERT_EQ(4, origin[3]);
    ASSERT_EQ(4, copy[3]);
    ASSERT_EQ(6, origin[5]);
    ASSERT_EQ(6, copy[5]);
    ASSERT_EQ(8, origin[7]);
    ASSERT_EQ(8, copy[7]);
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
