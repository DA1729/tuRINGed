cmake_minimum_required(VERSION 3.15)
project(FHE_Tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Math module sources
set(MATH_SOURCES
    ../math/bigint.cpp
    ../math/polynomial.cpp
    ../math/matrix.cpp
    ../math/ntt.cpp
)

# Test executables
add_executable(test_bigint test_bigint.cpp ${MATH_SOURCES})
add_executable(test_polynomial test_polynomial.cpp ${MATH_SOURCES})
add_executable(test_matrix test_matrix.cpp ${MATH_SOURCES})
add_executable(test_ntt test_ntt.cpp ${MATH_SOURCES})

# All tests executable
add_executable(run_all_tests
    test_runner.cpp
    ${MATH_SOURCES}
)

# Custom target to run all tests
add_custom_target(run_tests
    COMMAND test_bigint
    COMMAND test_polynomial
    COMMAND test_matrix
    COMMAND test_ntt
    DEPENDS test_bigint test_polynomial test_matrix test_ntt
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)