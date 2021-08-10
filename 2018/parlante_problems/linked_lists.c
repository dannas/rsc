#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#define NELTS(a) (sizeof((a))/sizeof((a[0])))

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
    struct Node *next;

} Node;

Node *node_new(int val) {
    Node *node = xmalloc(sizeof(*node));
    node->val = val;
    node->next = NULL;
    return node;
}

size_t length(Node *head) {
    size_t n = 0;
    for (Node *p = head; p != NULL; p = p->next) {
        n++;
    }
    return n;
}

void push(Node **head, int val) {
    Node *node = node_new(val);
    node->next = *head;
    *head = node;
}

Node *append(Node *head, Node *node) {
    node->next = NULL;
    if (head == NULL) {
        return node;
    }
    Node *p;
    for (p = head; p->next != NULL; p = p->next) {
        ;
    }
    p->next = node;
    return head;
}

Node *create(int *array, size_t len) {
    Node *head = NULL;
    for (size_t i = 0; i < len; i++) {
        head = append(head, node_new(array[i]));
    }
    return head;
}

Node *create123() {
    int a[] = {1, 2, 3};
    return create(a, NELTS(a));
}

size_t count(Node *head, int val) {
    size_t n = 0;
    for (Node *p = head; p != NULL; p = p->next) {
        if (p->val == val) {
            n++;
        }
    }
    return n;
}

void test_count() {
    int a[] = {1, 2, 1, 1};
    Node *head = create(a, NELTS(a));
    size_t n = count(head, 1);
    assert(n == 3);
}

int get_nth(Node *head, size_t index) {
    size_t i = 0;
    for (Node *p = head; p != NULL; p = p->next) {
        if (i == index) {
            return p->val;
        }
        i++;
    }
    assert(0);

}

void test_nth() {
    Node *head = create123();
    assert(1 == get_nth(head, 0));
    assert(2 == get_nth(head, 1));
    assert(3 == get_nth(head, 2));
}

Node *delete_list(Node *head) {
    Node *next;
    for (Node *p = head; p != NULL; p = next) {
        next = p->next;
        free(p);
    }
    return NULL;
}

void test_delete_list() {
    Node *head = create123();
    head = delete_list(head);
    assert(head == NULL);
}

int pop(Node **head) {
    assert(*head);
    int val = (*head)->val;
    *head = (*head)->next;
    return val;
}

void test_pop() {
    Node *head = create123();
    assert(1 == pop(&head));
    assert(2 == pop(&head));
    assert(3 == pop(&head));
    assert(0 == length(head));
}

void insert_nth(Node **head, size_t index, int val) {
    if (*head == NULL) {
        push(head, val);
        return;
    }
    Node *prev = NULL;
    size_t i = 0;
    for (Node *p = *head; p != NULL; p = p->next) {
        if (i == index) {
            Node *node = node_new(val);
            if (prev == NULL) {
                *head = node;
            } else {
                prev->next = node;
            }
            node->next = p;
            return;
        }
        prev = p;
        i++;
    }
    assert(0);
}

void insert_nth2(Node **head, size_t index, Node *node) {
    size_t i = 0;
    Node **pp;
    for (pp = head; *pp != NULL; pp = &((*pp)->next)) {
        if (i == index) {
            break;
        }
        i++;
    }
    assert(i == index);
    node->next = *pp;
    *pp = node;
}

void test_insert_nth() {
    int a[] = {0, 0, 0};
    Node *head = create(a, NELTS(a));
    insert_nth(&head, 0, 1);
    insert_nth(&head, 2, 2);
    insert_nth(&head, 4, 3);


    assert(1 == pop(&head));
    assert(0 == pop(&head));
    assert(2 == pop(&head));
    assert(0 == pop(&head));
    assert(3 == pop(&head));
    assert(0 == pop(&head));
    assert(0 == length(head));
}

// My code. Explicit handling of
// 		head == NULL
// 		insert first
// 		insert in midldle
// 		insert last
void sorted_insert(Node **head, Node *node) {
    if (*head == NULL) {
        *head = node;
        node->next = NULL;
        return;
    }
    Node *p;
    Node *prev = NULL;
    for (p = *head; p != NULL; p = p->next) {
        if (p->val >= node->val) {
            break;
        }
        prev = p;
    }
    if (prev == NULL) {
        node->next = *head;
        *head = node;
    } else {
        node->next = prev->next;
        prev->next = node;
    }
}

