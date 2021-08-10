#include <stdio.h>

int uninitialized;
int initialized = 1;
static int static_initialized = 2;

const int const_uninitialized;
const int const_initialized = 5;
static const int static_const_initialized = 6;

void func() {
}

static void static_func() {
}

int main(int argc, char *argv[])
{
    static int local_static_uninitialized;
    static int local_static_initialized = 3;
    const static int local_const_static_initialized = 3;
    const static int local_const_static_uninitialized;
	printf("hello world %d\n", static_initialized);
	return 0;
}
