#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <utility>

namespace counterfeit {

// Non modifying sequence operations

template <typename IT, typename T>
IT find(IT b, IT e, T val) {
    for (; b != e; ++b) {
        if (*b == val)
            return b;
    }
    return e;
}

template <typename T, typename Predicate>
T find_if(T b, T e, Predicate p) {
    for (; b != e; ++b) {
        if (p(*b))
            return b;
    }
    return e;
}

template <typename T, typename Predicate>
T find_if_not(T b, T e, Predicate p) {
    for (; b != e; ++b) {
        if (!p(*b))
            return b;
    }
    return e;
}

template <typename T, typename Predicate>
bool all_of(T b, T e, Predicate p) {
    return find_if_not(b, e, p) == e;
} 

template <typename T, typename Predicate>
bool any_of(T b, T e, Predicate p) {
    return find_if(b, e, p) != e;
}

template <typename T, typename Predicate>
bool none_of(T b, T e, Predicate p) {
    return find_if(b, e, p) == e;
}

template <typename T, typename Function>
Function for_each(T b, T e, Function f) {
    for (; b != e; ++b)
        f(*b);
    return f;
}

template <typename T, typename Size, typename Function>
Function for_each_n(T b, Size n, Function f) {
    auto e = b + n;
    for (; b != e; ++b)
        f(*b);
    return f;
}

template <typename T, typename V>
typename std::iterator_traits<T>::difference_type
count(T b, T e, V value) {
    typename std::iterator_traits<T>::difference_type n = 0;
    for (; b != e; ++b) {
        if (value == *b)
            ++n;
    }
    return n;
}

template <typename T, typename Predicate>
typename std::iterator_traits<T>::difference_type
count_if(T b, T e, Predicate p) {
    typename std::iterator_traits<T>::difference_type n = 0;
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

template <typename T>
T adjacent_find(T b, T e) {
    if (b == e)
        return e;
    T prev = b;
    ++b;
    for (; b != e; ++b) {
        if (*b == *prev)
            return prev;
        prev = b;
    }
    return e;
}

template <typename IT, typename Size, typename T>
IT search_n(IT b, IT e, Size size, const T& val) {
    Size n = 0;
    IT i = e;
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
// TODO: Add these
// move
// move_backward
// fill
// fill_n
// transform
// generate
// generate_n
// ...

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

template <typename T, typename Predicate>
T remove_if(T b, T e, Predicate p) {
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


} // namespace counterfeit
