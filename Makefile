C_FLAGS=-Wall -std=c17 -O3 -g -march=skylake -DNDEBUG
FLAGS=-Wall -std=c++17 -O3 -g -march=skylake -DNDEBUG

# integration: integration.c fn.o Tbl.o Makefile
# 	$(CC) $(C_FLAGS) $< fn.o Tbl.o -o $@

run_nb: benchmark_nb words-zero.bin
	./benchmark_nb ./words-zero.bin

integration.o: integration.c 
	$(CC) -c $(C_FLAGS) $< -o $@

%.o: qp/%.c Makefile
	$(CC) -c $(C_FLAGS) $< -o $@

nanobench.o: nanobench.cpp
	$(CXX) -c $(FLAGS) $< -o $@

benchmark_nb: benchmark_nb.cpp nanobench.o integration.o fn.o Tbl.o
	$(CXX) $(FLAGS) benchmark_nb.cpp nanobench.o integration.o fn.o Tbl.o -o $@

words-zero.bin: words.txt
	tr '\n' '\0' < words.txt > words-zero.bin
