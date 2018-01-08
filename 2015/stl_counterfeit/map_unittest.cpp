#include <gtest/gtest.h>
#include <utility>

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

TEST(Map, Iterator_BeginShouldReturnRootElement)  {
    danstd::map<int, int> m;
    m[0] = 1;
    auto it = m.begin();
    // TODO(dannas): How overload -> operator for iterator
    ASSERT_EQ(0, (*it).first);
    ASSERT_EQ(1, (*it).second);
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
