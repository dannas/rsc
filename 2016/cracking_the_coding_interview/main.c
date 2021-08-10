#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>

#include "common.c"

#if 0
#include "arrays_strings.c"
#include "linked_lists.c"
#include "stacks_queues.c"
#endif

typedef struct Node {
    int val;
    struct Node *left;
    struct Node *right;
} Node;

Node *new_node(int val) {
    Node *node = xcalloc(1, sizeof(*node));
    node->val = val;
    return node;
}

Node *insert(Node *node, int val) {
    if (node == NULL) {
        return new_node(val);
    }
    if (node->val > val) {
        node->left = insert(node->left, val);
    } else if (node->val < val) {
        node->right = insert(node->right, val);
    }
    return node;
}

int max_depth(Node *node) {
    if (node == NULL) {
        return 0;
    }
    return 1 + MAX(max_depth(node->left), max_depth(node->right));
}

int min_depth(Node *node) {
    if (node == NULL) {
        return 0;
    }
    return 1 + MIN(min_depth(node->left), min_depth(node->right));
}

bool is_balanced(Node *node) {
    return max_depth(node) - min_depth(node) <= 1;
}

typedef struct Queue {
    int *buf;
    size_t head;
    size_t tail;
    size_t cap;
} Queue;

bool enqueue(Queue *q, int val) {
    q->head = (q->head + 1) % q->cap;
    if (q->head == q->tail) {
        return false;
    }
    q->buf[q->head] = val;
    return true;
}

int dequeu(Queue *q) {
    assert(q->head != q->tail);
    int val = q->buf[q->tail];
    q->tail = q->tail == 0 ? q->cap-1 : q->tail - 1;
    return val;
}


bool has_route(int **G, size_t V) {
    bool *visited = xmalloc(V * sizeof(bool));
    int *queue = xmalloc(V * sizeof(int));

    for (size_t i = 0; i < V; i++) {
        memset(visited, false, V);

        while ()


    }


}

int main() {
    Node *root = NULL;
    root = insert(root, 1);
    root = insert(root, 2);
    root = insert(root, 3);
    root = insert(root, 4);
    root = insert(root, 1);
    assert(is_balanced(root) == false);

    int **G =  xcalloc(10, sizeof(int*));
    buf_push(G[0], 1);
    buf_push(G[0], 2);
    buf_push(G[1], 3);

    printf("all tests passed\n");
}
