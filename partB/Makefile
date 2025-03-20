cc=g++
CFLAGS=-Wall -std=c++17
all: hw2

hw2: ./srcs/hw2.cpp
	$(cc) $(CFLAGS) -o hw2 ./srcs/hw2.cpp

test: hw2
	@echo "Running tests..."
	@for test in ./test/input/case*.txt; do \
		base=$$(basename $$test); \
		echo "Testing $$base..."; \
		./hw2 $$test temp_output.txt; \
		if diff -w temp_output.txt ./test/output/$$base > /dev/null; then \
			echo "PASSED: $$base"; \
		else \
			echo "FAILED: $$base"; \
			diff -w temp_output.txt ./test/output/$$base; \
		fi; \
	done
	@rm -f temp_output.txt
	@echo "All tests completed."

benchmark: hw2
	@echo "Running benchmarks..."
	@for test in ./test/input/case*.txt; do \
		base=$$(basename $$test); \
		echo "\nBenchmarking $$base:"; \
		time ./hw2 $$test /dev/null; \
	done
	@echo "\nBenchmark completed."

check: hw2
	@echo "=== Running Tests ==="
	@$(MAKE) -s test
	@echo "\n=== Running Benchmarks ==="
	@$(MAKE) -s benchmark

.PHONY: clean test benchmark check

clean:
	rm -f hw2
	rm -f temp_output.txt

