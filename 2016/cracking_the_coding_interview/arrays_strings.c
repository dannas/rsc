
int compare_char(const void *x, const void *y) {
    return *(char*)x - *(char*)y;
}

bool unique_str_by_sorting(const char *str) {
    size_t len = strlen(str);
    char *copy = strdup(str);
    qsort(copy, len, 1, compare_char);

    bool uniq = true;
    char *prev = copy;
    for (char *ptr = copy+1; *ptr; ptr++) {
        if (*ptr == *prev) {
            uniq = false;
            break;
        }
        prev++;
    }
    free(copy);
    return uniq;
}

bool unique_str_by_hashing(const char * str) {
    Map map = {0};
    bool uniq = true;
    for (const char *ptr = str; *ptr; ptr++) {
        size_t n = map_get(&map, hash_uint64(*ptr));
        if (n > 0) {
            uniq = false;
            break;
        }
        map_put(&map, hash_uint64(*ptr), n+1);
    }
    return uniq;
}

bool unique_str_by_char_table(const char *str) {
    char table[256] = {0};

    for (const char *ptr = str; *ptr; ptr++) {
        size_t i = (size_t)*ptr;
        if (table[i]) {
            return false;
        }
        table[i]++;
    }
    return true;
}

bool unique_str_by_bitmask(const char *str) {
    uint64_t bitset[4] = {0};

    for (const char *ptr = str; *ptr; ptr++) {
        uint32_t n = *ptr;
        if (bit_test(bitset, n)) {
            return false;
        }
        bit_set(bitset, n);
    }
    return true;
}

void reverse(char *str) {
    if (*str == '\0') {
        return;
    }
    char *end = str;
    while (*end) {
        end--;
    }
    end--;
    while (str < end) {
        char t = *str;
        *str++ = *end;
        *end++ = t;
    }
}

void remove_duplicates(char *str) {
    uint64_t bitset[4] = {0};
    char *ins = str;
    for (; *str; str++) {
        size_t n = (size_t)*str;
        if (!bit_test(bitset, n)) {
            *ins++ = *str;
            bit_set(bitset, n);
        }
    }
    *ins = '\0';
}

bool is_anagram(const char *str1, const char *str2) {
    uint8_t str1_count[256] = {0};
    uint8_t str2_count[256] = {0};

    for (; *str1; str1++) {
        str1_count[(size_t)*str1]++;
    }
    for (; *str2; str2++) {
        str2_count[(size_t)*str2]++;
    }
    return memcmp(str1_count, str2_count, 256) == 0;
}

bool is_anagram_sort(const char *str1, const char *str2) {
    size_t str1_len = strlen(str1);
    size_t str2_len = strlen(str2);

    if (str1_len != str2_len) {
        return false;
    }

    char *copy1 = strdup(str1);
    char *copy2 = strdup(str2);
    qsort(copy1, str1_len, 1, compare_char);
    qsort(copy2, str2_len, 1, compare_char);
    bool ret = memcmp(copy1, copy2, str1_len) == 0;
    free(copy1);
    free(copy2);
    return ret;
}
char* replace_spaces(const char *str) {
    int num_spaces = 0;
    for (const char *ptr = str; *ptr; ptr++) {
        if (*ptr == ' ') {
            num_spaces++;
        }
    }
    size_t new_len = strlen(str) + num_spaces * 2;
    char *replaced = xmalloc(new_len+1);
    char *ins = replaced;
    for (const char *ptr = str; *ptr; ptr++) {
        if (*ptr == ' ') {
            *ins++ = '%';
            *ins++ = '2';
            *ins++ = '0';
        } else {
            *ins++ = *ptr;
        }
    }
    replaced[new_len] = '\0';
    return replaced;
}

void rotate_image(size_t len, uint32_t grid[len][len]) {
    uint32_t **copy = xmalloc(len * sizeof(grid[0]));
    for (size_t i = 0; i < len; i++) {
        copy[i] = xmalloc(len * sizeof(grid[0][0]));
    }

    for (size_t row = 0; row < len; row++) {
        for (size_t col = 0; col < len; col++) {
            copy[col][len-row-1] = grid[row][col];
        }
    }
    for (size_t row = 0; row < len; row++) {
        for (size_t col = 0; col < len; col++) {
            grid[row][col] = copy[row][col];
        }
    }

    for (size_t i = 0; i < len; i++) {
        free(copy[i]);
    }
    free(copy);
}

void rotate_image_inplace(size_t N, uint32_t grid[N][N]) {
    for (size_t layer = 0; layer < N / 2; layer++) {
        size_t first = layer;
        size_t last = N - layer - 1;
        for (size_t i = first; i < last; i++) {
            size_t offset = i - first;

            // save top
            uint32_t top = grid[i][first];
            // left -> top
            grid[i][first] = grid[last][i];
            // bottom -> left
            grid[last][i] = grid[last-offset][last];
            // right -> bottom
            grid[last-offset][last] = grid[first][last-offset];
            // top -> right
            grid[first][last-offset] = top;
        }
    }
}

