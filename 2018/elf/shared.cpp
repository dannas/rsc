int global_val = 42;

int add(int x, int y) {
    return x + y;
}

class Adder {
    virtual int add(int x, int y) {
        return x + y;
    }
};
