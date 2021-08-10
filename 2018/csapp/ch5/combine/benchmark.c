#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stddef.h>

uint64_t now_ns;

void start() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    now_ns = ts.tv_sec * 1000000000UL + ts.tv_nsec;
}

uint64_t now() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000000000UL + ts.tv_nsec;
}

uint64_t elapsed() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000000000UL + ts.tv_nsec - now_ns;
}


#define IDENT 0
#define OP +
typedef float data_t;

typedef struct BufHdr {
    size_t len;
    size_t cap;
    char buf[];
} BufHdr;

#define buf__hdr(b)   ((BufHdr*)((uintptr_t)(b) - offsetof(BufHdr, buf)))
#define buf_len(b)    ((b) ? buf__hdr(b)->len : 0)

size_t length(data_t *a);

void combine1(data_t *a, data_t *sum) {
    sum = IDENT;
    for (size_t i = 0; i < length(a); i++) {
        *sum = *sum OP a[i];
    }
}

void combine2(data_t *a, data_t *sum) {
    sum = IDENT;
    size_t N = length(a);
    for (size_t i = 0; i < N; i++) {
        *sum = *sum OP a[i];
    }
}

void combine3(data_t *a, data_t *sum) {
    sum = IDENT;
    size_t N = length(a);
    for (size_t i = 0; i < N; i++) {
        *sum = *sum OP a[i];
    }
}

void combine4(data_t *a, data_t *sum) {
    data_t acc = IDENT;
    size_t N = length(a);
    for (size_t i = 0; i < N; i++) {
        acc = acc OP a[i];
    }
    *sum = acc;
}
