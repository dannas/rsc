#include <benchmark/benchmark.h>
#include "vector.h"

// Benchmark various ways of appending integers to a vector.
//
// $ ./benchmark_append 
// Run on (8 X 3408.03 MHz CPU s)
// 2016-07-26 09:54:02
// Benchmark                              Time           CPU Iterations
// --------------------------------------------------------------------
// BM_New/8                              19 ns         26 ns   28225806
// BM_New/64                             86 ns         63 ns   10937500
// BM_New/512                           142 ns        187 ns    3723404
// BM_New/4k                           1679 ns       1233 ns     564516
// BM_New/32k                          7803 ns      10281 ns      67308
// BM_New/256k                       117875 ns      89368 ns       7609

// BM_VectorResizeIndex/8                19 ns         25 ns   27777778
// BM_VectorResizeIndex/64              116 ns         85 ns    8333333
// BM_VectorResizeIndex/512             257 ns        338 ns    2058824
// BM_VectorResizeIndex/4k             1847 ns       2426 ns     286885
// BM_VectorResizeIndex/32k           26938 ns      20516 ns      34314
// BM_VectorResizeIndex/256k         135784 ns     177938 ns       3889

// BM_VectorPushBack/8                  173 ns        130 ns    5833333
// BM_VectorPushBack/64                 306 ns        401 ns    1785714
// BM_VectorPushBack/512               3039 ns       2000 ns     324074
// BM_VectorPushBack/4k               10115 ns      13351 ns      53030
// BM_VectorPushBack/32k             136651 ns     102214 ns       6731
// BM_VectorPushBack/256k            915414 ns    1199269 ns        547

// BM_VectorReservePushback/8            34 ns         45 ns   15909091
// BM_VectorReservePushback/64          318 ns        240 ns    2966102
// BM_VectorReservePushback/512        1148 ns       1507 ns     448718
// BM_VectorReservePushback/4k        16310 ns      12116 ns      56452
// BM_VectorReservePushback/32k       74880 ns      98738 ns       7292
// BM_VectorReservePushback/256k    1099588 ns     835534 ns        833

// BM_VectorResizeIterator/8             22 ns         29 ns   23333333
// BM_VectorResizeIterator/64           134 ns         99 ns    7608696
// BM_VectorResizeIterator/512          259 ns        339 ns    1944444
// BM_VectorResizeIterator/4k          1856 ns       2440 ns     286885
// BM_VectorResizeIterator/32k        27029 ns      20516 ns      34314
// BM_VectorResizeIterator/256k      135604 ns     178966 ns       3889

using namespace std;

static void BM_New(benchmark::State& state) {
    int N = state.range_x();

    while (state.KeepRunning()) {
        int* bigarray = new int[N];
        benchmark::DoNotOptimize(bigarray);

        for (unsigned int k = 0; k<N; ++k) {
            bigarray[k] = k;
            benchmark::ClobberMemory();
        }
        delete [] bigarray;
    }
}
BENCHMARK(BM_New)->Range(8, 8<<15);

static void BM_VectorResizeIndex(benchmark::State& state) {
    int N = state.range_x();

    while (state.KeepRunning()) {
        vector<int> bigarray(N);
        benchmark::DoNotOptimize(bigarray.data());

        for (unsigned int k = 0; k<N; ++k) {
            bigarray[k] = k;
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_VectorResizeIndex)->Range(8, 8<<15);

static void BM_VectorPushBack(benchmark::State& state) {
    int N = state.range_x();

    while (state.KeepRunning()) {
        vector<int> bigarray;
        benchmark::DoNotOptimize(bigarray.data());

        for (unsigned int k = 0; k<N; ++k) {
            bigarray.push_back(k);
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_VectorPushBack)->Range(8, 8<<15);

static void BM_VectorReservePushback(benchmark::State& state) {
    int N = state.range_x();

    while (state.KeepRunning()) {
        vector<int> bigarray;
        bigarray.reserve(N);
        benchmark::DoNotOptimize(bigarray.data());

        for (unsigned int k = 0; k<N; ++k) {
            bigarray.push_back(k);
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_VectorReservePushback)->Range(8, 8<<15);

static void BM_VectorResizeIterator(benchmark::State& state) {
    int N = state.range_x();

    while (state.KeepRunning()) {
        vector<int> bigarray;
        bigarray.resize(N);
        benchmark::DoNotOptimize(bigarray.data());

        auto i = begin(bigarray);
        for(unsigned int k = 0; k<N; ++k) {
            *i++ = k;
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_VectorResizeIterator)->Range(8, 8<<15);


BENCHMARK_MAIN();
