cc=g++
CFLAGS=-Wall -std=c++17
all: solver

solver: ./srcs/solver.cpp
	$(cc) $(CFLAGS) -o solver ./srcs/solver.cpp

test: solver
	@echo "Running tests..."
	@for test in ./test/input/case*.txt; do \
		base=$$(basename $$test); \
		echo "Testing $$base..."; \
		./solver < $$test > temp_output.txt; \
		if diff -w temp_output.txt ./test/output/$$base > /dev/null; then \
			echo "PASSED: $$base"; \
		else \
			echo "FAILED: $$base"; \
			diff -w temp_output.txt ./test/output/$$base; \
		fi; \
	done
	@rm -f temp_output.txt
	@echo "All tests completed."

benchmark: solver
	@echo "Running benchmarks..."
	@for test in ./test/input/case*.txt; do \
		base=$$(basename $$test); \
		echo "\nBenchmarking $$base:"; \
		time ./solver < $$test > /dev/null; \
	done
	@echo "\nBenchmark completed."

check: solver
	@echo "=== Running Tests ==="
	@$(MAKE) -s test
	@echo "\n=== Running Benchmarks ==="
	@$(MAKE) -s benchmark

.PHONY: clean test benchmark check

clean:
	rm -f solver
	rm -f temp_output.txt

