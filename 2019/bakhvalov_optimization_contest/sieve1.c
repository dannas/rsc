// Copied from baseline

#define bit_test(a, i) (a[(i) / 64] & (1UL << ((i) % 64)))
#define bit_set(a, i) (a[(i) / 64] |= (1UL << ((i) % 64)))
#define bit_clear(a, i) (a[(i) / 64] &= ~(1UL << ((i) % 64)))

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
#define LENGTH 170000
    int NUM = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    static uint64_t flags[128 + 1];  // 8192 / 64
    long i, k;
    int count = 0;

    while (NUM--) {
        count = 0; 
        for (i=2; i <= 8192; i++) {
            bit_set(flags, i);
        }
        for (i=2; i <= 8192; i++) {
            if (bit_test(flags, i)) {
                /* remove all multiples of prime: i */
                for (k=i+i; k <= 8192; k+=i) {
                    bit_clear(flags, k);
                }
                count++;
            }
        }
    }
    printf("Count: %d\n", count);
    return(0);
}
