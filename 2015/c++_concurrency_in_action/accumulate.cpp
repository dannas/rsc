#include <thread>
#include <algorithm>
#include <cstdint>
#include <iostream>

using std::accumulate;
using std::min;
using std::thread;
using std::vector;

uint64_t threads(int size, uint64_t minsize)
{
    uint64_t N = thread::hardware_concurrency();
    uint64_t max = (size + minsize - 1) / minsize;
    return min(N != 0 ? N : 2 , max);
}

template<typename Iterator, typename T>
T parallell_accumulate(Iterator first, Iterator last, T init)
{
    uint64_t N = distance(first, last);

    if (N == 0)
        return init;

    uint64_t nthreads = threads(N, 25);
    uint64_t block_size = N / nthreads;

    vector<T> results(nthreads);
    vector<thread> threads(nthreads-1);

    auto f = [](Iterator b, Iterator e, T& result) {
        result = accumulate(b, e, result);
    };

    // accumulate all but one block...
    auto b = first;
    for (uint64_t i = 0; i < nthreads-1; ++i) {
        auto e = b;
        advance(e, block_size);
        threads[i] = thread(f, b, e, std::ref(results[i]));
        b = e;
    }

    // .. Last block.
    f(b, last, std::ref(results[nthreads-1]));

    for (auto &t : threads)
        t.join();

    return accumulate(begin(results), end(results), init);
}

int main(int argc, char *argv[])
{
    vector<int> v(1e6, 1);

    std::cout << parallell_accumulate(begin(v), end(v), 0) << "\n";
}