// Special case for head
void sorted_insert2(Node **head, Node *node) {
    if (*head == NULL || (*head)->val >= node->val) {
        node->next = *head;
        *head = node;
        return;
    }
    Node *p;
    for (p = *head; p->next != NULL; p = p->next) {
        if (p->next->val >= node->val) {
            break;
        }
    }
    node->next = p->next;
    p->next = node;
}

// Dummy node
void sorted_insert3(Node **head, Node *node) {
    Node dummy = {.next = *head};

    Node *p;
    for (p = &dummy; p->next != NULL; p = p->next) {
        if (p->next->val >= node->val) {
            break;
        }
    }
    node->next = p->next;
    p->next = node;
    *head = dummy.next;
}
// Local reference
void sorted_insert4(Node **head, Node *node) {
    Node **p;
    for (p = head; *p != NULL; p = &((*p)->next)) {
        if ((*p)->val >= node->val) {
            break;
        }
    }
    node->next = *p;
    *p = node;
}

void test_sorted_insert() {
    Node *head = create123();
    sorted_insert4(&head, node_new(0));
    sorted_insert4(&head, node_new(2));
    sorted_insert4(&head, node_new(4));

    assert(0 == pop(&head));
    assert(1 == pop(&head));
    assert(2 == pop(&head));
    assert(2 == pop(&head));
    assert(3 == pop(&head));
    assert(4 == pop(&head));
    assert(0 == length(head));
}

void insert_sort(Node **head) {
    Node *next;
    Node *new_head = NULL;
    for (Node *p = *head; p != NULL; p = next) {
        next = p->next;
        sorted_insert(&new_head, p);
    }
    *head = new_head;
}

void test_insert_sort() {
    int a[] = {3, 1, 2, 4, 1};
    Node *head = create(a, NELTS(a));
    insert_sort(&head);

    assert(1 == pop(&head));
    assert(1 == pop(&head));
    assert(2 == pop(&head));
    assert(3 == pop(&head));
    assert(4 == pop(&head));
    assert(0 == length(head));

}

void list_append(Node **a, Node **b) {
    Node **pp;
    for (pp = a; *pp != NULL; pp = &((*pp)->next)) {
        ;
    }
    *pp = *b;
    *b = NULL;
}

void test_list_append() {
    Node *a = create123();
    Node *b = create123();
    list_append(&a, &b);
    assert(1 == pop(&a));
    assert(2 == pop(&a));
    assert(3 == pop(&a));
    assert(1 == pop(&a));
    assert(2 == pop(&a));
    assert(3 == pop(&a));
    assert(0 == length(a));
    assert(b == NULL);
}

void front_back_split(Node *head, Node **first, Node **last) {
    size_t n = length(head);

    size_t mid = (n / 2.0) + 0.5;

    Node **pp;
    for (pp = &head; mid > 0; pp = &((*pp)->next)) {
        mid--;
    }
    *first = head;
    *last = *pp;
    *pp = NULL;
}

void test_front_back_split() {
    Node *head = create123();
    Node *first;
    Node *last;
    front_back_split(head, &first, &last);
    assert(1 == pop(&first));
    assert(2 == pop(&first));
    assert(0 == length(first));
    assert(3 == pop(&last));
    assert(0 == length(last));
}

void remove_duplicates(Node *head) {
    if (head == NULL) {
        return;
    }

    Node *next;
    for (Node *p = head; p->next != NULL; ){
        if (p->val == p->next->val) {
            next = p->next->next;
            free(p->next);
            p->next = next;
        } else {
            p = p->next;
        }
    }
}

void test_remove_duplicates() {
    int a[] = {1, 1, 2, 2, 3, 3, 3};
    Node *head = create(a, NELTS(a));
    remove_duplicates(head);
    assert(1 == pop(&head));
    assert(2 == pop(&head));
    assert(3 == pop(&head));
    assert(0 == length(head));

}

