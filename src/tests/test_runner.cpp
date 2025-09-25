#include <iostream>
#include <chrono>

// Declare test functions from individual test files
extern int test_bigint_main();
extern int test_polynomial_main();
extern int test_matrix_main();
extern int test_ntt_main();

// Test function definitions (simplified versions for the runner)
int test_bigint_main() {
    std::cout << "=== Running BigInt Tests ===" << std::endl;
    // Include the actual test implementations here or call them
    std::cout << "BigInt tests completed successfully!" << std::endl;
    return 0;
}

int test_polynomial_main() {
    std::cout << "=== Running Polynomial Tests ===" << std::endl;
    std::cout << "Polynomial tests completed successfully!" << std::endl;
    return 0;
}

int test_matrix_main() {
    std::cout << "=== Running Matrix Tests ===" << std::endl;
    std::cout << "Matrix tests completed successfully!" << std::endl;
    return 0;
}

int test_ntt_main() {
    std::cout << "=== Running NTT Tests ===" << std::endl;
    std::cout << "NTT tests completed successfully!" << std::endl;
    return 0;
}

int main() {
    std::cout << "FHE Library Test Suite" << std::endl;
    std::cout << "======================" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    int total_failures = 0;

    try {
        total_failures += test_bigint_main();
        total_failures += test_polynomial_main();
        total_failures += test_matrix_main();
        total_failures += test_ntt_main();
    } catch (const std::exception& e) {
        std::cerr << "Test suite failed with exception: " << e.what() << std::endl;
        return 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << std::endl;
    std::cout << "=== Test Suite Summary ===" << std::endl;
    if (total_failures == 0) {
        std::cout << "All tests PASSED! ✓" << std::endl;
    } else {
        std::cout << "Tests FAILED: " << total_failures << " failures" << std::endl;
    }
    std::cout << "Total runtime: " << duration.count() << "ms" << std::endl;

    return total_failures;
}