// ============================================================================
// /home/dannas/code/rsc/2015/string_performance/stringtest.cpprelative  time/iter  iters/s
// ============================================================================
// operatorPlus                                               396.94ns    2.52M
// operatorPlusAssign                                          21.80ns   45.86M
// append                                                      20.15ns   49.63M
// appendReserve                                               10.61ns   94.21M
// ostringStreamAppend                                         24.72ns   40.45M
// strcatStack                                                440.64ns    2.27M
// rawCopy                                                     10.73ns   93.18M
// qoperatorPlus                                              467.27ns    2.14M
// qoperatorPlusAssign                                         16.51ns   60.58M
// qappend                                                     12.99ns   76.96M
// qappendReserve                                              13.16ns   75.97M
// ============================================================================

#include <folly/Benchmark.h>
#include <folly/Foreach.h>
#include <string>
#include <QString>
#include <sstream>

#include "String.h"

using namespace folly;
using namespace std;

string t = "abcdefghijklmnopqrstuvwxyz";
QString qt = "abcdefghijklmnopqrstuvwxyz";
String st = "abcdefghijklmnopqrstuvwxyz";

char* rawCopy(char *dest, const void *src, size_t n)
{
    memcpy(dest, src, n);
    return dest + n;
}

BENCHMARK(operatorPlus, iters) {
    string s;
    FOR_EACH_RANGE(i, 0, iters)
        s = s + t;
    doNotOptimizeAway(&s);
}
BENCHMARK(operatorPlusAssign, iters) {
    string s;
    FOR_EACH_RANGE(i, 0, iters)
        s += t;
    doNotOptimizeAway(&s);
}

BENCHMARK(append, iters) {
    string s;
    FOR_EACH_RANGE(i, 0, iters)
        s.append(t);
    doNotOptimizeAway(&s);
}

BENCHMARK(appendReserve, iters) {
    string s;
    BENCHMARK_SUSPEND {
        s.reserve(iters * 30);
    }
    FOR_EACH_RANGE(i, 0, iters)
        s.append(t);
    doNotOptimizeAway(&s);
}

BENCHMARK(ostringStreamAppend, iters) {
    string s;
    ostringstream os;
    FOR_EACH_RANGE(i, 0, iters)
        os << t;
    s = os.str();
    doNotOptimizeAway(&s);
}

BENCHMARK(strcatStack, iters) {
    char buf[1024*1024];
    FOR_EACH_RANGE(i, 0, iters)
        strcat(buf, t.c_str());
    doNotOptimizeAway(&buf);
}

BENCHMARK(rawCopy, iters) {
    char buf[1024*1024];
    char *dest = buf;
    FOR_EACH_RANGE(i, 0, iters)
        dest = rawCopy(dest, t.c_str(), t.size());
    doNotOptimizeAway(&buf);
}

BENCHMARK(qoperatorPlus, iters) {
    QString s;
    FOR_EACH_RANGE(i, 0, iters)
        s = s + qt;
    doNotOptimizeAway(&s);
}
BENCHMARK(qoperatorPlusAssign, iters) {
    QString s;
    FOR_EACH_RANGE(i, 0, iters)
        s += qt;
    doNotOptimizeAway(&s);
}

BENCHMARK(qappend, iters) {
    QString s;
    FOR_EACH_RANGE(i, 0, iters)
        s.append(qt);
    doNotOptimizeAway(&s);
}

BENCHMARK(qappendReserve, iters) {
    QString s;
    BENCHMARK_SUSPEND {
        s.reserve(iters * 30);
    }
    FOR_EACH_RANGE(i, 0, iters)
        s.append(qt);
    doNotOptimizeAway(&s);
}

BENCHMARK(simpleAppend, iters) {
    String s;
    FOR_EACH_RANGE(i, 0, iters)
        s.append(st);
    doNotOptimizeAway(&s);
}

int main(int argc, char *argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    runBenchmarks();
}


