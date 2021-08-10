#define NUM_STACK 10
#if 0
int stack[3 * NUM_STACK];
int* stack_ptr[3] = {stack, stack+NUM_STACK, stack+2*NUM_STACK};

void push(size_t index, int val) {
    assert(index < 3);
    int **sp = &stack_ptr[index];
    assert(*sp < stack + (index+1) * NUM_STACK);
    **sp = val;
    (*sp)++;
}

int pop(size_t index) {
    assert(index < 3);
    int **sp = &stack_ptr[index];
    assert(*sp > stack + index * NUM_STACK);
    (*sp)--;
    return **sp;
}

int stack[NUM_STACK];
size_t stack_ptr[3] = {0*NUM_STACK, 1*NUM_STACK, 2*NUM_STACK};

void push(size_t index, int val) {
    assert(index < 3);
    size_t *sp = &stack_ptr[index];
    assert(*sp < (index+1) * NUM_STACK);
    stack[(*sp)++] = val;
}

int pop(size_t index) {
    assert(index < 3);
    size_t *sp = &stack_ptr[index];
    assert(*sp > 0);
    --(*sp);
    return stack[*sp];
}

int stack[3 * NUM_STACK];
size_t stack_ptr[3];

void push(size_t stack_index, int val) {
    assert(stack_index < 3);
    assert(stack_ptr[stack_index] < NUM_STACK);

    size_t i = stack_index * NUM_STACK + stack_ptr[stack_index];
    stack_ptr[stack_index]++;
    stack[i] = val;
}

int pop(size_t stack_index) {
    assert(stack_index < 3);
    assert(stack_ptr[stack_index] > 0);

    size_t i = stack_index * NUM_STACK + stack_ptr[stack_index];
    int val = stack[i-1];
    stack_ptr[stack_index]--;
    return val;
}

#define ALIGN_DOWN(n, a) ((n) & ~((1 << (a)) -1))
#define ALIGN_UP(n, a) ALIGN_DOWN((n) + ((1 << (a)) - 1), (a))

typedef struct Node {
    size_t prev;
    int val;
} Node;

Node stack[3 * NUM_STACK];
size_t unused_index = 1;
size_t stack_ptr[3] = {0, 0, 0};

#define NUM_WORDS(len) ALIGN_UP((len) / 64, 8)
uint64_t freelist[ALIGN_UP(3 * NUM_STACK / 64, 8)];

void bit_set(uint64_t *bitset, size_t i) {
    bitset[i / 64] |= (1UL << (i % 64));
}

bool bit_test(uint64_t *bitset, size_t i) {
    return bitset[i / 64] & (1UL << (i % 64));
}

void bit_clear(uint64_t *bitset, size_t i) {
    bitset[i / 64] &= ~(1UL << (i % 64));
}

ssize_t find_first_set_bit(uint64_t *bitset, size_t num_words) {
    for (size_t i = 0; i < num_words * 64; i++) {
        if (bit_test(bitset, i)) {
            return i;
        }
    }
    return -1;
}

void push(size_t stack_index, int val){
    assert(stack_index < 3);

    size_t i = unused_index++;
    stack[i].prev = stack_ptr[stack_index];
    stack_ptr[stack_index] = i;
    stack[i].val = val;
}

int pop(size_t stack_index) {
    assert(stack_index < 3);
    size_t i = stack_ptr[stack_index];
    assert(stack[i].prev != i);
    stack_ptr[stack_index] = stack[i].prev;
    return stack[i].val;
}

int stack[NUM_STACK];
size_t stack_index = 0;

size_t min_stack[NUM_STACK];
size_t min_stack_index = 0;

void push(int val) {
    assert(min_stack_index <= stack_index);
    assert(stack_index < NUM_STACK);

    if (min_stack_index == 0 || val < stack[min_stack[min_stack_index-1]]) {
        min_stack[min_stack_index++] = stack_index;
    }
   stack[stack_index++] = val;
}

int pop() {
    assert(min_stack_index <= stack_index);
    assert(stack_index > 0);

    stack_index--;
    if (min_stack_index > 0 && min_stack[min_stack_index-1] == stack_index) {
        min_stack_index--;
    }
    return stack[stack_index];
}

int min() {
    assert(min_stack_index <= stack_index);
    assert(min_stack_index > 0);

    return min_stack[min_stack_index-1];
}

int **stacks = NULL;
size_t stack_index;

void push(int val) {
    if (buf_len(stacks) == 0 || stack_index == NUM_STACK) {
        buf_push(stacks, xmalloc(NUM_STACK));
        stack_index = 0;
    }
    int **curr = buf_end(stacks) - 1;
    (*curr)[stack_index++] = val;
}

int pop() {
    assert(buf_len(stacks) > 0);

    if (stack_index == 0) {
        stack_index = NUM_STACK;
        (void)buf_pop(stacks);
        assert(buf_len(stacks) > 0);
    }
    int **curr = buf_end(stacks) - 1;
    return (*curr)[--stack_index];
}

int stacks[3][NUM_STACK];

int *sp[3] = {
    [0] = stacks[0],
    [1] = stacks[1],
    [2] = stacks[2],
};

void hanoi(int n, int *from, int *to, int *aux) {
    if (n == 1) {
        *to++ = *--from;
        return;
    }
    hanoi(n-1, from, aux, to);
    *to++ = *--from;
    hanoi(n-1, aux, to, from);
}
#endif

typedef struct Stack {
    int arr[NUM_STACK];
    size_t num_items;
} Stack;

void push(Stack *stack, int val) {
    stack->arr[num_items++] = val;
}

int pop(Stack *stack) {
    return stack->arr[--num_items];
}

typedef struct Queue {
    Stack s1;
    Stack s2;
} Queue;

void enqueue(Queue *q, int val) {
    Stack *s = &q->s1;
    s->arr[s->num_items++] = val;
}

int dequeue(Queue *q) {
    if (->s2 != q->stack2) {
        return pop(s2);
    }
    while (q->s1 != q->stack1) {
        push(s2, pop(s1));
    }
    return pop(s2);
}

void test_stacks_queues() {
    Queue queue = {
        .stack1 = xmalloc(NUM_STACK),
        .stack2 = xmalloc(NUM_STACK),
        .sp1 = queue.stack1,
        .sp2 = queue.stack2,
    };
    enqueue(&queue, 1);
    enqueue(&queue, 2);
    assert(1 == dequeue(&queue));
    enqueue(&queue, 3);
    assert(2 == dequeue(&queue));
    enqueue(&queue, 4);
    enqueue(&queue, 5);
    enqueue(&queue, 6);
    assert(3 == dequeue(&queue));
    assert(4 == dequeue(&queue));
    assert(5 == dequeue(&queue));
    assert(6 == dequeue(&queue));
    assert(queue.sp1 == queue.stack1 && queue.sp2 == queue.stack2);

}