void move_node(Node **dst, Node **src) {
    assert(src && *src);
    Node *p = *src;
    *src = (*src)->next;
    p->next = *dst;
    *dst = p;
}

void test_move_node() {
    Node *a = create123();
    Node *b = create123();
    move_node(&a, &b);
    assert(1 == pop(&a));
    assert(1 == pop(&a));
    assert(2 == pop(&a));
    assert(3 == pop(&a));
    assert(0 == length(a));

    assert(2 == pop(&b));
    assert(3 == pop(&b));
    assert(0 == length(b));
}

void alternating_split2(Node *src, Node **a, Node **b) {
    Node *pa = NULL;
    Node *pb = NULL;
    while (src != NULL) {
        move_node(&pa, &src);
        if (src != NULL) {
            move_node(&pb, &src);
        }
    }
    *a = pa;
    *b = pb;
}

void test_alternating_split() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7};
    Node *src = create(arr, NELTS(arr));
    Node *a;
    Node *b;
    alternating_split2(src, &a, &b);
    assert(7 == pop(&a));
    assert(5 == pop(&a));
    assert(3 == pop(&a));
    assert(1 == pop(&a));
    assert(0 == length(a));

    assert(6 == pop(&b));
    assert(4 == pop(&b));
    assert(2 == pop(&b));
    assert(0 == length(b));
}

Node *shuffle_merge(Node *a, Node *b) {
    Node *head = NULL;
    Node *p = head;
    while (a != NULL || b != NULL) {
        if (a) {
            if (head) {
                p->next = a;
                p = p->next;
            } else {
                head = p = a;
            }
            a = a->next;
        }
        if (b) {
            if (head) {
                p->next = b;
                p = p->next;
            } else {
                head = p = b;
            }
        }
        b = b->next;
    }
    if (p) {
        p->next = NULL;
    }
    return head;
}

Node* shuffle_merge2(Node *a, Node *b) {
    Node dummy  = {.next = NULL};

    Node *tail = &dummy;

    while (a != NULL || b != NULL) {
        if (a) {
            tail->next = a;
            tail = tail->next;
            a = a->next;
        }
        if (b) {
            tail->next = b;
            tail = tail->next;
            b = b->next;
        }
    }
    return dummy.next;
}

void test_shuffle_merge() {
    Node *a = create123();
    int arr_b[] = {1, 2, 3, 4};
    Node *b = create(arr_b, NELTS(arr_b));

    Node *head = shuffle_merge2(a, b);
    assert(1 == pop(&head));
    assert(1 == pop(&head));
    assert(2 == pop(&head));
    assert(2 == pop(&head));
    assert(3 == pop(&head));
    assert(3 == pop(&head));
    assert(4 == pop(&head));
    assert(0 == length(head));
}

Node *sorted_merge(Node *a, Node *b) {
    Node dummy = {.next = NULL};
    Node *tail = &dummy;
    while (a != NULL || b != NULL) {
        if (a && b) {
            if (a->val <= b->val) {
                tail->next = a;
                tail = tail->next;
                a = a->next;
            } else {
                tail->next = b;
                tail = tail->next;
                b = b->next;
            }

        } else if (a) {
            tail->next = a;
            tail = tail->next;
            a = a->next;

        } else {
            tail->next = b;
            tail = tail->next;
            b = b->next;
        }
    }
    return dummy.next;
}

Node *sorted_merge2(Node *a, Node *b) {
    Node dummy = {.next = NULL};
    Node *tail = &dummy;
    Node **pp;
    while (a != NULL || b != NULL) {
        if (a && b) {
            if (a->val <= b->val) {
                pp = &a;
            } else {
                pp = &b;
            }

        } else if (a) {
            pp = &a;

        } else {
            pp = &b;
        }
        tail->next = *pp;
        tail = tail->next;
        *pp = (*pp)->next;
    }
    return dummy.next;
}

Node *sorted_merge3(Node *a, Node *b) {
    Node dummy = {.next = NULL};
    Node *tail = &dummy;
    while (a != NULL || b != NULL) {
        if (a == NULL) {
            tail->next = b;
            break;
        } else if (b == NULL) {
            tail->next = a;
            break;
        } else {
            if (a->val <= b->val) {
                tail->next = a;
                tail = tail->next;
                a = a->next;
            } else {
                tail->next = b;
                tail = tail->next;
                b = b->next;
            }
        }
    }
    return dummy.next;
}

