#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <utility>

namespace counterfeit {

// Non modifying sequence operations

template <typename I, typename T>
I find(I b, I e, T val) {
    for (; b != e; ++b) {
        if (*b == val)
            return b;
    }
    return e;
}

template <typename I, typename Predicate>
I find_if(I b, I e, Predicate p) {
    for (; b != e; ++b) {
        if (p(*b))
            return b;
    }
    return e;
}

template <typename I, typename Predicate>
I find_if_not(I b, I e, Predicate p) {
    for (; b != e; ++b) {
        if (!p(*b))
            return b;
    }
    return e;
}

template <typename I, typename Predicate>
bool all_of(I b, I e, Predicate p) {
    return find_if_not(b, e, p) == e;
} 

template <typename I, typename Predicate>
bool any_of(I b, I e, Predicate p) {
    return find_if(b, e, p) != e;
}

template <typename I, typename Predicate>
bool none_of(I b, I e, Predicate p) {
    return find_if(b, e, p) == e;
}

template <typename I, typename Function>
Function for_each(I b, I e, Function f) {
    for (; b != e; ++b)
        f(*b);
    return f;
}

template <typename I, typename Size, typename Function>
Function for_each_n(I b, Size n, Function f) {
    auto e = b + n;
    for (; b != e; ++b)
        f(*b);
    return f;
}

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

template <typename T1, typename T2>
std::pair<T1, T2> mismatch(T1 b1, T1 e1, T1 b2) {
    while (b1 != e1 && *b1 == *b2) {
        ++b1;
        ++b2;
    }
    return std::make_pair(b1, b2);

}

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

template <typename T1, typename T2>
T1 find_end(T1 b, T1 e, T2 s_b, T2 s_e) {
    auto r = e;
    for (; b != e; ++b) {
        auto i = search(b, e, s_b, s_e);
        if (i != e)
            r = i;
    }
    return r;
}

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

// Modifying sequence operations

template <typename I, typename O>
O copy(I b, I e, O d_b) {
    for (; b != e; ++b, ++d_b) {
        *d_b = *b;
    }
    return d_b;
}

template <typename I, typename Size, typename O>
O copy_n(I b, I e, Size size, O d_b) {
    for(; b != e && size > 0; --size) {
        *d_b++ = *b++;
    }
    return d_b;
}

template <typename I, typename O>
O copy_backward(I b, I e, O d_e) {
    while (b != e) {
        *--d_e = *--e;
    }
    return d_e;
}

template <typename I, typename O>
O move(I b, I e, O d_b) {
    for (; b != e; ++b) {
        *d_b++ = std::move(*b);
    }
    return d_b;
}

template <typename I, typename O>
O move_backward(I b, I e, O d_b, O d_e) {
    while (b != e && d_b != d_e) {
        *--d_e = *--e;
    }
    return d_e;
}

template <typename I, typename T>
void fill(I b, I e, T val) {
    for (; b != e; ++b)
        *b = val;
}

template <typename I, typename Size, typename T>
void fill_n(I b, I e, Size n, T val) {
    for (; b != e && n > 0; ++b, --n) {
        *b = val;
    }
}

template <typename I, typename O, typename F>
O transform(I b, I e, O d_b, F f) {
    for (; b != e; ++b) {
        *d_b++ = f(*b);
    }
    return d_b;
}

template <typename I, typename F>
void generate(I b, I e, F f) {
    for (; b != e; ++b) {
        *b = f();
    }
}

template <typename I, typename Size, typename F>
void generate_n(I b, I e, Size n, F f) {
    for (; b != e && n > 0; ++b, --n) {
        *b = f();
    }
}

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

template <typename I, typename O, typename T>
O remove_copy(I b, I e, O d_b, const T& val) {
    for (; b != e; ++b) {
        if (*b != val)
            *d_b++ = *b;
    }
    return d_b;
}

template <typename I, typename T>
void replace(I b, I e, const T& old, const T& val) {
    for (; b != e; ++b) {
        if (*b == old)
            *b = val;
    }
}

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

template <typename T>
void swap(T& a, T& b) {
    T temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
}

template <typename I>
void iter_swap(I a, I b) {
    using std::swap;
    swap(*a, *b);
}

template <typename I, typename O>
O swap_ranges(I b1, I e1, O b2, O e2) {
    for (; b1 != e1 && b2 != e2; ++b1, ++b2) {
        iter_swap(b1, b2);
    }
    return b2;
}

template <typename I>
void reverse(I b, I e) {
    while (b != e && b != --e) {
        iter_swap(b++, e);
    }
}

template <typename I, typename O>
O reverse_copy(I b, I e, O d_b) {
    while (b != e) {
        *d_b++ = *--e;
    }
    return d_b;
}

// TODO(dannas): Add
// rotate
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
I partion(I b, I e, F f) {
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

// TODO(dannas): Add
// stable_partion
// partition_point
//
// Sorting operations
// is_sorted
// is_sorted_until
// sort
// partial_sort
// partial_sort_copy
// stable_sort
// nth_element
//
// Binary search operations
// lower_bound
// upper_bound
// binary_search
// equal_range
//
// Set operations
// merge
// inplace_merge
// includes
// set_difference
// set_intersection
// set_symmetric_intersection
// set_union
//
// Heap operations
// is_heap
// is_heap_until
// make_heap
// push_heap
// pop_heap
// sort_heap
//
// Minimum/maximum operations
// max
// max_element
// min
// min_element
// minmax_element
// lexiographical_compare
// is_permutation
// next_permutation
// prev_permutation
//
// Numeric operations
// iota
// accumulate
// inner_product
// adjacent_difference
// partial_sum

} // namespace counterfeit
