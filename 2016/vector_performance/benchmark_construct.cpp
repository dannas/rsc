
#include <benchmark/benchmark.h>
#include "vector.h"
#include <cstdlib>

using namespace std;

static void BM_Malloc(benchmark::State& state) {
    int N = state.range_x();

    while (state.KeepRunning()) {
        int* bigarray = static_cast<int*>(malloc(N * sizeof(int)));
        benchmark::DoNotOptimize(bigarray);
        free(bigarray);
    }
}
BENCHMARK(BM_Malloc)->Range(8, 8<<10);

static void BM_New(benchmark::State& state) {
    int N = state.range_x();

    while (state.KeepRunning()) {
        int* bigarray = new int[N];
        benchmark::DoNotOptimize(bigarray);
        delete [] bigarray;
    }
}
BENCHMARK(BM_New)->Range(8, 8<<10);

static void BM_VectorEmpty(benchmark::State& state) {
    int N = state.range_x();

    while (state.KeepRunning()) {
        vector<int> bigarray;
        benchmark::DoNotOptimize(bigarray.data());
    }
}
BENCHMARK(BM_VectorEmpty)->Range(8, 8<<10);

static void BM_VectorReserve(benchmark::State& state) {
    int N = state.range_x();

    while (state.KeepRunning()) {
        vector<int> bigarray;
        bigarray.reserve(N);
        benchmark::DoNotOptimize(bigarray.data());
    }
}
BENCHMARK(BM_VectorReserve)->Range(8, 8<<10);

static void BM_VectorResize(benchmark::State& state) {
    int N = state.range_x();

    while (state.KeepRunning()) {
        vector<int> bigarray;
        bigarray.resize(N);
        benchmark::DoNotOptimize(bigarray.data());
    }
}
BENCHMARK(BM_VectorResize)->Range(8, 8<<10);

static void BM_VectorConstruct(benchmark::State& state) {
    int N = state.range_x();

    while (state.KeepRunning()) {
        vector<int> bigarray(N);
        benchmark::DoNotOptimize(bigarray.data());
    }
}
BENCHMARK(BM_VectorConstruct)->Range(8, 8<<10);

static void BM_PodVectorConstruct(benchmark::State& state) {
    int N = state.range_x();

    while (state.KeepRunning()) {
        pod_vector<int> bigarray(N);
        benchmark::DoNotOptimize(bigarray.data());
    }
}
BENCHMARK(BM_PodVectorConstruct)->Range(8, 8<<10);

BENCHMARK_MAIN();
