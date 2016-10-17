#include <iostream>
#include <cassert>

using namespace std;

// TODO(dannas): Use unsigned arithmetic for calculating offsets
// https://fgiesen.wordpress.com/2010/12/14/ring-buffers-and-queues/

// Or use bitwise tricks for powers-of-2.
// https://www.kernel.org/doc/Documentation/circular-buffers.txt

// Uses regular indices. Upon each |enqueue| and |dequeue| call, we make sure
// that we properly wrap around the indices.
template <typename T>
class CyclicQueue1 {
public:
    CyclicQueue1(int capacity)
        : buf(new T[capacity]) 
        , cap(capacity) 
        , head(0)
        , tail(0)
    {
            assert(capacity % 2 == 0);
    }

    ~CyclicQueue1() {
        delete[] buf;
    }

    void enqueue(T val) {
        buf[head] = val;

        head = (head + 1) % cap;

        if (head == tail) {
            tail = (tail + 1) % cap;
        }
    }

    T dequeue() {
        T val = buf[tail];
        tail = (tail + 1) % cap;
        return val;
    }

    void isEmpty() {
        return head == tail;
    }

    // A proof of this formula:
    //
    //     tail   head
    //       v      v
    //     +-+-+-+-+-+-+-+
    //     | |x|x|x| | | |
    //     +-+-+-+-+-+-+-+
    //
    //       head tail
    //        v   v
    //     +-+-+-+-+-+-+-+
    //     |x| | |x|x|x|x|
    //     +-+-+-+-+-+-+-+
    //
    //     First case:      h - t = 5 - 1 = 4
    //     Second case: N - t + h = 7 - 3 + 1 = 5
    //
    // Rearrange the expressions
    //      
    //       h - t
    //   N + h - t
    //
    // Since both are bounded to be in the range [0, N), we can calculate
    // them modulo N.
    //
    //       (h - t) % N
    //   (N + h - t) % N
    //
    // Adding N to the first expression doesn't change its result; then they 
    // are identical:
    //
    //   (N + h - t) % N
    //   (N + h - t) % N
    void size() {
        return (cap + head - tail) % cap;
    }

private:
    T* buf;            // The buffer holding the elements.
    size_t cap;        // Maximum number of elements.
    size_t head;       // The write position.
    size_t tail;       // The read position.
};

// Uses virtual indices; the indices are incremented upon each |enqueue| 
// call. Relies on unsigned integer wrap around and that the capacity is a 
// power-of-two.
// Simplifies the specfication of class invariants; the head is always larger
// or equal to the tail.
template <typename T>
class CyclicQueue2 {
public:
    CyclicQueue2(int capacity)
        : buf(new T[capacity]) 
        , cap(capacity) 
        , head(0)
        , tail(0)
    {
            assert(capacity % 2 == 0);
    }

    ~CyclicQueue2() {
        delete[] buf;
    }

    void enqueue(T val) {
        buf[head % cap] = val;

        head++;

        if (head == tail)
            tail++;
    }

    T dequeue() {

        T val = buf[tail % cap];
        tail++;
        return val;
    }

    void isEmpty() {
        return head == tail;
    }

    void size() {
        return (head - tail) % cap;
    }

private:
    T* buf;            // The buffer holding the elements.
    size_t cap;        // Maximum number of elements.
    size_t head;       // The write position.
    size_t tail;       // The read position.
};

template <typename T>
class CyclicQueue3 {
public:
    CyclicQueue3(int capacity)
        : buf(new T[capacity]) 
        , cap(capacity) 
        , head(0)
        , tail(0)
    {
            assert(capacity % 2 == 0);
    }

    ~CyclicQueue3() {
        delete[] buf;
    }

    void enqueue(T val) {
        buf[head] = val;

        head = (head + 1) % cap;

        if (head == tail) {
            tail = (tail + 1) % cap;
        }
    }

    T dequeue() {
        T val = buf[tail];
        tail = (tail + 1) % cap;
        return val;
    }

    void isEmpty() {
        return head == tail;
    }

    // An optimization of CyclicQueue::size().
    //
    //    return (cap + head - tail) % cap;
    //
    // If C++'s modulo operator followed the mathematical definition of modulo
    // then the above could have been simplified to
    //
    //    return (head - tail) % cap;
    //
    // But C++ evalues expressions with negative dividens as negative; it's a 
    // remainder operation rather than a modulo operation.
    // But if the capacity of the queue is an even number, we can work around the 
    // modulo limitation.
    void size() {
        return (head - tail) & (cap - 1);
    }

private:
    T* buf;            // The buffer holding the elements.
    size_t cap;        // Maximum number of elements.
    size_t head;       // The write position.
    size_t tail;       // The read position.
};

int main() {
    CyclicQueue1<int> c(10);
    c.enqueue(1);
    c.enqueue(3);
    c.enqueue(5);
    for (int i = 0; i < 100; i++)
        c.enqueue(i);
}
