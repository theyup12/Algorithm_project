
CLANG_FLAGS = -std=c++17 -Wall -O -g

GTEST_FLAGS = -lpthread -lgtest_main -lgtest

# determine Python version, need at least 3.7
PYTHON=python3
ifneq (, $(shell which python3.7))
	PYTHON=python3.7
endif
ifneq (, $(shell which python3.8))
	PYTHON=python3.8
endif

build: algorithms_test algorithms_timing

test: algorithms_test
	./algorithms_test

grade: grade.py algorithms_test
	${PYTHON} grade.py

algorithms_test:  algorithms.hpp algorithms_test.cpp
	clang++ ${CLANG_FLAGS} ${GTEST_FLAGS} algorithms_test.cpp -o algorithms_test

algorithms_timing: timer.hpp algorithms.hpp algorithms_timing.cpp
	clang++ ${CLANG_FLAGS} algorithms_timing.cpp -o algorithms_timing

clean:
	rm -f gtest.xml results.json algorithms_test algorithms_timing
