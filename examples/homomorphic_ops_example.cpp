#include <iostream>
#include "turinged/turinged.hpp"

using namespace turinged;

void test_lwe_homomorphic_ops() {
    std::cout << "=== LWE Homomorphic Operations ===" << std::endl;

    // Parameters
    std::size_t k = 128;
    int64 q = 1LL << 25;
    int64 t = 8;
    int64 noise_bound = q / (8 * t);

    Parameters params(0, q, t, noise_bound);

    // Key generation
    auto sk = keys::generate_lwe_secret_key(k);

    // Test messages
    int64 m1 = 3, m2 = 5;
    std::cout << "Testing m1=" << m1 << ", m2=" << m2 << std::endl;

    // Encrypt
    auto ct1 = schemes::encrypt_lwe(m1, sk, params);
    auto ct2 = schemes::encrypt_lwe(m2, sk, params);

    // Homomorphic addition
    auto ct_add = operations::add_lwe(ct1, ct2, params);
    int64 result_add = schemes::decrypt_lwe(ct_add, sk, params);
    std::cout << "Homomorphic addition: " << m1 << " + " << m2 << " = " << result_add
              << " (expected: " << ((m1 + m2) % t) << ")" << std::endl;

    // Homomorphic subtraction
    auto ct_sub = operations::subtract_lwe(ct1, ct2, params);
    int64 result_sub = schemes::decrypt_lwe(ct_sub, sk, params);
    std::cout << "Homomorphic subtraction: " << m1 << " - " << m2 << " = " << result_sub
              << " (expected: " << ((m1 - m2 + t) % t) << ")" << std::endl;

    // Scalar multiplication
    int64 scalar = 2;
    auto ct_scalar = operations::scalar_multiply_lwe(ct1, scalar, params);
    int64 result_scalar = schemes::decrypt_lwe(ct_scalar, sk, params);
    std::cout << "Scalar multiplication: " << scalar << " * " << m1 << " = " << result_scalar
              << " (expected: " << ((scalar * m1) % t) << ")" << std::endl;
}

void test_rlwe_homomorphic_ops() {
    std::cout << "\n=== RLWE Homomorphic Operations ===" << std::endl;

    // Parameters
    std::size_t n = 512;
    int64 q = 8192;
    int64 t = 16;
    int64 noise_bound = 2;

    Parameters params(n, q, t, noise_bound);

    // Key generation
    auto sk = keys::generate_rlwe_secret_key(n);

    // Create test messages
    Polynomial m1(n), m2(n);
    for (std::size_t i = 0; i < n; ++i) {
        m1[i] = (i + 1) % t;
        m2[i] = (i + 2) % t;
    }

    std::cout << "Testing polynomial messages..." << std::endl;

    // Encrypt
    auto ct1 = schemes::encrypt_rlwe(m1, sk, params);
    auto ct2 = schemes::encrypt_rlwe(m2, sk, params);

    // Homomorphic addition
    auto ct_add = operations::add_rlwe(ct1, ct2, params);
    Polynomial result_add = schemes::decrypt_rlwe(ct_add, sk, params);

    // Check first few coefficients
    bool add_correct = true;
    for (std::size_t i = 0; i < 10; ++i) {
        int64 expected = (m1[i] + m2[i]) % t;
        if (result_add[i] != expected) {
            add_correct = false;
            break;
        }
    }
    std::cout << "Homomorphic addition: " << (add_correct ? "PASS" : "FAIL") << std::endl;

    // Scalar multiplication
    int64 scalar = 3;
    auto ct_scalar = operations::scalar_multiply_rlwe(ct1, scalar, params);
    Polynomial result_scalar = schemes::decrypt_rlwe(ct_scalar, sk, params);

    bool scalar_correct = true;
    for (std::size_t i = 0; i < 10; ++i) {
        int64 expected = (scalar * m1[i]) % t;
        if (result_scalar[i] != expected) {
            scalar_correct = false;
            break;
        }
    }
    std::cout << "Scalar multiplication: " << (scalar_correct ? "PASS" : "FAIL") << std::endl;
}

int main() {
    std::cout << "Turinged Homomorphic Operations Example" << std::endl;

    test_lwe_homomorphic_ops();
    test_rlwe_homomorphic_ops();

    return 0;
}