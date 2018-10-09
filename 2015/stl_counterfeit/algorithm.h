#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <utility>

namespace danstd {

// NON MODIFYING SEQUENCE OPERATIONS

// Return the first element in [b, e) that is equal to |val|.
template <typename I, typename T>
I find(I b, I e, T val) {
    for (; b != e; ++b) {
        if (*b == val)
            return b;
    }
    return e;
}

// Return the first element in [b, e) for which |p| returns true.
template <typename I, typename Predicate>
I find_if(I b, I e, Predicate p) {
    for (; b != e; ++b) {
        if (p(*b))
            return b;
    }
    return e;
}

// Return the first element in [b, e) for which |p| returns false.
template <typename I, typename Predicate>
I find_if_not(I b, I e, Predicate p) {
    for (; b != e; ++b) {
        if (!p(*b))
            return b;
    }
    return e;
}

// Return true if |p| returns true for all elements in [b, e).
template <typename I, typename Predicate>
bool all_of(I b, I e, Predicate p) {
    return find_if_not(b, e, p) == e;
} 

// Return true if |p| returns true for any of the elements in [b, e).
template <typename I, typename Predicate>
bool any_of(I b, I e, Predicate p) {
    return find_if(b, e, p) != e;
}

// Return true if |p| is false for all elements in [b, e).
template <typename I, typename Predicate>
bool none_of(I b, I e, Predicate p) {
    return find_if(b, e, p) == e;
}

// Apply |f| to all elements in [b, e)
template <typename I, typename Function>
Function for_each(I b, I e, Function f) {
    for (; b != e; ++b)
        f(*b);
    return f;
}

// Apply |f| to all elements in [b, b+n).
template <typename I, typename Size, typename Function>
Function for_each_n(I b, Size n, Function f) {
    auto e = b + n;
    for (; b != e; ++b)
        f(*b);
    return f;
}

// Return the number of elements in [b, e) that are equal to |value|.
template <typename I, typename T>
typename std::iterator_traits<I>::difference_type
count(I b, I e, T value) {
    typename std::iterator_traits<I>::difference_type n = 0;
    for (; b != e; ++b) {
        if (value == *b)
            ++n;
    }
    return n;
}

// Return the number of elements in [b, e) for which |p| is true.
template <typename I, typename Predicate>
typename std::iterator_traits<I>::difference_type
count_if(I b, I e, Predicate p) {
    typename std::iterator_traits<I>::difference_type n = 0;
    for (; b != e; ++b) {
        if (p(*b))
            ++n;
    }
    return n;
}

// Return the first pair of mismatching elements from [b1, e1) and
// [b2, b2+ e1-b1).
template <typename T1, typename T2>
std::pair<T1, T2> mismatch(T1 b1, T1 e1, T2 b2) {
    while (b1 != e1 && *b1 == *b2) {
        ++b1;
        ++b2;
    }
    return std::make_pair(b1, b2);

}

// Return true if [b1, e1) is equal to [b2, b2+e1-b1).
template <typename T1, typename T2>
bool equal(T1 b1, T1 e1, T2 b2) {
    while (b1 != e1) {
        if (*b1 != *b2)
            return false;
        ++b1;
        ++b2;
    }
    return true;
}

// Return position of the first subsequence [s_b, s_e) in [b, e).
template <typename T1, typename T2>
T1 search(T1 b, T1 e, T2 s_b, T2 s_e) {
    for (; b != e; ++b) {
        auto s_next = s_b;
        for (auto i = b; i != e && s_next != s_e; ++i, ++s_next) {
            if (*i != *s_next) {
                break;
            }
        }
        if (s_next == s_e)
            return b;
    }
    return e;
}

// Return position of the last subsequence [s_b, s_e) in [b, e).
template <typename T1, typename T2>
T1 find_end(T1 b, T1 e, T2 s_b, T2 s_e) {
    auto r = e;
    for (; b != e; ++b) {
        auto i = danstd::search(b, e, s_b, s_e);
        if (i != e)
            r = i;
    }
    return r;
}

// Return position of first element in [b, e) that is equal to an element in
// [s_b, s_e)
template <typename T1, typename T2>
T1 find_first_of(T1 b, T1 e, T2 s_b, T2 s_e) {
    for (; b != e; ++b) {
        for (auto i = s_b; i != s_e; ++i) {
            if (*b == *i)
                return b;
        }
    }
    return e;
}

// Return first position of two identical adjacent elements in [b, e).
template <typename I>
I adjacent_find(I b, I e) {
    if (b == e)
        return e;
    I prev = b;
    ++b;
    for (; b != e; ++b) {
        if (*b == *prev)
            return prev;
        prev = b;
    }
    return e;
}

// Return position of [i, i+size) where *i is equal to |val|
template <typename I, typename Size, typename T>
I search_n(I b, I e, Size size, const T& val) {
    Size n = 0;
    I i = e;
    for (; b != e; ++b) {
        if (*b == val) {
            if (n == 0)
                i = b;
            ++n;
        } else {
            n = 0;
        }
        if (n == size)
            return i;
    }
    return e;
}

// MODIFYING SEQUENCE OPERATIONS

// Copy [b, e) to [d_b, d_b + e-b). Return end(destination).
template <typename I, typename O>
O copy(I b, I e, O d_b) {
    for (; b != e; ++b, ++d_b) {
        *d_b = *b;
    }
    return d_b;
}

// Copy [b, b+size) to [d_b, d_b + size). Return d_b + size.
template <typename I, typename Size, typename O>
O copy_n(I b, I e, Size size, O d_b) {
    for(; b != e && size > 0; --size) {
        *d_b++ = *b++;
    }
    return d_b;
}

// Copy [b, e) in reverse order to [..., d_e). Return iterator
// to last element copied.
template <typename I, typename O>
O copy_backward(I b, I e, O d_e) {
    while (b != e) {
        *--d_e = *--e;
    }
    return d_e;
}

// Move [b, e) to [d_b, ...)
template <typename I, typename O>
O move(I b, I e, O d_b) {
    for (; b != e; ++b) {
        *d_b++ = std::move(*b);
    }
    return d_b;
}

// Move [b, e) in reverse order to a range endinng at |d_e|. Return iterator
// to the last element copied.
template <typename I, typename O>
O move_backward(I b, I e, O d_b, O d_e) {
    while (b != e && d_b != d_e) {
        *--d_e = *--e;
    }
    return d_e;
}

// Assign [b, e) the value |val|.
template <typename I, typename T>
void fill(I b, I e, T val) {
    for (; b != e; ++b)
        *b = val;
}

// Assign [b, b+n) the value |val|.
template <typename I, typename Size, typename T>
void fill_n(I b, I e, Size n, T val) {
    for (; b != e && n > 0; ++b, --n) {
        *b = val;
    }
}

// Apply |p| to [b, e) and store the result [d_b, ...)
template <typename I, typename O, typename F>
O transform(I b, I e, O d_b, F f) {
    for (; b != e; ++b) {
        *d_b++ = f(*b);
    }
    return d_b;
}

// Assign [b, e) a value generated by |f|
template <typename I, typename F>
void generate(I b, I e, F f) {
    for (; b != e; ++b) {
        *b = f();
    }
}

// Assign [b, b+n) a value generated by |f|
template <typename I, typename Size, typename F>
void generate_n(I b, I e, Size n, F f) {
    for (; b != e && n > 0; ++b, --n) {
        *b = f();
    }
}

// Remove each element in [b, e) for which |p| is true. Return a new past-end
// iterator.
template <typename I, typename Predicate>
I remove_if(I b, I e, Predicate p) {
    b = find_if(b, e, p);
    if (b == e)
        return b;

    // Precondition: b points to an item that shall be removed.
    auto result = b;
    ++b;
    for (; b != e; ++b) {
        // Invariant: b > result
        if (!p(*b)) {
            *result = std::move(*b);
            ++result;
        }
    }
    return result;
}

// Copy [b, e) to [d_b, ...), omitting elements equal to |val|.
template <typename I, typename O, typename T>
O remove_copy(I b, I e, O d_b, const T& val) {
    for (; b != e; ++b) {
        if (*b != val)
            *d_b++ = *b;
    }
    return d_b;
}

// Replace all elements in [b, e) equal to |old| with |val|.
template <typename I, typename T>
void replace(I b, I e, const T& old, const T& val) {
    for (; b != e; ++b) {
        if (*b == old)
            *b = val;
    }
}

// Copy [b, e) to [d_b, ...), replacing elements equal to |old| with |val|.
template <typename I, typename O, typename T>
O replace_copy(I b, I e, O d_b, const T& old, const T& val) {
    for (; b != e; ++b) {
        if (*b == old)
            *d_b++ = val;
        else
            *d_b++ = *b;
    }
    return d_b;
}

// Swap values |a| and |b|.
template <typename T>
void swap(T& a, T& b) {
    T temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
}

// Swaps the values of the elements the given iterators are pointing to.
template <typename I>
void iter_swap(I a, I b) {
    using std::swap;
    swap(*a, *b);
}

// Exchange [b1, e1) and [b2,  e2)
template <typename I, typename O>
O swap_ranges(I b1, I e1, O b2, O e2) {
    for (; b1 != e1 && b2 != e2; ++b1, ++b2) {
        iter_swap(b1, b2);
    }
    return b2;
}

// Reverse the order of the elements in [b, e).
template <typename I>
void reverse(I b, I e) {
    while (b != e && b != --e) {
        iter_swap(b++, e);
    }
}

// Copy [b, e) to [d_b, ...) in reverse order.
template <typename I, typename O>
O reverse_copy(I b, I e, O d_b) {
    while (b != e) {
        *d_b++ = *--e;
    }
    return d_b;
}

// Perform a left rotation on [b,...,m,...,e) such that [m, ..., m-1)
template <typename I>
void rotate(I b, I m, I e) {
    danstd::reverse(b, m);
    danstd::reverse(m, e);
    danstd::reverse(b, e);
}

// TODO(dannas): Add
// rotate_copy
// shuffle

template <typename I>
I unique(I b, I e) {
    if (b == e)
        return e;

    I r = b;
    while (++b != e) {
        if (*r != *b && ++r != b) {
            *r = std::move(*b);
        }
    }
    return ++r;
}

template <typename I, typename O>
O unique_copy(I b, I e, O d_b) {
   if (b == e)
       return e;

   auto prev = *b;
   *d_b++ = *b++;

   for(; b != e; ++b) {
       if (*b != prev)
           *d_b++ = *b;
       prev = *b;
   }
   return d_b;
}

// Partitioning operations

template <typename I, typename F>
bool is_partitioned(I b, I e, F f) {
    bool seen = false;

    for (; b != e; ++b) {
        bool r = f(*b);
        if (seen && r)
            return false;
        if (!r)
            seen = true;
    }
    return true;
}

template <typename I, typename O1, typename O2, typename F>
std::pair<O1, O2> partition_copy(I b, I e, O1 d_true, O2 d_false, F f) {
    for (; b != e; ++b) {
        if (f(*b))
            *d_true++ = *b;
        else
            *d_false++ = *b;
    }
    return make_pair(d_true, d_false);
}

template <typename I, typename F>
I partition(I b, I e, F f) {
    auto i = b;
    for (; b != e; ++b) {
        // Invariant: [0,i) is true for f
        // Invariant: [i,b) is false for f
        if (f(*b))
            iter_swap(b, i);
        if (f(*i))
            ++i;
    }
    return i;
}

template <typename I, typename F>
I partition_point(I b, I e, F f) {
    while (b != e) {
        I m = b + (e - b) / 2;
        if (f(*m)) {
            b = m + 1;
        } else {
            e = m + (e - m) / 2;
        }
    }
    return b;
}

// TODO(dannas): Add
// stable_partion

// Sorting operations

template <typename I>
I is_sorted_until(I b, I e) {
    if (b == e)
        return e;
    auto prev = b;
    ++b;
    while (b != e) {
        // Invariant: [0, b) is sorted
        if (*b < *prev)
            return b;
        prev = b;
        ++b;
    }
    return b;
}

template <typename I>
bool is_sorted(I b, I e) {
    return danstd::is_sorted_until(b, e) == e;
}

template <typename I>
void sort(I b, I e) {
    if (std::distance(b, e) <= 1)
        return;
    auto mid = b + (e - b) / 2;
    auto pivot= *mid;

    danstd::iter_swap(mid, e-1);
    mid = danstd::partition(b, e-1, [pivot] (auto& x) { return x < pivot;});
    danstd::iter_swap(e-1, mid);

    danstd::sort(b, mid);
    danstd::sort(mid, e);
}

template <typename I>
void nth_element(I b, I m, I e) {
    if (std::distance(b, e) <= 1)
        return;
    auto mid = b + (e - b) / 2;
    auto pivot= *mid;

    danstd::iter_swap(mid, e-1);
    mid = danstd::partition(b, e-1, [pivot] (auto& x) { return x < pivot;});
    danstd::iter_swap(e-1, mid);

    if (m < mid)
        danstd::sort(b, mid);
    else
        danstd::sort(mid, e);
}

template <typename I>
void partial_sort(I b, I m, I e) {
    danstd::nth_element(b, m, e);
    danstd::sort(b, m);
}

// TODO(dannas): Add
// stable_sort
// partial_sort_copy
//
// Binary search operations

template <typename I, typename T>
I lower_bound(I b, I e, T val) {
    if (b == e)
        return e;
    auto notFound = e;

    while (b < e) {
        auto mid = b + (e - b) / 2;
        if (*mid < val)
            b = mid + 1;
        else
            e = mid;
    }

    if (*e == val)
        return e;
    return notFound;
}

template <typename I, typename T>
I upper_bound(I b, I e, T val) {
    // TODO(dannas): How add lower_bound
    // Pre: A sorted list L
    // Post: pos points to the rightmost elemnt equal to or larger than val
    //      upper_bound of 2 for [1, 2, 2, 3] => pos 2
    //      upper_bound of 2 for [1, 3, 3, 3] => pos 0
    //      upper_bound of 2 for [1, 1, 1, 3] => pos 2
    // What  to do now?
    //      Come up with a strategy
    //      refine it
    //      make sure we make progress
    //      make sure we halt
    //      make sure each iteration is correct
    //
    // How make sure the input range is cut in half on each iteration?
    //  How can I cut it in half but end up on the left end of the interval?
    //  If I just shrink the upper endpoint by one,
    //  then in the  pathological case [1, 1, 1, 1...] It will take N iterations

}

// TODO(dannas): Add
// binary_search

template <typename I, typename T>
bool binary_search(I b, I e, T val) {
    if (b == e)
        return false;
    e--;
    while (b <= e)  {
        auto mid = b + (e - b) / 2;
        if (*mid == val)
            return true;
        else if (*mid < val)
            b = mid + 1;
        else
            e = mid - 1;
    }
    return false;
}

// equal_range
//
// Set operations

template <typename I1, typename I2, typename O>
O merge(I1 b1, I1 e1, I2 b2, I2 e2, O d_b) {
    while (b1 != e1 || b2 != e2) {
        if (b1 == e1)
            *d_b++ = *b2++;
        else if (b2 == e2)
            *d_b++ = *b1++;
        else {
            if (*b1 < *b2)
                *d_b++ = *b1++;
            else
                *d_b++ = *b2++;
        }
    }
    return d_b;
}

template <typename I1, typename I2>
bool includes(I1 b1, I2 e1, I2 b2, I2 e2) {

    for (; b1 != e1; ++b1) {
        b2 = danstd::find(b2, e2, *b1);
        if (b2 == e2)
            return false;
    }
    return true;
}

template <typename I1, typename I2, typename O>
O set_union(I1 b1, I1 e1, I2 b2, I2 e2, O d_b) {
    auto o = d_b;
    for (; b1 != e1; ++b1) {
        *d_b++ = *b1;
        while (b2 != e2 && *b2 <= *b1) {
            if (*b2 != *b1)
                *d_b++ = *b2;
            b2++;
        }
    }
    for (; b2 != e2; ++b2) {
        *d_b++ = *b2;
    }
    return o;
}

template <typename I1, typename I2, typename O>
O set_difference(I1 b1, I1 e1, I2 b2, I2 e2, O d_b) {
    for (; b1 != e1; ++b1) {
        while (b2 != e2 && *b2 < *b1) {
            ++b2;
        }
        if (b2 == e2 ||  *b2 != *b1) {
            *d_b++ = *b1;
        }
    }
    return d_b;
}

// inplace_merge
// set_intersection
// set_symmetric_intersection
//
// Heap operations
// is_heap
// is_heap_until
// make_heap
// push_heap
// pop_heap
// sort_heap

// Minimum/maximum operations

template <typename T>
const T& max(const T& a, const T& b) {
    return a < b ? b : a;
}

template <typename I>
I max_element(I b, I e) {
    if (b == e) {
        return e;
    }
    I m = b;
    ++b;
    while (b != e) {
        if (*m < *b) {
            m = b;
        }
        b++;
    }
    return m;
}

template <typename T>
const T& min(const T& a, const T& b) {
    return a > b ? b : a;
}

template <typename I>
I min_element(I b, I e) {
    if (b == e) {
        return e;
    }
    I m = b;
    ++b;
    while (b != e) {
        if (*m > *b) {
            m = b;
        }
        b++;
    }
    return m;
}
// minmax_element
// lexiographical_compare
// is_permutation
// next_permutation
// prev_permutation
//
// Numeric operations
// iota
// inner_product
// adjacent_difference

template <typename I, typename O>
O partial_sum(I b, I e, O d_b) {
    if (b == e)
        return d_b;
    O r = d_b;
    *d_b++ = *b++;

    while (b != e) {
        *d_b = d_b[-1] + *b;
        b++;
        d_b++;
    }
    return r;
}

template <typename I, typename T>
T accumulate(I b, I e, T init) {
    while (b != e) {
        init += *b;
        b++;
    }
    return init;
}


} // namespace counterfeit
