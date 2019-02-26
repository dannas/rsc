// Copied from sieve8.c
// Unrolled outer loop 2x

#define bit_test(a, i)  (a[(i) / 64]  &  (1UL << ((i) % 64)))
#define bit_set(a, i)   (a[(i) / 64] |=  (1UL << ((i) % 64)))
#define bit_clear(a, i) (a[(i) / 64] &= ~(1UL << ((i) % 64)))

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
#define LENGTH 170000
    int NUM = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    static uint64_t flags[64 + 1];
    long i, j, k;
    int count = 0;

    while (NUM--) {
        // For 2
        count = 1;
        memset(&flags, ~0, sizeof(flags));

        for (i=3; i <= 8192-3; i+=4) {

            j = i + 2;
#define BODY(i) \
            if (bit_test(flags, i >> 1)) { \
                for (k=3*i; k <= 8192; k+=4*i) { \
                    bit_clear(flags, k >> 1); \
                    bit_clear(flags, (k+i+i) >> 1); \
                } \
                for (; k <= 8192; k+=2*i) { \
                    bit_clear(flags, k >> 1); \
                } \
                count++; \
            }

            BODY(i)
            BODY(j)
        }
        for (; i <= 8192; i+=2) {
            BODY(i)
        }
    }
    printf("Count: %d\n", count);
    return(0);
}
