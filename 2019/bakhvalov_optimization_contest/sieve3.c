// Try unrolling first loop as well
// Did not work out.

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
#define LENGTH 170000
    int NUM = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    static char flags[8192 + 1];
    long i, k;
    int count = 0;

    while (NUM--) {
        count = 0; 
        for (i=2; i <= 8192-4; i++) {
            flags[i] = 1;
            flags[i+1] = 1;
            flags[i+2] = 1;
            flags[i+3] = 1;
        }
		for (; i <= 8192; i++) {
			flags[i] = 1;
		}
        for (i=2; i <= 8192; i++) {
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
