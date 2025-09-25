# FHE Library Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Isrc
LDFLAGS =

# Source directories
MATH_DIR = src/math
TEST_DIR = src/tests

# Math module sources
MATH_SOURCES = $(MATH_DIR)/bigint.cpp $(MATH_DIR)/polynomial.cpp $(MATH_DIR)/matrix.cpp $(MATH_DIR)/ntt.cpp

# Object files
MATH_OBJECTS = $(MATH_SOURCES:.cpp=.o)

# Test executables
TEST_TARGETS = $(TEST_DIR)/test_bigint $(TEST_DIR)/test_polynomial $(TEST_DIR)/test_matrix $(TEST_DIR)/test_ntt

# Build directory
BUILD_DIR = build

.PHONY: all clean test math help

# Default target
all: math

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build the math module (default)"
	@echo "  math       - Build the math module"
	@echo "  test       - Build and run all tests"
	@echo "  test_*     - Build and run specific test (e.g., test_bigint)"
	@echo "  clean      - Remove all build artifacts"
	@echo "  help       - Show this help message"

# Math module target
math: $(MATH_OBJECTS)
	@echo "Math module built successfully!"

# Individual test targets
$(TEST_DIR)/test_bigint: $(TEST_DIR)/test_bigint.cpp $(MATH_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(MATH_OBJECTS) $(LDFLAGS)

$(TEST_DIR)/test_polynomial: $(TEST_DIR)/test_polynomial.cpp $(MATH_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(MATH_OBJECTS) $(LDFLAGS)

$(TEST_DIR)/test_matrix: $(TEST_DIR)/test_matrix.cpp $(MATH_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(MATH_OBJECTS) $(LDFLAGS)

$(TEST_DIR)/test_ntt: $(TEST_DIR)/test_ntt.cpp $(MATH_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(MATH_OBJECTS) $(LDFLAGS)

# Test targets
test_bigint: $(TEST_DIR)/test_bigint
	@echo "Running BigInt tests..."
	@./$(TEST_DIR)/test_bigint

test_polynomial: $(TEST_DIR)/test_polynomial
	@echo "Running Polynomial tests..."
	@./$(TEST_DIR)/test_polynomial

test_matrix: $(TEST_DIR)/test_matrix
	@echo "Running Matrix tests..."
	@./$(TEST_DIR)/test_matrix

test_ntt: $(TEST_DIR)/test_ntt
	@echo "Running NTT tests..."
	@./$(TEST_DIR)/test_ntt

# Run all tests
test: $(TEST_TARGETS)
	@echo "Running all tests..."
	@./$(TEST_DIR)/test_bigint
	@./$(TEST_DIR)/test_polynomial
	@./$(TEST_DIR)/test_matrix
	@./$(TEST_DIR)/test_ntt
	@echo "All tests completed!"

# Object file compilation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(MATH_OBJECTS)
	rm -f $(TEST_TARGETS)
	@echo "Clean completed!"

# Dependencies (simplified)
$(MATH_DIR)/bigint.o: $(MATH_DIR)/bigint.cpp $(MATH_DIR)/bigint.hpp
$(MATH_DIR)/polynomial.o: $(MATH_DIR)/polynomial.cpp $(MATH_DIR)/polynomial.hpp $(MATH_DIR)/bigint.hpp
$(MATH_DIR)/matrix.o: $(MATH_DIR)/matrix.cpp $(MATH_DIR)/matrix.hpp $(MATH_DIR)/bigint.hpp
$(MATH_DIR)/ntt.o: $(MATH_DIR)/ntt.cpp $(MATH_DIR)/ntt.hpp $(MATH_DIR)/bigint.hpp