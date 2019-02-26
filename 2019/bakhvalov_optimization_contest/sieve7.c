// Copied from baseline
// Only check odd numbers
// Skip even multiples of odd numbers

#define bit_test(a, i)  (a[(i) / 64]  &  (1UL << ((i) % 64)))
#define bit_set(a, i)   (a[(i) / 64] |=  (1UL << ((i) % 64)))
#define bit_clear(a, i) (a[(i) / 64] &= ~(1UL << ((i) % 64)))

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
#define LENGTH 170000
    int NUM = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    static uint64_t flags[128 + 1];
    long i, k;
    int count = 0;

    while (NUM--) {
        // For 2
        count = 1;
        for (i=0; i <= 4096; i++) {
            bit_set(flags, i);
        }
        for (i=3; i <= 8192; i+=2) {
            if (bit_test(flags, i >> 1)) {
                /* remove all multiples of prime: i */
                for (k=i+i+i; k <= 8192; k+=i+i) {
                    bit_clear(flags, k >> 1);
                }
                count++;
            }
        }
    }
    printf("Count: %d\n", count);
    return(0);
}
