cc=g++
CFLAGS=-Wall -std=c++17
all: hw2

hw2: ./srcs/hw2.cpp
	$(cc) $(CFLAGS) -o hw2 ./srcs/hw2.cpp

test: hw2
	@echo "Running tests..."
	@mkdir -p ./test/output
	@for test in ./test/input/case*.txt; do \
		base=$$(basename $$test); \
		echo "Testing $$base..."; \
		./hw2 $$test ./test/output/$$base; \
		python3 ./test/visualize.py ./test/output/$$base; \
		if diff -w ./test/output/$$base ./test/expected_output/$$base > /dev/null; then \
			echo "PASSED: $$base"; \
		else \
			echo "FAILED: $$base"; \
			diff -w ./test/output/$$base ./test/expected_output/$$base; \
		fi; \
	done
	@echo "All tests completed."

benchmark: hw2
	@echo "Running benchmarks..."
	@mkdir -p ./test/output
	@for test in ./test/input/case*.txt; do \
		base=$$(basename $$test); \
		echo "\nBenchmarking $$base:"; \
		time ./hw2 $$test ./test/output/$$base; \
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
	rm -rf ./test/output

