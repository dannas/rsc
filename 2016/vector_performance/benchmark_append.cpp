#include <benchmark/benchmark.h>
#include "vector.h"

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