void clear_matrix_row_col(size_t len, uint32_t grid[len][len]) {
    bool *rows = calloc(len, sizeof(rows[0]));
    bool *cols = calloc(len, sizeof(rows[0]));

    for (size_t row = 0; row < len; row++) {
        for (size_t col = 0; col < len; col++) {
            if (grid[row][col] == 0) {
                rows[row] = true;
                cols[col] = true;
            }
        }
    }
    for (size_t r = 0; r < len; r++) {
        for (size_t c = 0; c < len; c++) {
            if (rows[r] || cols[c]) {
                grid[r][c] = 0;
            }
        }
    }
    free(rows);
    free(cols);
}


void test_reverse(const char *str) {
    char *copy = strdup(str);
    reverse(copy);
    reverse(copy);
    assert(strcmp(str, copy) == 0);
    free(copy);
}

bool is_rotation(const char *str1, const char *str2) {
    size_t len = strlen(str1);

    if (len != strlen(str2)) {
        return false;
    }

    for (size_t start = 0; start < len; start++) {
        bool match = true;
        for (size_t i = 0; i < len; i++) {
            if (str1[i] != str2[(start+i) % len]) {
                match = false;
                break;
            }
        }
        if (match) {
            return true;
        }
    }
    return false;
}


void test_remove_duplicates(const char *str, const char *expected) {
    char *copy = strdup(str);
    remove_duplicates(copy);
    assert(strcmp(copy, expected) == 0);
    free(copy);
}

void test_replace_spaces(const char *str, const char *expected) {
    char *replaced = replace_spaces(str);
    assert(strcmp(replaced, expected) == 0);
}

#define assert_unique(str, expected) assert(uniq(str) == expected)
void test_arrays() {
#define uniq unique_str_by_sorting
    assert_unique("", true);
    assert_unique("a", true);
    assert_unique("aa", false);
    assert_unique("abab", false);
    assert_unique("abcdefghijklmnopqrstuvwxyz", true);

#undef uniq
#define uniq unique_str_by_hashing
    assert_unique("", true);
    assert_unique("a", true);
    assert_unique("aa", false);
    assert_unique("abab", false);
    assert_unique("abcdefghijklmnopqrstuvwxyz", true);

#undef uniq
#define uniq unique_str_by_bitmask
    assert_unique("", true);
    assert_unique("a", true);
    assert_unique("aa", false);
    assert_unique("abab", false);
    assert_unique("abcdefghijklmnopqrstuvwxyz", true);
#undef uniq
#undef assert_unique

    test_reverse("");
    test_reverse("a");
    test_reverse("ab");
    test_reverse("abc");
    test_reverse("abcdefghijklmnopqrstuvwxyz");

    test_remove_duplicates("", "");
    test_remove_duplicates("a", "a");
    test_remove_duplicates("aa", "a");
    test_remove_duplicates("abc", "abc");
    test_remove_duplicates("abcddd", "abcd");
    test_remove_duplicates("aaabbbcccddd", "abcd");
    test_remove_duplicates("abaabbbabcccdabcdd", "abcd");

    assert(is_anagram("", "") == true);
    assert(is_anagram("a", "a") == true);
    assert(is_anagram("a", "b") == false);
    assert(is_anagram("b", "a") == false);
    assert(is_anagram("abcdefgh", "dcbahgfe") == true);

    assert(is_anagram_sort("", "") == true);
    assert(is_anagram_sort("a", "a") == true);
    assert(is_anagram_sort("a", "b") == false);
    assert(is_anagram_sort("b", "a") == false);
    assert(is_anagram_sort("abcdefgh", "dcbahgfe") == true);

    test_replace_spaces("", "");
    test_replace_spaces("a", "a");
    test_replace_spaces("ab", "ab");
    test_replace_spaces(" ", "%20");
    test_replace_spaces("  ", "%20%20");
    test_replace_spaces(" a b", "%20a%20b");

    uint32_t grid3[][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9},
    };

    uint32_t rotated3[][3] = {
        {7, 4, 1},
        {8, 5, 2},
        {9, 6, 3},
    };

    uint32_t grid4[][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16},
    };

    uint32_t rotated4[][4] = {
        {13, 9, 5, 1},
        {14, 10, 6, 2},
        {15, 11, 7, 3},
        {16, 12, 8, 4},
    };
    rotate_image(3, grid3);
    assert(memcmp(grid3, rotated3, sizeof(grid3)) == 0);

    rotate_image_inplace(4, grid4);
    assert(memcmp(grid4, rotated4, sizeof(grid4)) == 0);

    uint32_t grid[][4] = {
        {0, 2, 3, 0},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16},
    };
    clear_matrix_row_col(4, grid);

    assert(is_rotation("abc", "abc") == true);
    assert(is_rotation("abc", "cab") == true);
    assert(is_rotation("abc", "xyz") == false);

    printf("all tests passed!\n");
}
