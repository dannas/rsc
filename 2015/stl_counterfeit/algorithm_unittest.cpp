#include "algorithm.h"
#include <vector>

#include <gtest/gtest.h>

using namespace danstd;

using Vector = std::vector<int>;

TEST(Algorithm, find_MatchAtStart) {
    Vector v = {0, 1, 2, 3, 4};
    auto i = danstd::find(begin(v), end(v), 0);
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, find_if_MatchExists) {
    Vector v = {0, 1, 2, 3, 4};
    auto i = danstd::find_if(begin(v), end(v), [] (int x) { return x == 3;});
    ASSERT_EQ(i, begin(v) + 3);
}

TEST(Algorithm, find_if_NoMatch) {
    Vector v = {0, 1, 2, 3, 4};
    auto i = danstd::find_if(begin(v), end(v), [] (int x) { return x == -1;});
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, find_if_ContainerEmpty) {
    Vector v;
    auto i = danstd::find_if(begin(v), end(v), [] (int x) { return x == 3;});
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, find_if_not_MatchExists) {
    Vector v = {0, 1, 2, 3, 4};
    auto i = danstd::find_if_not(begin(v), end(v), [] (int x) { return x == 3;});
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, find_if_not_NoMatch) {
    Vector v = {1, 1, 1, 1, 1};
    auto i = danstd::find_if_not(begin(v), end(v), [] (int x) { return x == 1;});
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, find_if_not_ContainerEmpty) {
    Vector v;
    auto i = danstd::find_if_not(begin(v), end(v), [] (int x) { return x == 3;});
    ASSERT_EQ(i, end(v));
}

// TODO(dannas): Test the rest of the non-modifying algorithms

TEST(Algorithm, mismatch_MismatchExists) {
    Vector v1 = {0, 1, 2};
    Vector v2 = {0, 1, 0};
    auto p = danstd::mismatch(begin(v1), end(v1), begin(v2));
    ASSERT_EQ(p.first, begin(v1) + 2);
    ASSERT_EQ(p.second, begin(v2) + 2);
}

TEST(Algorithm, mismatch_NoMismatch) {
    Vector v1 = {0, 1, 2};
    Vector v2 = {0, 1, 2};
    auto p = danstd::mismatch(begin(v1), end(v1), begin(v2));
    ASSERT_EQ(p.first, end(v1));
    ASSERT_EQ(p.second, end(v2));
}

TEST(Algorithm, equal_Equal) {
    Vector v1 = {0, 1, 2};
    Vector v2 = {0, 1, 2};
    bool ret = danstd::equal(begin(v1), end(v1), begin(v2));
    ASSERT_TRUE(ret);
}

TEST(Algorithm, search_MatchAtStart) {
    Vector v = {0, 1, 2, 3, 4};
    Vector s = {0, 1, 2};
    auto i = danstd::search(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, search_MatchAtEnd) {
    Vector v = {0, 0, 0, 1, 2};
    Vector s = {0, 1, 2};
    auto i = danstd::search(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v) + 2);
}

TEST(Algorithm, find_end_MatchAtStart) {
    Vector v = {1, 1, 1, 0, 0};
    Vector s = {1, 1, 1};
    auto i = danstd::find_end(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, find_end_MatchAtEnd) {
    Vector v = {1, 1, 0, 0, 1, 1};
    Vector s = {1, 1 };
    auto i = danstd::find_end(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v) + 4);
}

