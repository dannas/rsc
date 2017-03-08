#include "algorithm.h"
#include <vector>

#include <gtest/gtest.h>

using std::vector;
using namespace counterfeit;

TEST(Algorithm, find_MatchAtStart) {
    vector<int> v = {0, 1, 2, 3, 4};
    auto i = find(begin(v), end(v), 0);
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, find_if_MatchExists) {
    vector<int> v = {0, 1, 2, 3, 4};
    auto i = find_if(begin(v), end(v), [] (int x) { return x == 3;});
    ASSERT_EQ(i, begin(v) + 3);
}

TEST(Algorithm, find_if_NoMatch) {
    vector<int> v = {0, 1, 2, 3, 4};
    auto i = find_if(begin(v), end(v), [] (int x) { return x == -1;});
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, find_if_ContainerEmpty) {
    vector<int> v;
    auto i = find_if(begin(v), end(v), [] (int x) { return x == 3;});
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, find_if_not_MatchExists) {
    vector<int> v = {0, 1, 2, 3, 4};
    auto i = find_if_not(begin(v), end(v), [] (int x) { return x == 3;});
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, find_if_not_NoMatch) {
    vector<int> v = {1, 1, 1, 1, 1};
    auto i = find_if_not(begin(v), end(v), [] (int x) { return x == 1;});
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, find_if_not_ContainerEmpty) {
    vector<int> v;
    auto i = find_if_not(begin(v), end(v), [] (int x) { return x == 3;});
    ASSERT_EQ(i, end(v));
}

// TODO(dannas): Test the rest of the non-modifying algorithms

TEST(Algorithm, mismatch_MismatchExists) {
    vector<int> v1 = {0, 1, 2};
    vector<int> v2 = {0, 1, 0};
    auto p = mismatch(begin(v1), end(v1), begin(v2));
    ASSERT_EQ(p.first, begin(v1) + 2);
    ASSERT_EQ(p.second, begin(v2) + 2);
}

TEST(Algorithm, mismatch_NoMismatch) {
    vector<int> v1 = {0, 1, 2};
    vector<int> v2 = {0, 1, 2};
    auto p = mismatch(begin(v1), end(v1), begin(v2));
    ASSERT_EQ(p.first, end(v1));
    ASSERT_EQ(p.second, end(v2));
}

TEST(Algorithm, equal_Equal) {
    vector<int> v1 = {0, 1, 2};
    vector<int> v2 = {0, 1, 2};
    bool ret = counterfeit::equal(begin(v1), end(v1), begin(v2));
    ASSERT_TRUE(ret);
}

TEST(Algorithm, search_MatchAtStart) {
    vector<int> v = {0, 1, 2, 3, 4};
    vector<int> s = {0, 1, 2};
    auto i = search(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, search_MatchAtEnd) {
    vector<int> v = {0, 0, 0, 1, 2};
    vector<int> s = {0, 1, 2};
    auto i = search(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v) + 2);
}

TEST(Algorithm, find_end_MatchAtStart) {
    vector<int> v = {1, 1, 1, 0, 0};
    vector<int> s = {1, 1, 1};
    auto i = find_end(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, find_end_MatchAtEnd) {
    vector<int> v = {1, 1, 0, 0, 1, 1};
    vector<int> s = {1, 1 };
    auto i = find_end(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v) + 4);
}

TEST(Algorithm, find_first_of_MatchAtStart) {
    vector<int> v = {1, 1, 1, 1};
    vector<int> s = {1, 2, 3, 4};
    auto i = find_first_of(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, find_first_of_MatchAtEnd) {
    vector<int> v = {0, 0, 0, 1};
    vector<int> s = {1, 2, 3, 4};
    auto i = find_first_of(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v) + 3);
}

TEST(Algorithm, adjacent_find_MatchAtStart) {
    vector<int> v = {0, 0, 1, 1};
    auto i = adjacent_find(begin(v), end(v));
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, adjacent_find_MatchAtMiddle) {
    vector<int> v = {1, 2, 2, 3};
    auto i = adjacent_find(begin(v), end(v));
    ASSERT_EQ(i, begin(v) + 1);
}

TEST(Algorithm, adjacent_find_MatchAtEnd) {
    vector<int> v = {1, 2, 3, 3};
    auto i = adjacent_find(begin(v), end(v));
    ASSERT_EQ(i, begin(v) + 2);
}

TEST(Algorithm, adjacent_find_NoMatch) {
    vector<int> v = {1, 2, 3, 4};
    auto i = adjacent_find(begin(v), end(v));
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, search_n_MatchAtStart) {
    vector<int> v = {1, 1, 1, 2, 3};
    auto i = search_n(begin(v), end(v), 3, 1);
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, search_n_MatchAtEnd) {
    vector<int> v = {1, 1, 1, 2, 2};
    auto i = search_n(begin(v), end(v), 2, 2);
    ASSERT_EQ(i, begin(v) + 3);
}

TEST(Algorithm, search_n_NoMatch) {
    vector<int> v = {1, 1, 1, 2, 2};
    auto i = search_n(begin(v), end(v), 2, 3);
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, copy_Empty) {
    vector<int> src;
    vector<int> dst;
    auto i = counterfeit::copy(begin(src), end(src), begin(dst));
    ASSERT_TRUE(dst.empty());
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_All) {
    vector<int> src = {1, 2, 3, 4};
    vector<int> dst(src.size());
    auto i = counterfeit::copy(begin(src), end(src), begin(dst));
    ASSERT_EQ(src, dst);
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_n_Empty) {
    vector<int> src;
    vector<int> dst;
    auto i = counterfeit::copy_n(begin(src), end(src), 4, begin(dst));
    ASSERT_TRUE(dst.empty());
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_n_All) {
    vector<int> src = {1, 2, 3, 4};
    vector<int> dst(src.size());
    auto i = counterfeit::copy_n(begin(src), end(src), 4, begin(dst));
    ASSERT_EQ(src, dst);
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_backward_Empty) {
    vector<int> src;
    vector<int> dst;
    auto i = counterfeit::copy_backward(begin(src), end(src), begin(dst));
    ASSERT_TRUE(dst.empty());
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_backward_All) {
    vector<int> src = {1, 2, 3, 4};
    vector<int> dst(src.size());
    auto i = counterfeit::copy_backward(begin(src), end(src), end(dst));
    ASSERT_EQ(src, dst);
    ASSERT_EQ(i, begin(dst));
}

class NonCopyable {
public:
    NonCopyable() : x_(0) {}
    NonCopyable(int x) : x_(x) {}
    NonCopyable(NonCopyable&& other) {
        x_ = std::move(other.x_);
    }
    NonCopyable& operator=(NonCopyable&& other) {
        x_ = std::move(other.x_);
        return *this;
    }
    bool operator==(const NonCopyable& other) const {
        return other.x_ == x_;
    }
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
private:
    int x_;
};

TEST(Algorithm, move_Empty) {
    vector<NonCopyable> src;
    vector<NonCopyable> dst;
    counterfeit::move(begin(src), end(src), begin(dst));
    ASSERT_TRUE(dst.empty());
}

TEST(Algorithm, move_All) {
    vector<NonCopyable> src;
    src.emplace_back(1);
    src.emplace_back(2);
    vector<NonCopyable> dst(src.size());
    auto i = counterfeit::move(begin(src), end(src), begin(dst));
    ASSERT_EQ(i, end(dst));
    ASSERT_EQ(src, dst);
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
