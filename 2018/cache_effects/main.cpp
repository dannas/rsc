#include <iostream>
#include <cstdint>
#include <ctime>
#include <sys/time.h>
#include <algorithm>
#include <vector>
#include <unistd.h>
#include <cassert>

#define FORCE_INLINE inline __attribute__((always_inline))

using namespace std;

FORCE_INLINE uint64_t rdtscp_start() {
    uint32_t low, high;
	asm volatile("rdtscp" : "=a" (low), "=d" (high) :: "ecx", "memory");
    return ((uint64_t)high << 32) | low;
}

FORCE_INLINE uint64_t rdtscp_stop() {
    uint32_t low, high;
	asm volatile("rdtscp" : "=a" (low), "=d" (high) :: "ecx", "memory");
	asm volatile("lfence" ::: "memory");
    return ((uint64_t)high << 32) | low;
}

#define RDTSC_DIRTY "%rax", "%rbx", "%rcx", "%rdx"

#define RDTSC_START(cycles)                                \
    do {                                                   \
        register unsigned cyc_high, cyc_low;               \
        asm volatile("CPUID\n\t"                           \
                     "RDTSC\n\t"                           \
                     "mov %%edx, %0\n\t"                   \
                     "mov %%eax, %1\n\t"                   \
                     : "=r" (cyc_high), "=r" (cyc_low)     \
                     :: RDTSC_DIRTY);                      \
        (cycles) = ((uint64_t)cyc_high << 32) | cyc_low;   \
    } while (0)

#define RDTSC_STOP(cycles)                                 \
    do {                                                   \
        register unsigned cyc_high, cyc_low;               \
        asm volatile("RDTSCP\n\t"                          \
                     "mov %%edx, %0\n\t"                   \
                     "mov %%eax, %1\n\t"                   \
                     "CPUID\n\t"                           \
                     : "=r" (cyc_high), "=r" (cyc_low)     \
                     :: RDTSC_DIRTY);                      \
        (cycles) = ((uint64_t)cyc_high << 32) | cyc_low;   \
    } while(0)

class StopWatch {
public:
    StopWatch() {
    }
    void start() {
        gettimeofday(&start_, nullptr);
    }
    // Return elapsed time in seconds
    double elapsed() {
        timeval end;
        gettimeofday(&end, nullptr);
        long sec = end.tv_sec - start_.tv_sec;
        long usec = end.tv_usec - start_.tv_usec;
        return sec + usec * 1e-6;
    }
private:
    timeval start_;

};

const size_t kMegaByte = 1024 * 1024;

uint32_t array[64 * kMegaByte];

void step1() {
    for (size_t i = 0; i < sizeof(array) / sizeof(array[0]); ++i) {
        array[i] *= 3;
    }
}

void step16() {
    for (size_t i = 0; i < sizeof(array) / sizeof(array[0]); i += 16) {
        array[i] *= 3;
    }
}

uint64_t median(const vector<uint64_t>& datums) {
    size_t mid = datums.size() / 2;

    if (datums.size() % 2 == 0)
        return (datums[mid-1] + datums[mid]) / 2;
    return datums[mid];
}

uint64_t range(const vector<uint64_t>& datums) {
    // assert(datums.size() < 0)
    auto e = end(datums) - 1;
    auto b = begin(datums);
    return *e - *b;
}

// TODO(dannas): Start by verifying the latency of rdtscp using the
// code from the Intel white paper.

// TODO(dannas): Use these factors
// 1. hyperthreading on|off
// 2. turbo boost on|off
// 3. cpu scaling on|off
// 4. load high|low
// 5. process isolation via cset on|off

// Try these benchmarking methods
// rdtsc
// clock_gettime(CLOCK_MONOTONIC)
// clock_gettime(CLOCK_PROCESS_CPUTIME_ID)
// std::chrono::high_resolution_clock
// std::chrono::stable_clock
// gettimeofday
// getrusage
// times
// performance counters

// Measure
// latency
// resolution
// variance - compare min to median

// Use these inputs
// a few cycles
// hundreds of cycles
// 1 us
// 100 us
// 1 ms
// 10 ms

// Take measurement over a span of time to avoid affects from external load

vector<uint64_t> measureRdtscLatency() {
    vector<uint64_t> datums;

    for (size_t i = 0 ; i < 100; ++i) {
        uint64_t start, end;
        start = rdtscp_start();
        end = rdtscp_stop();
        datums.push_back(end - start);
    }
    return datums;
}

int main(int argc, char *argv[])
{
    StopWatch watch;
#if 0
    watch.start();
    step1();
    cout << watch.elapsed() << "\n";
#else

    vector<uint64_t> datums;

    for (size_t i = 0 ; i < 100; ++i) {
        uint64_t start, end;
        timespec tp;
        timeval tv;

        //clock_gettime(CLOCK_MONOTONIC, &tp);
        //RDTSC_START(start);
        // TODO(dannas): Why is first call to clock_gettime 100x slower than
        // the rest of the  calls?
        //RDTSC_STOP(end);
        start = rdtscp_start();
        //clock_gettime(CLOCK_MONOTONIC, &tp);
        //gettimeofday(&tv, nullptr);
        //clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
        end = rdtscp_stop();

        //usleep(10);

        datums.push_back(end - start);

    }
    sort(begin(datums), end(datums));
    for (uint64_t d :  datums) {
        cout << d << "\n";
    }
#endif
}