TEST(Algorithm, find_first_of_MatchAtStart) {
    Vector v = {1, 1, 1, 1};
    Vector s = {1, 2, 3, 4};
    auto i = danstd::find_first_of(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, find_first_of_MatchAtEnd) {
    Vector v = {0, 0, 0, 1};
    Vector s = {1, 2, 3, 4};
    auto i = danstd::find_first_of(begin(v), end(v), begin(s), end(s));
    ASSERT_EQ(i, begin(v) + 3);
}

TEST(Algorithm, adjacent_find_MatchAtStart) {
    Vector v = {0, 0, 1, 1};
    auto i = danstd::adjacent_find(begin(v), end(v));
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, adjacent_find_MatchAtMiddle) {
    Vector v = {1, 2, 2, 3};
    auto i = danstd::adjacent_find(begin(v), end(v));
    ASSERT_EQ(i, begin(v) + 1);
}

TEST(Algorithm, adjacent_find_MatchAtEnd) {
    Vector v = {1, 2, 3, 3};
    auto i = danstd::adjacent_find(begin(v), end(v));
    ASSERT_EQ(i, begin(v) + 2);
}

TEST(Algorithm, adjacent_find_NoMatch) {
    Vector v = {1, 2, 3, 4};
    auto i = danstd::adjacent_find(begin(v), end(v));
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, search_n_MatchAtStart) {
    Vector v = {1, 1, 1, 2, 3};
    auto i = danstd::search_n(begin(v), end(v), 3, 1);
    ASSERT_EQ(i, begin(v));
}

TEST(Algorithm, search_n_MatchAtEnd) {
    Vector v = {1, 1, 1, 2, 2};
    auto i = danstd::search_n(begin(v), end(v), 2, 2);
    ASSERT_EQ(i, begin(v) + 3);
}

TEST(Algorithm, search_n_NoMatch) {
    Vector v = {1, 1, 1, 2, 2};
    auto i = danstd::search_n(begin(v), end(v), 2, 3);
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, copy_Empty) {
    Vector src;
    Vector dst;
    auto i = danstd::copy(begin(src), end(src), begin(dst));
    ASSERT_TRUE(dst.empty());
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_All) {
    Vector src = {1, 2, 3, 4};
    Vector dst(src.size());
    auto i = danstd::copy(begin(src), end(src), begin(dst));
    ASSERT_EQ(src, dst);
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_n_Empty) {
    Vector src;
    Vector dst;
    auto i = danstd::copy_n(begin(src), end(src), 4, begin(dst));
    ASSERT_TRUE(dst.empty());
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_n_All) {
    Vector src = {1, 2, 3, 4};
    Vector dst(src.size());
    auto i = danstd::copy_n(begin(src), end(src), 4, begin(dst));
    ASSERT_EQ(src, dst);
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_backward_Empty) {
    Vector src;
    Vector dst;
    auto i = danstd::copy_backward(begin(src), end(src), begin(dst));
    ASSERT_TRUE(dst.empty());
    ASSERT_EQ(i, end(dst));
}

TEST(Algorithm, copy_backward_All) {
    Vector src = {1, 2, 3, 4};
    Vector dst(src.size());
    auto i = danstd::copy_backward(begin(src), end(src), end(dst));
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
    danstd::move(begin(src), end(src), begin(dst));
    ASSERT_TRUE(dst.empty());
}

TEST(Algorithm, move_All) {
    std::vector<NonCopyable> src;
    src.emplace_back(1);
    src.emplace_back(2);
    std::vector<NonCopyable> dst(src.size());
    auto i = danstd::move(begin(src), end(src), begin(dst));
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
    danstd::swap(nx, ny);
    ASSERT_EQ(nx, y);
    ASSERT_EQ(ny, x);
}

// TODO(dannas): Add tests for
// iter_swap
// swap_ranges

TEST(Algorithm, reverse_Empty) {
    Vector v;
    danstd::reverse(begin(v), end(v));
    ASSERT_TRUE(v.empty());
}

TEST(Algorithm, reverse_OneElement) {
    Vector v = {1};
    Vector r = {1};
    danstd::reverse(begin(v), end(v));
    ASSERT_EQ(v, r);
}

TEST(Algorithm, reverse_TwoElements) {
    Vector v = {1, 2};
    Vector r = {2, 1};
    danstd::reverse(begin(v), end(v));
    ASSERT_EQ(v, r);
}


TEST(Algorithm, reverse_ThreeElements) {
    Vector v = {1, 2, 3};
    Vector r = {3, 2, 1};
    danstd::reverse(begin(v), end(v));
    ASSERT_EQ(v, r);
}

TEST(Algorithm, reverse_copy_OneElement) {
    Vector src = {1};
    Vector dst(src.size());
    Vector r = {1};
    auto i = danstd::reverse_copy(begin(src), end(src), begin(dst));
    ASSERT_EQ(i, end(dst));
    ASSERT_EQ(dst, r);
}

TEST(Algorithm, reverse_copy_TwoElements) {
    Vector src = {1, 2};
    Vector dst(src.size());
    Vector r = {2, 1};
    auto i = danstd::reverse_copy(begin(src), end(src), begin(dst));
    ASSERT_EQ(i, end(dst));
    ASSERT_EQ(dst, r);
}

TEST(Algorithm, reverse_copy_ThreeElements) {
    Vector src = {1, 2, 3};
    Vector dst(src.size());
    Vector r = {3, 2, 1};
    auto i = danstd::reverse_copy(begin(src), end(src), begin(dst));
    ASSERT_EQ(i, end(dst));
    ASSERT_EQ(dst, r);
}

TEST(Algorithm, rotate_Empty) {
    Vector v;
    danstd::rotate(begin(v), begin(v), end(v));
    ASSERT_TRUE(v.empty());
}

TEST(Algorithm, rotate_Even) {
    Vector v = {1, 2, 3, 4};
    Vector expected = {3, 4, 1, 2};
    danstd::rotate(begin(v), begin(v) + 2, end(v));
    ASSERT_EQ(expected, v);
}

TEST(Algorithm, rotate_FirstHalfLarger) {
    Vector v = {1, 2, 3, 4, 5};
    Vector expected = {4, 5, 1, 2, 3};
    danstd::rotate(begin(v), begin(v) + 3, end(v));
    ASSERT_EQ(expected, v);
}

TEST(Algorithm, rotate_SecondHalfLarger) {
    Vector v = {1, 2, 3, 4, 5};
    Vector expected = {3, 4, 5, 1, 2};
    danstd::rotate(begin(v), begin(v) + 2, end(v));
    ASSERT_EQ(expected, v);
}

TEST(Algorithm, unique_Empty) {
    Vector v;
    auto i = danstd::unique(begin(v), end(v));
    ASSERT_EQ(i, end(v));
    ASSERT_TRUE(v.empty());
}

TEST(Algorithm, unique_OneElement) {
    Vector v = {1};
    Vector expected = {1};
    auto i = danstd::unique(begin(v), end(v));
    ASSERT_EQ(i, end(v));
    ASSERT_EQ(v, expected);
}

TEST(Algorithm, unique_DuplicateAtBegin) {
    Vector v = {1, 1, 2};
    Vector expected = {1, 2};
    auto i = danstd::unique(begin(v), end(v));
    Vector actual(begin(v), i);
    ASSERT_EQ(i, begin(v)+2);
    ASSERT_EQ(actual, expected);
}

TEST(Algorithm, unique_DuplicateAtEnd) {
    Vector v = {1, 2, 2};
    Vector expected = {1, 2};
    auto i = danstd::unique(begin(v), end(v));
    Vector actual(begin(v), i);
    ASSERT_EQ(i, begin(v)+2);
    ASSERT_EQ(actual, expected);
}

TEST(Algorithm, unique_copy_Empty) {
    Vector src;
    Vector dst;
    auto i = danstd::unique_copy(begin(src), end(src), begin(dst));
    ASSERT_EQ(i, end(dst));
    ASSERT_TRUE(dst.empty());
}

TEST(Algorithm, unique_copy_OneElement) {
    Vector src = {1};
    Vector dst(src.size());
    Vector expected = {1};
    auto i = danstd::unique_copy(begin(src), end(src), begin(dst));
    ASSERT_EQ(i, end(dst));
    ASSERT_EQ(dst, expected);
}

TEST(Algorithm, unique_copy_DuplicateAtBegin) {
    Vector src = {1, 1, 2};
    Vector dst(src.size());
    Vector expected = {1, 2};
    auto i = danstd::unique_copy(begin(src), end(src), begin(dst));
    ASSERT_EQ(i, begin(dst)+2);
    Vector actual(begin(dst), i);
    ASSERT_EQ(actual, expected);
}

TEST(Algorithm, unique_copy_DuplicateAtEnd) {
    Vector src = {1, 2, 2};
    Vector dst(src.size());
    Vector expected = {1, 2};
    auto i = danstd::unique_copy(begin(src), end(src), begin(dst));
    ASSERT_EQ(i, begin(dst)+2);
    Vector actual(begin(dst), i);
    ASSERT_EQ(actual, expected);
}

struct IsZero {
    bool operator()(int x) {
        return x == 0;
    }
};

TEST(Algorithm, is_paritioned_empty) {
    Vector v;
    bool r = danstd::is_partitioned(begin(v), end(v), IsZero());
    ASSERT_TRUE(r);
}

TEST(Algorithm, is_partitioned_OnPartionedArray) {
    Vector v = {0, 1};
    bool r = danstd::is_partitioned(begin(v), end(v), IsZero());
    ASSERT_TRUE(r);
}

TEST(Algorithm, is_partitioned_OnNotPartionedArray) {
    Vector v = {1, 0, 1};
    bool r = danstd::is_partitioned(begin(v), end(v), IsZero());
    ASSERT_FALSE(r);
}

TEST(Algorithm, partition_copy_empty) {
    Vector v;
    Vector vTrue;
    Vector vFalse;

    auto i = danstd::partition_copy(begin(v), end(v), begin(vTrue), begin(vFalse), IsZero());
    ASSERT_EQ(i.first, end(vTrue));
    ASSERT_EQ(i.second, end(vFalse));
}

TEST(Algorithm, partition_copy_AlreadyPartitioned) {
    Vector v = {0, 0, 1, 1};
    Vector vTrue(2);
    Vector vFalse(2);

    Vector vTrueExpected = {0, 0};
    Vector vFalseExpected = {1, 1};

    auto i = danstd::partition_copy(begin(v), end(v), begin(vTrue), begin(vFalse), IsZero());
    ASSERT_EQ(i.first, end(vTrue));
    ASSERT_EQ(i.second, end(vFalse));
    ASSERT_EQ(vTrue, vTrueExpected);
    ASSERT_EQ(vFalse, vFalseExpected);
}

TEST(Algorithm, partition_copy_NotPartitioned) {
    Vector v = {0, 1, 0, 1};
    Vector vTrue(2);
    Vector vFalse(2);

    Vector vTrueExpected = {0, 0};
    Vector vFalseExpected = {1, 1};

    auto i = danstd::partition_copy(begin(v), end(v), begin(vTrue), begin(vFalse), IsZero());
    ASSERT_EQ(i.first, end(vTrue));
    ASSERT_EQ(i.second, end(vFalse));
    ASSERT_EQ(vTrue, vTrueExpected);
    ASSERT_EQ(vFalse, vFalseExpected);
}

TEST(Algorithm, partition_empty) {
    Vector v;
    auto i = danstd::partition(begin(v), end(v), IsZero());
    ASSERT_EQ(i, end(v));
    ASSERT_TRUE(v.empty());
}

TEST(Algorithm, partition_AlreadyPartitioned) {
    Vector v = {0, 0, 1, 1};
    auto i = danstd::partition(begin(v), end(v), IsZero());
    ASSERT_EQ(i, begin(v)+2);
}

TEST(Algorithm, partition_point_Empty) {
    Vector v;
    auto i = danstd::partition_point(begin(v), end(v), IsZero());
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, partition_point_TwoElements) {
    Vector v = {0, 1};
    auto i = danstd::partition_point(begin(v), end(v), IsZero());
    ASSERT_EQ(i, begin(v) + 1);
}

TEST(Algorithm, partition_point_ThreeElements) {
    Vector v = {0, 1, 1};
    auto i = danstd::partition_point(begin(v), end(v), IsZero());
    ASSERT_EQ(1, std::distance(begin(v), i));
    v = {0, 0, 1};
    i = danstd::partition_point(begin(v), end(v), IsZero());
    ASSERT_EQ(i, begin(v) + 2);
    v = {0, 0, 0};
    i = danstd::partition_point(begin(v), end(v), IsZero());
    ASSERT_EQ(i, end(v));
}

TEST(Algorithm, is_sorted_until_Empty) {
    Vector v;
    auto i = danstd::is_sorted_until(begin(v), end(v));
    ASSERT_EQ(end(v), i);
}

TEST(Algorithm, is_sorted_until_OneElement) {
    Vector v = {1};
    auto i = danstd::is_sorted_until(begin(v), end(v));
    ASSERT_EQ(1, i - begin(v));
}

TEST(Algorithm, is_sorted_until_TwoElements) {
    Vector v = {1, 2};
    auto i = danstd::is_sorted_until(begin(v), end(v));
    ASSERT_EQ(2, i - begin(v));

    v = {2, 1};
    i = danstd::is_sorted_until(begin(v), end(v));
    ASSERT_EQ(1, i - begin(v));
}

TEST(Algorithm, is_sorted_Empty) {
    Vector v;
    bool ret = danstd::is_sorted(begin(v), end(v));
    ASSERT_TRUE(ret);
}

TEST(Algorithm, is_sorted_Sorted) {
    Vector v = {1, 2, 3, 4};
    bool ret = danstd::is_sorted(begin(v), end(v));
    ASSERT_TRUE(ret);
}

TEST(Algorithm, is_sorted_UnSorted) {
    Vector v = {1, 5, 3, 4};
    bool ret = danstd::is_sorted(begin(v), end(v));
    ASSERT_FALSE(ret);
}

TEST(Algorithm, sort_Empty) {
    Vector expected;
    Vector v = expected;
    danstd::sort(begin(v), end(v));
    ASSERT_EQ(expected, v);
}

TEST(Algorithm, sort_OneElement) {
    Vector expected = {1};
    Vector v = expected;
    danstd::sort(begin(v), end(v));
    ASSERT_EQ(expected, v);
}

TEST(Algorithm, sort_TwoSortedElements) {
    Vector expected = {1, 2};
    Vector v = expected;
    danstd::sort(begin(v), end(v));
    ASSERT_EQ(expected, v);
}

TEST(Algorithm, sort_TwoUnsortedElements) {
    Vector expected = {1, 2};
    Vector v = {2, 1};
    danstd::sort(begin(v), end(v));
    ASSERT_EQ(expected, v);
}

TEST(Algorithm, sort_ThreeUnsortedElements) {
    Vector expected = {1, 2, 3};
    Vector v = {3, 2, 1};
    danstd::sort(begin(v), end(v));
    ASSERT_EQ(expected, v);

}

TEST(Algorithm, sort_TenUnsortedElements) {
    Vector expected = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Vector v = {9, 8, 10, 7, 4, 3, 2, 5, 1, 6};
    danstd::sort(begin(v), end(v));
    ASSERT_EQ(expected, v);
}

TEST(Algorithm, nth_element_Empty) {
    Vector expected;
    Vector v = expected;
    danstd::nth_element(begin(v), begin(v), end(v));
    ASSERT_EQ(expected, v);

}

TEST(Algorithm, nth_element_ThreeElements) {
    Vector expected = {1, 2, 3};
    Vector v = {3, 2, 1};
    danstd::nth_element(begin(v), begin(v), end(v));
    ASSERT_EQ(expected, v);
}

TEST(Algorithm, nth_element_TenElements) {
    Vector v = {8, 7, 2, 5, 4, 3, 6, 1, 0, 9};
    danstd::nth_element(begin(v), begin(v) + 5, end(v));
    ASSERT_EQ(v[5], 5);
    ASSERT_GT(5, v[0]);
    ASSERT_GT(5, v[1]);
    ASSERT_GT(5, v[2]);
    ASSERT_GT(5, v[3]);
    ASSERT_GT(5, v[4]);
}

TEST(Algorithm, partial_sort_TenElements) {
    Vector v = {8, 7, 2, 5, 4, 3, 6, 1, 0, 9};
    danstd::partial_sort(begin(v), begin(v) + 5, end(v));
    ASSERT_EQ(v[5], 5);
    ASSERT_EQ(0, v[0]);
    ASSERT_EQ(1, v[1]);
    ASSERT_EQ(2, v[2]);
    ASSERT_EQ(3, v[3]);
    ASSERT_EQ(4, v[4]);
}

TEST(Algorithm, binary_search_Empty) {
    Vector v;
    bool found = danstd::binary_search(begin(v), end(v), 0);
    ASSERT_EQ(false, found);
}

TEST(Algorithm, binary_search_OneElement) {
    Vector v = {1};
    bool found = danstd::binary_search(begin(v), end(v), 0);
    ASSERT_EQ(false, found);
    found = danstd::binary_search(begin(v), end(v), 1);
    ASSERT_EQ(true, found);
    found = danstd::binary_search(begin(v), end(v), 2);
    ASSERT_EQ(false, found);
}

TEST(Algorithm, binary_search_TwoElements) {
    Vector v = {1, 2};
    bool found = danstd::binary_search(begin(v), end(v), 0);
    ASSERT_EQ(false, found);
    found = danstd::binary_search(begin(v), end(v), 1);
    ASSERT_EQ(true, found);
    found = danstd::binary_search(begin(v), end(v), 2);
    ASSERT_EQ(true, found);
    found = danstd::binary_search(begin(v), end(v), 3);
    ASSERT_EQ(false, found);
}

TEST(Algorithm, binary_search_ThreeElements) {
    Vector v = {1, 2, 3};
    bool found = danstd::binary_search(begin(v), end(v), 0);
    ASSERT_EQ(false, found);
    found = danstd::binary_search(begin(v), end(v), 1);
    ASSERT_EQ(true, found);
    found = danstd::binary_search(begin(v), end(v), 2);
    ASSERT_EQ(true, found);
    found = danstd::binary_search(begin(v), end(v), 3);
    ASSERT_EQ(true, found);
    found = danstd::binary_search(begin(v), end(v), 4);
    ASSERT_EQ(false, found);
}

TEST(Algorithm, lower_bound_Empty) {
    Vector v;
    auto i = danstd::lower_bound(begin(v), end(v), 42);
    ASSERT_EQ(end(v), i);
}

TEST(Algorithm, lower_bound_OneElement) {
    Vector v = {1};
    auto i = danstd::lower_bound(begin(v), end(v), 0);
    ASSERT_EQ(end(v), i);
    i = danstd::lower_bound(begin(v), end(v), 1);
    ASSERT_EQ(0, std::distance(begin(v), i));
}

TEST(Algorithm, lower_bound_TwoElements) {
    Vector v = {1, 2};
    auto i = danstd::lower_bound(begin(v), end(v), 0);
    ASSERT_EQ(0, std::distance(end(v), i));
    i = danstd::lower_bound(begin(v), end(v), 1);
    ASSERT_EQ(0, std::distance(begin(v), i));
    i = danstd::lower_bound(begin(v), end(v), 2);
    ASSERT_EQ(1, std::distance(begin(v), i));
}

class StableInt {
public:
    StableInt() {
        StableInt(0);
    }
    StableInt(int val) : val_(val), order_(kSequenceNumber++) {}
    bool operator<(const StableInt& other) {
        return val_ < other.val_;
    }

    bool operator==(const StableInt& other) const {
        return val_ == other.val_;
    }


    int operator()() {
        return val_;
    }

    int order() {
        return order_;
    }

    friend  std::ostream& operator<<(std::ostream& os, const StableInt& s);

private:
    int val_;
    int order_;
    static int kSequenceNumber;
};

std::ostream& operator<<(std::ostream& os, const StableInt& s) {
    os << s.val_;
    return os;
}

int StableInt::kSequenceNumber = 0;

TEST(Algorithm, merge_Empty) {
    std::vector<StableInt> a;
    std::vector<StableInt> b;
    std::vector<StableInt> dest(a.size() + b.size());
    auto i = danstd::merge(begin(a), end(a), begin(b), end(b), begin(dest));
    ASSERT_EQ(0, std::distance(begin(dest), i));
}

TEST(Algorithm, merge_OneElement) {
    std::vector<StableInt> a = {0};
    std::vector<StableInt> b = {1};
    std::vector<StableInt> dest(a.size() + b.size());
    std::vector<StableInt> expected = {0, 1};
    auto i = danstd::merge(begin(a), end(a), begin(b), end(b), begin(dest));
    ASSERT_EQ(expected, dest);
    ASSERT_EQ(2, std::distance(begin(dest), i));

}

TEST(Algorithm, merge_ThreeElementsTwoIdentical) {
    std::vector<StableInt> a = {0, 0};
    std::vector<StableInt> b = {1};
    std::vector<StableInt> dest(a.size() + b.size());
    std::vector<StableInt> expected = {0, 0, 1};
    auto i = danstd::merge(begin(a), end(a), begin(b), end(b), begin(dest));
    ASSERT_EQ(expected, dest);
    ASSERT_EQ(3, std::distance(begin(dest), i));
    ASSERT_LT(dest[0].order(), dest[1].order());
}

TEST(Algorithm, includes_Empty) {
    Vector v1;
    Vector v2;
    bool r = danstd::includes(begin(v1), end(v1), begin(v2), end(v2));
    ASSERT_EQ(true, r);
}

TEST(Algorithm, includes_EmptyOrSingle) {
    Vector v1;
    Vector v2 = {1};
    bool r = danstd::includes(begin(v1), end(v1), begin(v2), end(v2));
    ASSERT_EQ(true, r);

    v1 = {1};
    v2 = {};
    r = danstd::includes(begin(v1), end(v1), begin(v2), end(v2));
    ASSERT_EQ(false, r);

    v1 = {1};
    v2 = {1};
    r = danstd::includes(begin(v1), end(v1), begin(v2), end(v2));
    ASSERT_EQ(true, r);
}

TEST(Algorithm, includes_TwoInSizeDifference) {
    Vector v1 = {1, 2};
    Vector v2 = {1, 2, 3, 4};
    bool r = danstd::includes(begin(v1), end(v1), begin(v2), end(v2));
    ASSERT_EQ(true, r);
}

TEST(Algorithm, includes_AnElementMissing) {
    Vector v1 = {1, 2, 42};
    Vector v2 = {1, 2};
    bool r = danstd::includes(begin(v1), end(v1), begin(v2), end(v2));
    ASSERT_EQ(false, r);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
