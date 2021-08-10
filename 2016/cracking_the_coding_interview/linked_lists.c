
typedef struct Node {
    int val;
    struct Node *next;
} Node;


Node *node_new(int val) {
    Node *node = xmalloc(sizeof(*node));
    node->next = NULL;
    node->val = val;
    return node;
}
Node *node_append(Node *head, Node *node) {
    Node *prev = NULL;
    for (Node *p = head; p != NULL; p = p->next) {
        prev = p;
    }
    if (prev == NULL) {
        return node;
    }
    prev->next = node;
    return head;
}

Node *node_delete(Node *head, int val) {
    Node *prev = NULL;
    for (Node *p = head; p != NULL; p = p->next) {
        if (p->val == val) {
            if (prev == NULL) {
                head = p->next;
            } else {
                prev->next = p->next;
            }
            free(p);
            return head;
        }
        prev = p;
    }
    return head;
}

Node *node_lookup(Node *head, int val) {
    for (Node *p = head; p != NULL; p = p->next) {
        if (p->val == val) {
            return p;
        }
    }
    return NULL;
}

Node *list_remove_duplicates(Node *head) {
    Node *prev = NULL;
    for (Node *curr = head; curr != NULL; curr = curr->next) {
        bool match = false;
        for (Node *p = head; p != curr; p = p->next) {
            if (p->val == curr->val) {
                match = true;
                break;
            }
        }
        if (match) {
            if (prev == NULL) {
                head = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
        }
        prev = curr;
    }
    return head;
}

void test_list_remove_duplicates() {
    int a[] = {1, 2, 3, 4, 5};
    size_t len = sizeof(a)/sizeof(a[0]);
    Node *head = NULL;
    for (size_t i = 0; i < len; i++) {
        head = node_append(head, node_new(a[i]));
        head = node_append(head, node_new(a[i]));
    }
    head = list_remove_duplicates(head);

    for (size_t i = 0; i < len; i++) {
        assert(head);
        assert(a[i] == head->val);
        head = head->next;
    }
    assert(head == NULL);
}

void test_lists() {
    Node *head = NULL;
    for (size_t i = 0; i < 5; i++) {
        head = node_append(head, node_new(i));
    }

    for (size_t val = 0; val < 5; val++) {
        head = node_delete(head, val);
    }
    assert(head == NULL);
    test_list_remove_duplicates();
    printf("all list tests pass\n");
}

void list_delete_node(Node *node) {
    assert(node != NULL);
    assert(node->next != NULL);

    Node *next = node->next;
    node->next = next->next;
    node->val = next->val;
    free(next);
}

Node* list_add_numbers(Node *a, Node *b) {
    Node dummy = {.next = NULL};
    Node *tail = &dummy;
    int carry = 0;
    while (a != NULL|| b != NULL) {
        if (a && b) {
            int sum = a->val + b->val + carry;
            a->val = sum % 10;
            carry = sum / 10;
            tail->next = a;
            tail = tail->next;
            a = a->next;
            b = b->next;
        } else if (a == NULL) {
            int sum = b->val + carry;
            b->val = sum % 10;
            carry = sum / 10;
            tail->next = b;
            tail = tail->next;
            b = b->next;
        } else if (b == NULL) {
            int sum = a->val + carry;
            a->val = sum % 10;
            carry = sum / 10;
            tail->next = a;
            tail = tail->next;
            a = a->next;
        }
    }
    return dummy.next;
}
