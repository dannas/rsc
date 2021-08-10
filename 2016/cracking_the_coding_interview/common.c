#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define CLAMP_MIN(x, min) MAX((x), (min))
#define IS_POW2(x) ((x) && (((x) & ((x)-1)) == 0))

void* xmalloc(size_t num_bytes) {
    void *ptr = malloc(num_bytes);
    if (!ptr) {
        perror("malloc failed");
        exit(1);
    }
    return ptr;
}

void* xcalloc(size_t num_elems, size_t elem_size) {
    void *ptr = calloc(num_elems, elem_size);
    if (!ptr) {
        perror("calloc failed");
        exit(1);
    }
    return ptr;
}

void* xrealloc(void *ptr, size_t num_bytes) {
    ptr = realloc(ptr, num_bytes);
    if (!ptr) {
        perror("realloc failed");
        exit(1);
    }
    return ptr;
}

typedef struct BufHdr {
   size_t len;
   size_t cap;
   char buf[];
} BufHdr;

#define buf__hdr(b) ((BufHdr*)((uintptr_t)(b) - offsetof(BufHdr, buf)))
#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) (b ? buf__hdr(b)->cap : 0)
#define buf_fit(b, n) ((n) <= buf_cap(b) ? 0 : ((b) = buf_grow(b, n, sizeof(*(b)))))
#define buf_push(b, ...) (buf_fit(b, buf_len(b) + 1), (b)[buf__hdr(b)->len++] = __VA_ARGS__)
#define buf_end(b) ((b) + buf_len(b))
#define buf_pop(b) (buf__hdr(b)->len--, ((b)[buf_len(b) + 1]))

void* buf_grow(void *buf, size_t new_len, size_t elem_size) {

    size_t new_cap = CLAMP_MIN(2 * buf_cap(buf), MAX(new_len, 16));
    size_t new_size = offsetof(BufHdr, buf) + new_cap * elem_size;
    BufHdr *new_hdr;
    if (buf) {
        new_hdr = xrealloc(buf__hdr(buf), new_size);
    } else {
        new_hdr = xmalloc(new_size);
        new_hdr->len = 0;
    }
    new_hdr->cap = new_cap;
    return new_hdr->buf;
}


char *strdup(const char *str) {
    size_t len = strlen(str);
    char *copy = xmalloc(len + 1);
    memcpy(copy, str, len);
    copy[len] = '\0';
    return copy;
}
uint64_t hash_uint64(uint64_t x) {
    x *= 0xff51afd7ed558ccd;
    x ^= x >> 32;
    return x;
}
uint64_t hash_mix(uint64_t x, uint64_t y) {
    x ^= y;
    x *= 0xff51afd7ed558ccd;
    x ^= x >> 32;
    return x;
}

typedef struct Map {
    uint64_t *keys;
    uint64_t *vals;
    size_t cap;
    size_t len;
} Map;

uint64_t map_get(Map *map, uint64_t key) {
    if (map->len == 0) {
        return 0;
    }
    key = key ? key : 1;
    size_t i = (size_t)hash_uint64(key);
    assert(map->len < map->cap);
    for (;;) {
        i = i % map->cap;
        if (map->keys[i] == key) {
            return map->vals[i];
        }
        if (!map->keys[i]) {
            return 0;
        }
        i++;
    }
}

void map_grow(Map *map, size_t new_cap);

void map_put(Map *map, uint64_t key, uint64_t val) {
    key = key ? key : 1;
    if (!val) {
        return;
    }
    if (2 * map->len >= map->cap) {
        map_grow(map, 2 * map->cap);
    }
    assert(IS_POW2(map->cap));

    size_t i = (size_t)hash_uint64(key);
    for (;;) {
        i = i % map->cap;
        if (!map->keys[i]) {
            map->keys[i] = key;
            map->vals[i] = val;
            map->len++;
            return;
        }
        if (map->keys[i] == key) {
            map->vals[i] = val;
        }
        i++;
    }
}

void map_grow(Map *map, size_t new_cap) {
    size_t cap = CLAMP_MIN(new_cap, 16);
    Map new_map = {
        .keys = xcalloc(cap, sizeof(uint64_t)),
        .vals = xmalloc(cap * sizeof(uint64_t)),
        .cap = cap,
    };
    for (size_t i = 0; i < map->cap; i++) {
        if (map->keys[i]) {
            map_put(&new_map, map->keys[i], map->vals[i]);
        }
    }
    free(map->keys);
    free(map->vals);
    *map = new_map;
}

bool bit_test(uint64_t *bitset, size_t n) {
    return bitset[n/64] & (1UL << (n % 64));
}

void bit_set(uint64_t *bitset, size_t n) {
    bitset[n/64] |= 1UL << (n % 64);
}
