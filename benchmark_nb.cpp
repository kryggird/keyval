#include "nanobench.h"

#include <cassert>
#include <cstdlib>
#include <random>
#include <string>
#include <string_view>
#include <vector>

#include <fcntl.h> // O_RDONLY
#include <sys/stat.h> // fstat
#include <unistd.h> // read

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

    uniform_int_distribution<size_t> rand_geom {};
    //geometric_distribution<size_t> rand_geom {};

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

    Bench().title(title).minEpochIterations(128ul)
           .run(name, [&words, func]() {
        auto res = func(words.data(), words.size());

        doNotOptimizeAway(res);
    });
}

std::string buffer_from_file(const char* path) {
    int fd = open(path, O_RDONLY);
    struct stat st;
    fstat(fd, &st);

    std::string res(st.st_size, '\0');
    read(fd, res.data(), res.size());

    return res;
}

std::vector<const char*> lines_from_buffer(std::string_view sv) {
    std::vector<const char*> res {}; 

    while (sv.size() > 0ul) {
        res.push_back(sv.data());
        size_t shift = sv.find_first_of('\0'); 
        sv = sv.substr(shift + 1ul);
    }

    return res;
}

int main(int argc, char* argv[]) {
    if (argc != 2) { return 1; }
    auto buf = buffer_from_file(argv[1]);
    auto dict = lines_from_buffer(buf);
    auto words = prepare_random_input(dict, 8 * 1024ul);
    
    run_benchmark("small dict", "do nothing", words, [](auto a1, auto a2) { return 0; });
    run_benchmark("small dict", "khash", words, max_word_frequency_khash);
    run_benchmark("small dict", "kbtree", words, max_word_frequency_kbtree);
    run_benchmark("small dict", "qptrie", words, max_word_frequency_qptrie);

    return EXIT_SUCCESS;
}
