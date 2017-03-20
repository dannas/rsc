#include "algorithm.h"
#include <vector>

#include <gtest/gtest.h>

using namespace counterfeit;

using Vector = std::vector<int>;

TEST(Algorithm, find_MatchAtStart) {
    Vector v = {0, 1, 2, 3, 4};
    auto i = find(begin(v), end(v), 0);
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, find_if_MatchExists) {
    Vector v = {0, 1, 2, 3, 4};
    auto i = find_if(begin(v), end(v), [] (int x) { return x == 3;});
    ASSERT_EQ(i, begin(v) + 3);
}

TEST(Algorithm, find_if_NoMatch) {
    Vector v = {0, 1, 2, 3, 4};
    auto i = find_if(begin(v), end(v), [] (int x) { return x == -1;});
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, find_if_ContainerEmpty) {
    Vector v;
    auto i = find_if(begin(v), end(v), [] (int x) { return x == 3;});
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, find_if_not_MatchExists) {
    Vector v = {0, 1, 2, 3, 4};
    auto i = find_if_not(begin(v), end(v), [] (int x) { return x == 3;});
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, find_if_not_NoMatch) {
    Vector v = {1, 1, 1, 1, 1};
    auto i = find_if_not(begin(v), end(v), [] (int x) { return x == 1;});
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, find_if_not_ContainerEmpty) {
    Vector v;
    auto i = find_if_not(begin(v), end(v), [] (int x) { return x == 3;});
    ASSERT_EQ(i, end(v));
}

// TODO(dannas): Test the rest of the non-modifying algorithms

TEST(Algorithm, mismatch_MismatchExists) {
    Vector v1 = {0, 1, 2};
    Vector v2 = {0, 1, 0};
    auto p = mismatch(begin(v1), end(v1), begin(v2));
    ASSERT_EQ(p.first, begin(v1) + 2);
    ASSERT_EQ(p.second, begin(v2) + 2);
}

TEST(Algorithm, mismatch_NoMismatch) {
    Vector v1 = {0, 1, 2};
    Vector v2 = {0, 1, 2};
    auto p = mismatch(begin(v1), end(v1), begin(v2));
    ASSERT_EQ(p.first, end(v1));
    ASSERT_EQ(p.second, end(v2));
}

TEST(Algorithm, equal_Equal) {
    Vector v1 = {0, 1, 2};
    Vector v2 = {0, 1, 2};
    bool ret = counterfeit::equal(begin(v1), end(v1), begin(v2));
    ASSERT_TRUE(ret);
}

TEST(Algorithm, search_MatchAtStart) {
    Vector v = {0, 1, 2, 3, 4};
    Vector s = {0, 1, 2};
    auto i = search(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, search_MatchAtEnd) {
    Vector v = {0, 0, 0, 1, 2};
    Vector s = {0, 1, 2};
    auto i = search(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v) + 2);
}

TEST(Algorithm, find_end_MatchAtStart) {
    Vector v = {1, 1, 1, 0, 0};
    Vector s = {1, 1, 1};
    auto i = find_end(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, find_end_MatchAtEnd) {
    Vector v = {1, 1, 0, 0, 1, 1};
    Vector s = {1, 1 };
    auto i = find_end(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v) + 4);
}

TEST(Algorithm, find_first_of_MatchAtStart) {
    Vector v = {1, 1, 1, 1};
    Vector s = {1, 2, 3, 4};
    auto i = find_first_of(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, find_first_of_MatchAtEnd) {
    Vector v = {0, 0, 0, 1};
    Vector s = {1, 2, 3, 4};
    auto i = find_first_of(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v) + 3);
}

TEST(Algorithm, adjacent_find_MatchAtStart) {
    Vector v = {0, 0, 1, 1};
    auto i = adjacent_find(begin(v), end(v));
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, adjacent_find_MatchAtMiddle) {
    Vector v = {1, 2, 2, 3};
    auto i = adjacent_find(begin(v), end(v));
    ASSERT_EQ(i, begin(v) + 1);
}

TEST(Algorithm, adjacent_find_MatchAtEnd) {
    Vector v = {1, 2, 3, 3};
    auto i = adjacent_find(begin(v), end(v));
    ASSERT_EQ(i, begin(v) + 2);
}

TEST(Algorithm, adjacent_find_NoMatch) {
    Vector v = {1, 2, 3, 4};
    auto i = adjacent_find(begin(v), end(v));
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, search_n_MatchAtStart) {
    Vector v = {1, 1, 1, 2, 3};
    auto i = search_n(begin(v), end(v), 3, 1);
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, search_n_MatchAtEnd) {
    Vector v = {1, 1, 1, 2, 2};
    auto i = search_n(begin(v), end(v), 2, 2);
    ASSERT_EQ(i, begin(v) + 3);
}

