#include <benchmark/benchmark.h>
#include "vector.h"

// Benchmark various ways of appending integers to a vector.
//
// Surprisingly to me, vector::push_back() is ~9x slower than
// access using iterators or indexes. Using reserve() + push_back() was
// as slow as just running push_back().
//
// We have 500x more page-faults for push_back() vs non-push_back().
//
// $ perf stat ./benchmark_append --benchmark-filter=BM_New | grep page-faults
// 126 page-faults
//
// $ perf stat ./benchmark_append --benchmark-filter=BM_VectorPushBack | grep page-faults
// 67,433 page-faults
//
// I guess it's because the OS does lazy loading of physical pages that have
// not yet been accessed. Reserve requests pages, but do not touch them.
//
// $ ./benchmark_append 
// Run on (8 X 840.023 MHz CPU s)
// 2016-07-25 21:44:42
// Benchmark                             Time           CPU Iterations
// -------------------------------------------------------------------
// BM_New/8                              3 ns          3 ns  243055556
// BM_New/64                            25 ns         25 ns   27777778
// BM_New/512                          154 ns        153 ns    4487179
// BM_New/4k                          1200 ns       1200 ns     583333
// BM_New/8k                          2400 ns       2400 ns     291667
// BM_VectorResizeIndex/8                3 ns          3 ns  216049383
// BM_VectorResizeIndex/64              25 ns         25 ns   28225806
// BM_VectorResizeIndex/512            155 ns        154 ns    4487179
// BM_VectorResizeIndex/4k            1201 ns       1200 ns     583333
// BM_VectorResizeIndex/8k            2393 ns       2386 ns     291667
// BM_VectorPushBack/8                  19 ns         19 ns   30701754
// BM_VectorPushBack/64                188 ns        187 ns    4375000
// BM_VectorPushBack/512              1486 ns       1486 ns     530303
// BM_VectorPushBack/4k               9763 ns       9744 ns      60345
// BM_VectorPushBack/8k              19448 ns      19412 ns      30702
// BM_VectorReservePushback/8           19 ns         19 ns   30701754
// BM_VectorReservePushback/64         186 ns        186 ns    4487179
// BM_VectorReservePushback/512       1503 ns       1501 ns     530303
// BM_VectorReservePushback/4k        9852 ns       9806 ns      58333
// BM_VectorReservePushback/8k       19415 ns      19456 ns      31250
// BM_VectorResizeIterator/8             4 ns          4 ns  190217391
// BM_VectorResizeIterator/64           27 ns         27 ns   25735294
// BM_VectorResizeIterator/512         159 ns        159 ns    4375000
// BM_VectorResizeIterator/4k         1200 ns       1200 ns     583333
// BM_VectorResizeIterator/8k         2395 ns       2384 ns     286885

using namespace std;

static void BM_New(benchmark::State& state) {
    int N = state.range_x();
    int* bigarray = new int[N];
    benchmark::DoNotOptimize(bigarray);

    while (state.KeepRunning()) {

        for (unsigned int k = 0; k<N; ++k) {
            bigarray[k] = k;
            benchmark::ClobberMemory();
        }
    }
    delete [] bigarray;
}
BENCHMARK(BM_New)->Range(8, 8<<10);

static void BM_VectorResizeIndex(benchmark::State& state) {
    int N = state.range_x();
    vector<int> bigarray(N);
    benchmark::DoNotOptimize(bigarray.data());

    while (state.KeepRunning()) {

        for (unsigned int k = 0; k<N; ++k) {
            bigarray[k] = k;
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_VectorResizeIndex)->Range(8, 8<<10);

static void BM_VectorPushBack(benchmark::State& state) {
    int N = state.range_x();
    vector<int> bigarray;
    benchmark::DoNotOptimize(bigarray.data());

    while (state.KeepRunning()) {

        for (unsigned int k = 0; k<N; ++k) {
            bigarray.push_back(k);
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_VectorPushBack)->Range(8, 8<<10);

static void BM_VectorReservePushback(benchmark::State& state) {
    int N = state.range_x();
    vector<int> bigarray;
    bigarray.reserve(N);
    benchmark::DoNotOptimize(bigarray.data());

    while (state.KeepRunning()) {

        for (unsigned int k = 0; k<N; ++k) {
            bigarray.push_back(k);
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_VectorReservePushback)->Range(8, 8<<10);

static void BM_VectorResizeIterator(benchmark::State& state) {
    int N = state.range_x();
    vector<int> bigarray;
    bigarray.resize(N);
    benchmark::DoNotOptimize(bigarray.data());

    while (state.KeepRunning()) {

        auto i = begin(bigarray);
        for(unsigned int k = 0; k<N; ++k) {
            *i++ = k;
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_VectorResizeIterator)->Range(8, 8<<10);


BENCHMARK_MAIN();