void test_sorted_merge() {
    int arr_a[] = {1, 3, 5};
    int arr_b[] = {2, 4, 6};
    Node *a = create(arr_a, NELTS(arr_a));
    Node *b = create(arr_b, NELTS(arr_b));
    Node *head = sorted_merge3(a, b);

    assert(1 == pop(&head));
    assert(2 == pop(&head));
    assert(3 == pop(&head));
    assert(4 == pop(&head));
    assert(5 == pop(&head));
    assert(6 == pop(&head));
    assert(0 == length(head));
}

void merge_sort(Node **head) {
    if (*head == NULL || (*head)->next == NULL) {
        return;
    }

    Node *a;
    Node *b;

    front_back_split(*head, &a, &b);
    merge_sort(&a);
    merge_sort(&b);
    *head = sorted_merge3(a, b);
}

void test_merge_sort() {
    Node *head = NULL;
    merge_sort(&head);
    assert(0 == length(head));

    int a[] = {4, 2, 5, 1};

    head = create(a, 1);
    merge_sort(&head);
    assert(4 == pop(&head));
    assert(0 == length(head));

    head = create(a, 2);
    merge_sort(&head);
    assert(2 == pop(&head));
    assert(4 == pop(&head));
    assert(0 == length(head));

    head = create(a, 3);
    merge_sort(&head);
    assert(2 == pop(&head));
    assert(4 == pop(&head));
    assert(5 == pop(&head));
    assert(0 == length(head));

    head = create(a, NELTS(a));
    merge_sort(&head);
    assert(1 == pop(&head));
    assert(2 == pop(&head));
    assert(4 == pop(&head));
    assert(5 == pop(&head));
    assert(0 == length(head));
}

Node *sorted_intersect(Node *a, Node *b) {
    Node dummy = {.next = NULL};
    Node *tail = &dummy;
    while (a != NULL && b != NULL) {
        if (a->val == b->val) {
            push(&tail->next, a->val);
            tail = tail->next;
            a = a->next;
            b = b->next;
        } else if (a->val < b->val) {
            a = a->next;
        } else {
            b = b->next;
        }
    }
    return dummy.next;
}

void test_sorted_intersect() {
    int arr_a[] = {1, 2, 3, 4, 5, 6};
    int arr_b[] = {1, 3, 6};
    Node *a = create(arr_a, NELTS(arr_a));
    Node *b = create(arr_b, NELTS(arr_b));
    Node *head = sorted_intersect(a, b);

    assert(1 == pop(&head));
    assert(3 == pop(&head));
    assert(6 == pop(&head));
    assert(0 == length(head));
}

void reverse(Node **head) {
    Node *next;
    Node *prev = NULL;
    for (Node *p = *head; p != NULL; p = next ) {
        next = p->next;
        p->next = prev;
        prev = p;
    }
    *head = prev;
}

void test_reverse() {
    Node *head = create123();
    reverse(&head);

    assert(3 == pop(&head));
    assert(2 == pop(&head));
    assert(1 == pop(&head));
    assert(0 == length(head));
}

void recursive_reverse(Node **head) {
    if (*head == NULL || (*head)->next == NULL) {
        return;
    }
    Node *rest = (*head)->next;

    recursive_reverse(&rest);
    (*head)->next->next = *head;
    (*head)->next = NULL;
    *head = rest;
}

void test_recursive_reverse() {
    Node *head = create123();
    recursive_reverse(&head);

    assert(3 == pop(&head));
    assert(2 == pop(&head));
    assert(1 == pop(&head));
    assert(0 == length(head));
}

int main(void) {
    test_count();
    test_nth();
    test_delete_list();
    test_pop();
    test_insert_nth();
    test_sorted_insert();
    test_insert_sort();
    test_list_append();
    test_front_back_split();
    test_remove_duplicates();
    test_move_node();
    test_alternating_split();
    test_shuffle_merge();
    test_sorted_merge();
    test_merge_sort();
    test_sorted_intersect();
    test_reverse();
    test_recursive_reverse();

    printf("all tests passed\n");
}
