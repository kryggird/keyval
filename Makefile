C_FLAGS=-Wall -std=c17 -O3 -g -march=skylake -DNDEBUG
FLAGS=-Wall -std=c++17 -O3 -g -march=skylake -DNDEBUG

run_nb: benchmark_nb
	./benchmark_nb

nanobench.o: nanobench.cpp
	$(CXX) -c $(FLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(C_FLAGS) $< -o $@

benchmark_nb: benchmark_nb.cpp nanobench.o binary_search.o multi_binary_search.o compile_flags.txt
	$(CXX) $(FLAGS) benchmark_nb.cpp nanobench.o binary_search.o multi_binary_search.o -o $@

compile_flags.txt: Makefile
	echo $(FLAGS) | sed 's/\s\+/\n/g' > compile_flags.txt
