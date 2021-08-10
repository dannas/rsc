
#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define CLAMP_MIN(x, min) MAX((x), (min))
#define CLAMP_MAX(x, max) MIN((x), (max))

#define ALIGN_DOWN(n, a) ((n) & ~((a)-1))
#define ALIGN_UP(n, a) ALIGN_DOWN((n)+(a)-1, (a))
#define ALIGN_DOWN_PTR(p, a) ((void*)(ALIGN_DOWN((uintptr_t)(p), (a))))
#define ALIGN_UP_PTR(p, a) ((void*)(ALIGN_UP((uintptr_t)(p), (a))))

void fatal(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("FATAL: ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
    exit(1);
}

void *xmalloc(size_t num_bytes) {
    void *ptr = malloc(num_bytes);
    if (!ptr) {
        perror("xmalloc failed");
        exit(1);
    }
    return ptr;
}

void *xcalloc(size_t num_elems, size_t elem_size) {
    void *ptr = calloc(num_elems, elem_size);
    if (!ptr) {
        perror("calloc failed");
        exit(1);
    }
    return ptr;
}

void *xrealloc(void *ptr, size_t num_bytes) {
    ptr = realloc(ptr, num_bytes);
    if (!ptr) {
        perror("xrealloc failed");
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
#define buf__fit(b, n) ((n) <= buf_cap(b) ? 0 : ((b) = buf__grow(b, (n), sizeof(*b))))
#define buf_push(b,...) (buf__fit(b, buf_len(b)+1), (b)[buf__hdr(b)->len++] = (__VA_ARGS__))
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_end(b) ((b) + buf_len(b))
#define buf_free(b) ((b) ? (free(buf__hdr(b)), (b) = NULL) : 0)

void* buf__grow(void *buf, size_t new_len, size_t elem_size) {
    assert(buf_cap(buf) <= (SIZE_MAX-1)/2);
    size_t new_cap = CLAMP_MIN(2 * buf_cap(buf), MAX(new_len, 16));
    assert(new_len <= new_cap);
    assert(new_cap <= (SIZE_MAX - offsetof(BufHdr, buf))/elem_size);
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

typedef struct Arena {
    char *ptr;
    char *end;
    char **blocks;
} Arena;

#define ARENA_BLOCK_SIZE (1024 * 1024)
#define ARENA_ALIGNMENT 8

void arena_grow(Arena *arena, size_t min_size) {
    size_t size = ALIGN_UP(CLAMP_MIN(min_size, ARENA_BLOCK_SIZE), ARENA_ALIGNMENT);
    arena->ptr = xmalloc(size);
    arena->end = arena->ptr + size;
    assert(arena->ptr == ALIGN_DOWN_PTR(arena->ptr, ARENA_ALIGNMENT));
    buf_push(arena->blocks, arena->ptr);
}

void *arena_alloc(Arena *arena, size_t size) {
    if (size > (size_t)(arena->end - arena->ptr)) {
        arena_grow(arena, size);
        assert(size <= (size_t)(arena->end - arena->ptr));
    }

    void *ptr = arena->ptr;
    arena->ptr = ALIGN_UP_PTR(arena->ptr + size, ARENA_ALIGNMENT);
    assert(arena->ptr <= arena->end);
    assert(ptr == ALIGN_DOWN_PTR(ptr, ARENA_ALIGNMENT));

    return ptr;
}

void arena_free(Arena *arena) {
    for (char **it = arena->blocks; it != buf_end(arena->blocks); it++)
        free(*it);
    buf_free(arena->blocks);
}

typedef struct InternStr {
    size_t len;
    const char* str;
} InternStr;

static InternStr *interns;
static Arena str_arena;

const char* str_intern_range(const char* start, const char* end) {
    size_t len = end - start;

    for (InternStr *it = interns; it != buf_end(interns); it++) {
        if (len == it->len && strncmp(it->str, start, len) == 0) {
            return it->str;
        }
    }

    char *str = arena_alloc(&str_arena, len+1);
    memcpy(str, start, len);
    str[len] = '\0';
    buf_push(interns, (InternStr){len, str});
    return str;
}

const char* str_intern(const char* str) {
    return str_intern_range(str, str + strlen(str));
}