TEST(Algorithm, search_n_NoMatch) {
    Vector v = {1, 1, 1, 2, 2};
    auto i = search_n(begin(v), end(v), 2, 3);
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, copy_Empty) {
    Vector src;
    Vector dst;
    auto i = counterfeit::copy(begin(src), end(src), begin(dst));
    ASSERT_TRUE(dst.empty());
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_All) {
    Vector src = {1, 2, 3, 4};
    Vector dst(src.size());
    auto i = counterfeit::copy(begin(src), end(src), begin(dst));
    ASSERT_EQ(src, dst);
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_n_Empty) {
    Vector src;
    Vector dst;
    auto i = counterfeit::copy_n(begin(src), end(src), 4, begin(dst));
    ASSERT_TRUE(dst.empty());
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_n_All) {
    Vector src = {1, 2, 3, 4};
    Vector dst(src.size());
    auto i = counterfeit::copy_n(begin(src), end(src), 4, begin(dst));
    ASSERT_EQ(src, dst);
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_backward_Empty) {
    Vector src;
    Vector dst;
    auto i = counterfeit::copy_backward(begin(src), end(src), begin(dst));
    ASSERT_TRUE(dst.empty());
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_backward_All) {
    Vector src = {1, 2, 3, 4};
    Vector dst(src.size());
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

    operator int() {
        return x_;
    }

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
private:
    int x_;
};

TEST(Algorithm, move_Empty) {
    std::vector<NonCopyable> src;
    std::vector<NonCopyable> dst;
    counterfeit::move(begin(src), end(src), begin(dst));
    ASSERT_TRUE(dst.empty());
}

TEST(Algorithm, move_All) {
    std::vector<NonCopyable> src;
    src.emplace_back(1);
    src.emplace_back(2);
    std::vector<NonCopyable> dst(src.size());
    auto i = counterfeit::move(begin(src), end(src), begin(dst));
    ASSERT_EQ(i, end(dst));
    ASSERT_EQ(src, dst);
 }

// TODO(dannas): Add tests for:
// move_backward
// fill
// fill_n
// transform
// generate
// generate_n
// remove_if
// remove_copy
// replace
// replace_copy

TEST(Algorithm, swap_NotCopyable) {
    int x = 1;
    int y = 2;
    NonCopyable nx(x);
    NonCopyable ny(y);
    swap(nx, ny);
    ASSERT_EQ(nx, y);
    ASSERT_EQ(ny, x);
}

// TODO(dannas): Add tests for
// iter_swap
// swap_ranges

TEST(Algorithm, reverse_Empty) {
    Vector v;
    reverse(begin(v), end(v));
    ASSERT_TRUE(v.empty());
}

TEST(Algorithm, reverse_OneElement) {
    Vector v = {1};
    Vector r = {1};
    reverse(begin(v), end(v));
    ASSERT_EQ(v, r);
}

TEST(Algorithm, reverse_TwoElements) {
    Vector v = {1, 2};
    Vector r = {2, 1};
    reverse(begin(v), end(v));
    ASSERT_EQ(v, r);
}


TEST(Algorithm, reverse_ThreeElements) {
    Vector v = {1, 2, 3};
    Vector r = {3, 2, 1};
    reverse(begin(v), end(v));
    ASSERT_EQ(v, r);
}

TEST(Algorithm, reverse_copy_OneElement) {
    Vector src = {1};
    Vector dst(src.size());
    Vector r = {1};
    auto i = reverse_copy(begin(src), end(src), begin(dst));
    ASSERT_EQ(i, end(dst));
    ASSERT_EQ(dst, r);
}

TEST(Algorithm, reverse_copy_TwoElements) {
    Vector src = {1, 2};
    Vector dst(src.size());
    Vector r = {2, 1};
    auto i = reverse_copy(begin(src), end(src), begin(dst));
    ASSERT_EQ(i, end(dst));
    ASSERT_EQ(dst, r);
}

TEST(Algorithm, reverse_copy_ThreeElements) {
    Vector src = {1, 2, 3};
    Vector dst(src.size());
    Vector r = {3, 2, 1};
    auto i = reverse_copy(begin(src), end(src), begin(dst));
    ASSERT_EQ(i, end(dst));
    ASSERT_EQ(dst, r);
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
