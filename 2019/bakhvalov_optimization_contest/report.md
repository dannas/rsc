## Preparations
I started out by downloading the sieve.c from
https://dendibakh.github.io/blog/2019/02/16/Performance-optimization-contest-1

## Baseline

    $ time -p ./sieve
    Count: 1028
    real 5.18
    user 5.18
    sys 0.00

It reported that the program was frontend bound

    $ cset shield --exec -- toplev.py --core S0-C0 -l2  --no-desc taskset -c 0 ./sieve
    Count: 1028
    C0    FE             Frontend_Bound:                             38.16 +-     0.00 % Slots
    C0    BAD            Bad_Speculation:                            24.42 +-     0.00 % Slots
    C0    FE             Frontend_Bound.Frontend_Latency:            27.24 +-     0.00 % Slots <==
    C0    FE             Frontend_Bound.Frontend_Bandwidth:          10.97 +-     0.00 % Slots
    C0    BAD            Bad_Speculation.Branch_Mispredicts:         24.42 +-     0.00 % Slots

## Version 1
Dead end. Tried using a bitset instead of the char array, but it was much slower.

    $ time -p ./sieve1
    Count: 1028
    real 9.43
    user 9.43
    sys 0.00

## Version 2
Unrolled the inner loop by 4x.

    $ time -p ./sieve2
    Count: 1028
    real 3.41
    user 3.41
    sys 0.00

Now the program was bound by bad speculation.

    $ cset shield --exec -- toplev.py --core S0-C0 -l2  --no-desc taskset -c 0 ./sieve2
    Count: 1028
    C0    BAD            Bad_Speculation:                            22.82 +-     0.00 % Slots
    C0    BAD            Bad_Speculation.Branch_Mispredicts:         22.82 +-     0.00 % Slots <==
    C0    BE             Backend_Bound:                              19.60 +-     0.00 % Slots
    C0    BE/Core        Backend_Bound.Core_Bound:                   12.34 +-     0.00 % Slots

# Version 3
Dead end. I unrolled the first loop that initializes `flags` to all ones, but that didn't improve performance.

    $ time -p ./sieve3
    Count: 1028
    real 3.58
    user 3.58
    sys 0.00

It was now backend bound.

    $ cset shield --exec -- toplev.py --core S0-C0 -l2  --no-desc taskset -c 0 ./sieve3
    Count: 1028
    C0    BAD            Bad_Speculation:                            19.52 +-     0.00 % Slots
    C0    BAD            Bad_Speculation.Branch_Mispredicts:         19.51 +-     0.00 % Slots
    C0    BE             Backend_Bound:                              20.30 +-     0.00 % Slots
    C0    BE/Core        Backend_Bound.Core_Bound:                   13.39 +-     0.00 % Slots <==

# Version 4
I dropped version 3, and instead unrolled the inner loop from version 2 by 2x.

    $ time -p ./sieve4
    Count: 1028
    real 2.69
    user 2.69
    sys 0.00

This version was backend bound.

    $ cset shield --exec -- toplev.py --core S0-C0 -l2  --no-desc taskset -c 0 ./sieve4
    Count: 1028
    C0    BAD            Bad_Speculation:                            13.49 +-     0.00 % Slots
    C0    BAD            Bad_Speculation.Branch_Mispredicts:         13.49 +-     0.00 % Slots
    C0    BE             Backend_Bound:                              22.79 +-     0.00 % Slots
    C0    BE/Core        Backend_Bound.Core_Bound:                   14.02 +-     0.00 % Slots <==

# Version 5
I unrolled the outer loop by 3x.

    $ time -p ./sieve5
    Count: 1028
    real 2.04
    user 2.04
    sys 0.00

It was still backend bound.

    $ cset shield --exec -- toplev.py --core S0-C0 -l2  --no-desc taskset -c 0 ./sieve5
    Count: 1028
    C0    BE             Backend_Bound:                      28.19 +-     0.00 % Slots <==
    C0    BE/Mem         Backend_Bound.Memory_Bound:         12.59 +-     0.00 % Slots
    C0    BE/Core        Backend_Bound.Core_Bound:           15.59 +-     0.00 % Slots

# Version 6
Dead end. I unrolled the inner loop by 4x, but didn't get any noticeable improvement.

    $ time -p ./sieve5
    Count: 1028
    real 2.04
    user 2.04
    sys 0.00

It was still backend bound.

    $ cset shield --exec -- toplev.py --core S0-C0 -l2  --no-desc taskset -c 0 ./sieve6
    Count: 1028
    C0    BE             Backend_Bound:                      28.93 +-     0.00 % Slots <==
    C0    BE/Mem         Backend_Bound.Memory_Bound:         13.38 +-     0.00 % Slots
    C0    BE/Core        Backend_Bound.Core_Bound:           15.55 +-     0.00 % Slots



