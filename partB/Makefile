cc=g++
CFLAGS=-Wall -std=c++17
all: floorplanner

floorplanner: ./srcs/floorplanner.cpp
	$(cc) $(CFLAGS) -o floorplanner ./srcs/floorplanner.cpp

run: floorplanner
	@echo "Running all test cases..."
	@mkdir -p ./output
	@for test in ./test/input/case*.txt; do \
		base=$$(basename $$test); \
		echo "Running $$base..."; \
		./floorplanner $$test ./output/$$base; \
	done
	@echo "All runs completed."

test: floorplanner
	@echo "Running tests..."
	@mkdir -p ./output
	@for test in ./test/input/case*.txt; do \
		base=$$(basename $$test); \
		echo "Testing $$base..."; \
		./floorplanner $$test ./output/$$base; \
		if diff -w ./output/$$base ./test/output/$$base > /dev/null; then \
			echo "PASSED: $$base"; \
		else \
			echo "FAILED: $$base"; \
			diff -w ./output/$$base ./test/output/$$base; \
		fi; \
		python3 ./test/visualize.py ./output/$$base; \
	done
	@echo "All tests completed."


visualize: floorplanner
	@echo "Visualizing all test cases..."
	@$(MAKE) -s run
	@for test in ./output/case*.txt; do \
		base=$$(basename $$test); \
		echo "Visualizing $$base..."; \
		python3 ./test/visualize.py $$test; \
	done
	@echo "All visualizations completed."

benchmark: floorplanner
	@echo "Running benchmarks..."
	@mkdir -p ./output
	@for test in ./test/input/case*.txt; do \
		base=$$(basename $$test); \
		echo "\nBenchmarking $$base:"; \
		time ./floorplanner $$test ./output/$$base; \
	done
	@echo "\nBenchmark completed."

check: floorplanner
	@echo "=== Running Tests ==="
	@$(MAKE) -s test
	@echo "\n=== Running Benchmarks ==="
	@$(MAKE) -s benchmark

.PHONY: clean test benchmark check visualize run

clean:
	rm -f floorplanner
	rm -rf ./output

