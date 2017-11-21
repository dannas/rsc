#include <benchmark/benchmark.h>
#include <sstream>

#include "vm.h"

using namespace std;


static void BM_InterpretAdd(benchmark::State& state) {

    char buf[] = R"(
        iconst 1
        iconst 2
        call .f
        print
        halt
        .def .f args=2, locals=0
        load 0
        load 1
        iadd
        ret
    )";

    stringstream as(buf);
    auto code = assemble(as);
    for (auto _ : state) {
        state.PauseTiming();
        stringstream ss;
        state.ResumeTiming();

        interpret(code, ss);
    }
}
BENCHMARK(BM_InterpretAdd);

BENCHMARK_MAIN();
