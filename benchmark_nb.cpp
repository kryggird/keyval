#include "nanobench.h"

#include <cassert>
#include <cstdlib>
#include <random>
#include <vector>

#include <x86intrin.h>

using needle_t = uint64_t;

extern "C" {
    ssize_t max_word_frequency_khash(const char *const words[], size_t len);
    ssize_t max_word_frequency_kbtree(const char *const words[], size_t len);
    ssize_t max_word_frequency_qptrie(const char *const words[], size_t len);
}

auto prepare_random_input(const std::vector<const char*>& dictionary,  size_t output_size)
{
    using namespace std;
    random_device random_device {};
    mt19937 random_engine {random_device()};
    random_engine.seed(42);

    geometric_distribution<size_t> rand_geom {};

    vector<const char*> res {};
    res.reserve(output_size);

    for (size_t i=0; i < output_size; i++) {
        const char* w = dictionary[rand_geom(random_engine) % dictionary.size()];
        res.push_back(w);
    }

    return res;
}


template <typename F>
void run_benchmark(const char* title, 
                   const char* name,
                   const std::vector<const char*>& words,
                   F func) {
    using namespace ankerl::nanobench;

    Bench().title(title).minEpochIterations(64ul)
           .run(name, [&words, func]() {
        auto res = func(words.data(), words.size());

        doNotOptimizeAway(res);
    });
}


int main(int argc, char* argv[]) {
    auto dictionary = std::vector<const char*> {"one", "two", "three", "four"};
    auto words = prepare_random_input(dictionary, 32ul * 1024ul);
    
    run_benchmark("khash", "clflush", words, max_word_frequency_khash);
    run_benchmark("kbtree", "clflush", words, max_word_frequency_kbtree);
    // run_benchmark("qptrie", "clflush", words, max_word_frequency_qptrie);

    return EXIT_SUCCESS;
}
