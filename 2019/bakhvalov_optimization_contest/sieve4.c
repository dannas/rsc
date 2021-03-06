// Unroll inner loop by 4x
// Unroll outer loop by 2x

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
#define LENGTH 170000
    int NUM = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    static char flags[8192 + 1];
    long i, j, k;
    int count = 0;

    while (NUM--) {
        count = 0; 
        for (i=2; i <= 8192; i++) {
            flags[i] = 1;
        }
        for (i=2; i <= 8192-1; i+= 2) {
			j = i+1;
            if (flags[i]) {
                /* remove all multiples of prime: i */
                int limit = 8192 - 4 * i;
                for (k=i+i; k <= limit; k+=4 * i) {
                    flags[k] = 0;
                    flags[k+i] = 0;
                    flags[k+i+i] = 0;
                    flags[k+i+i+i] = 0;
                }
                for (; k <= 8192; k+=i) {
                    flags[k] = 0;
                }

                count++;
            }
            if (flags[j]) {
                /* remove all multiples of prime: j */
                int limit = 8192 - 4 * j;
                for (k=j+j; k <= limit; k+=4 * j) {
                    flags[k] = 0;
                    flags[k+j] = 0;
                    flags[k+j+j] = 0;
                    flags[k+j+j+j] = 0;
                }
                for (; k <= 8192; k+=j) {
                    flags[k] = 0;
                }

                count++;
            }
        }
        for (; i <= 8192; i++) {
            if (flags[i]) {
                /* remove all multiples of prime: i */
                int limit = 8192 - 4 * i;
                for (k=i+i; k <= limit; k+=4 * i) {
                    flags[k] = 0;
                    flags[k+i] = 0;
                    flags[k+i+i] = 0;
                    flags[k+i+i+i] = 0;
                }
                for (; k <= 8192; k+=i) {
                    flags[k] = 0;
                }

                count++;
            }
        }
    }
    printf("Count: %d\n", count);
    return(0);
}
