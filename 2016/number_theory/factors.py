
"""
Factorize a number into two factors.

We don't have to check for factors larger than sqrt(N),
TODO(dannas): Can we do better than O(sqrt(N))?

2, 3, 5, 7 are often found to be factors, after that there is often a lot of
numbers that are not found to be factors.
Can we skip some numbers if we've found a match?
That sounds like a Sieve of Erasthones thingy...

Factoring numbers are hard, if not, we could just do the factoring
and combine the found numbers.

What about detecting primes? If that's easy then we could optimize those paths?
But the ratio of primes to non-primes are low, we'd be optimizing the cold path.

If this is the "optimial" algorithm, then how about doing a Jon Bentley
step-by-step optimization. Suggestions:
* Use a non-interpreted language
* optimize division
* optimize readahead cache
* use vector instructions
* use multiple threads
* use the gpu
"""

from math import sqrt, ceil

def max_factor(N):
    return int((sqrt(N)))

def iterfactors(N):
    m = max_factor(N)
    return range(2, m+1)

def is_factor(x, N):
    return N % x == 0

nil = (0, 0)

def try_factorize(x, N):
    return (x, N/x) if is_factor(x, N) else nil

def factor_pairs(N):
    """ Return factors of N. """
    P = []
    for x in iterfactors(N):
        pair = try_factorize(x, N)
        if pair != nil:
            P.append(pair)

    # TODO(dannas): Should we include the mirrored factor pairs?
    #P += [(y, x) for x, y in P if x != y]
    return P

def count_factors(N):
    return len(factors(N))

def distance(N):
    # TODO(dannas): How write a list comprehension that computes the
    # difference between elements in a sequence?
    f = factor_pairs(N)
    L = [x for x,_ in f]
    d = []
    prev = 0
    for x in L:
        d.append(x-prev)
        prev = x
    return d

