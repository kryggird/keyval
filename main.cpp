#include "nanobench.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <random>
#include <vector>

#include <x86intrin.h>

using needle_t = uint64_t;

extern "C" {
    typedef enum flags_t {
        PREFETCH = 1,
        MEMORY = 2,
    } flags_t;

    size_t searchsorted_branchless(const needle_t*, size_t, const needle_t*, size_t, size_t);
    size_t searchsorted_branchy(const needle_t*, size_t, const needle_t*, size_t);
    size_t searchsorted_streams(const needle_t*, size_t, const needle_t*, size_t, size_t);
    size_t searchsorted_hybrid(const needle_t*, size_t, const needle_t*, size_t, size_t);
    size_t searchsorted_multipass(const needle_t*, size_t, const needle_t*, size_t, size_t);
}

auto prepare_random_input(size_t output_size)
{
    using namespace std;
    random_device random_device {};
    mt19937 random_engine {random_device()};
    random_engine.seed(42);

    uniform_int_distribution<needle_t> random_num {0, needle_t(-1)};

    vector<needle_t> res {};
    res.reserve(output_size);

    for (size_t i=0; i < output_size; i++) {
        res.emplace_back(random_num(random_engine));
    }

    sort(res.begin(), res.end());

    return res;
}


template <typename F>
void run_benchmark(const char* title, 
                   const char* name,
                   const std::vector<needle_t>& haystack,
                   const std::vector<needle_t>& needles,
                   F func) {
    using namespace ankerl::nanobench;

    Bench().title(title).minEpochIterations(64ul)
           .run(name, [&haystack, &needles, func]() {
        auto res = func(
            haystack.data(), haystack.size(), needles.data(), needles.size()
        );

        doNotOptimizeAway(res);
    });
}


int main(int argc, char* argv[]) {
    auto haystack = prepare_random_input(128ul * 1024ul * 1024ul);
    // auto haystack = prepare_random_input(256ul * 1024ul - 50ul);
    auto needles = prepare_random_input(1024ul);
    
    run_benchmark("both random", "clflush", haystack, needles, [](auto a1, auto a2, auto a3, auto a4) { return 0; });

    return EXIT_SUCCESS;
}
