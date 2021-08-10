#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#define NELTS(a) (sizeof((a))/sizeof((a[0])))
#define MAX(a, b) ((a) >= (b) ? (a) : (b))

void *xmalloc(size_t num_bytes) {
    void *ptr = malloc(num_bytes);
    if (!ptr) {
        perror("malloc failed");
        exit(1);
    }
    return ptr;
}

typedef struct Node {
    int val;
    struct Node *left;
    struct Node *right;

} Node;

Node *new_node(int val) {
    Node *node = xmalloc(sizeof(Node));
    node->left = NULL;
    node->right = NULL;
    node->val = val;
    return node;
}

Node *insert(Node *node, int val) {
    if (!node) {
        return new_node(val);
    }
    if (node->val > val) {
        node->left = insert(node->left, val);
    } else {
        node->right = insert(node->right, val);
    }
    return node;
}

Node *build123() {
    Node *root = insert(NULL, 2);
    root = insert(root, 1);
    root = insert(root, 3);
    return root;
}

size_t size(Node *node) {
    if (!node) {
        return 0;
    }
    return size(node->left) + 1 + size(node->right);
}

size_t max_depth(Node *node) {
    if (!node) {
        return 0;
    }
    size_t l = max_depth(node->left);
    size_t r = max_depth(node->right);
    return 1 + MAX(l, r);
}

int min_value(Node *node) {
    assert(node);
    while (node->left) {
        node = node->left;
    }
    return node->val;
}

int max_value(Node *node) {
    assert(node);
    while (node->right) {
        node = node->right;
    }
    return node->val;
}

void print_tree(Node *node) {
    if (!node) {
        return;
    }
    print_tree(node->left);
    printf("%d ", node->val);
    print_tree(node->right);
}

void print_postorder(Node *node) {
    if (!node) {
        return;
    }
    print_tree(node->left);
    print_tree(node->right);
    printf("%d ", node->val);
}

bool has_path_sum(Node *node, int sum) {
    if (!node) {
        return sum == 0;
    }
    int r = sum - node->val;
    return has_path_sum(node->left, r) || has_path_sum(node->right, r);
}

void do_print_path(Node *node, int *paths, size_t path_len) {
    paths[path_len++] = node->val;
    if (node->left == NULL && node->right == NULL) {
        for (size_t i = 0; i < path_len; i++) {
            printf("%d ", paths[i]);
        }
        printf("\n");
        return;
    }
    do_print_path(node->left, paths, path_len);
    do_print_path(node->right, paths, path_len);
}

void print_paths(Node *node) {
    int paths[64];
    size_t path_len = 0;
    if (node) {
        do_print_path(node, paths, path_len);
    }
}

void mirror(Node *node) {
    if (!node){
        return;
    }
    Node *t = node->left;
    node->left = node->right;
    node->right = t;
    mirror(node->left);
    mirror(node->right);
}

void double_tree(Node *node) {
    if (!node) {
        return;
    }
    Node *dup = new_node(node->val);
    double_tree(node->left);
    dup->left = node->left;
    node->left = dup;
    double_tree(node->right);
}

bool same_tree(Node *a, Node *b) {
    if (a == NULL || b == NULL) {
        return a == b;
    }
    if (a->val !=b->val) {
        return false;
    }
    return same_tree(a->left, b->left) && same_tree(a->right, b->right);
}

int count_trees(size_t num_keys) {
    if (num_keys <= 1) {
        return 1;
    }
    int n = 0;
    for (size_t i = 0; i < num_keys; i++) {
        int left = count_trees(i);
        int right = count_trees(num_keys - i - 1);
        n += left * right;
    }
    return  n;
}

bool is_bst(Node *node) {
    if (!node) {
        return true;
    }
    is_bst(node->left);
    is_bst(node->right);


}

int main(void) {
    Node *root = build123();
    assert(root && root->val == 2);
    assert(root->left->val == 1);
    assert(root->right->val == 3);

    assert(size(NULL) == 0);
    assert(size(root) == 3);
    assert(max_depth(NULL) == 0);
    assert(max_depth(root) == 2);

    assert(min_value(root) == 1);
    assert(min_value(root->left) == 1);
    assert(min_value(root->right) == 3);

    print_tree(root);
    printf("\n");

    print_postorder(root);
    printf("\n");

    assert(has_path_sum(root, 0) == false);
    assert(has_path_sum(root, 1) == false);
    assert(has_path_sum(root, 3) == true);
    assert(has_path_sum(root, 4) == false);
    assert(has_path_sum(root, 5) == true);
    assert(has_path_sum(root, 6) == false);

    print_paths(root);

    mirror(root);
    assert(root && root->val == 2);
    assert(root->left->val == 3);
    assert(root->right->val == 1);

    double_tree(root);
    assert(root && root->val == 2);
    assert(root->left->val == 2);
    assert(root->left->left->val == 3);
    assert(root->left->left->left->val == 3);
    assert(root->right->val == 1);
    assert(root->right->left->val == 1);

    assert(count_trees(4) == 14);


    printf("all tests passed\n");
}
