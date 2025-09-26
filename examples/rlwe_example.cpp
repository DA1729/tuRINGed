#include <iostream>
#include "turinged/turinged.hpp"

using namespace turinged;

int main() {
    std::cout << "Turinged RLWE Example" << std::endl;

    // Parameters
    std::size_t n = 1024;
    int64 q = 16384;
    int64 t = 256;
    int64 noise_bound = 3;

    Parameters params(n, q, t, noise_bound);

    std::cout << "Parameters: n=" << n << ", q=" << q << ", t=" << t
              << ", noise_bound=" << noise_bound << std::endl;

    // Key generation
    auto sk = keys::generate_rlwe_secret_key(n);
    std::cout << "Secret key generated" << std::endl;

    // Create test message
    Polynomial message(n);
    for (std::size_t i = 0; i < n; ++i) {
        message[i] = i % t;
    }

    polynomial::print_polynomial(message, "Original message");

    // Encrypt
    auto ct = schemes::encrypt_rlwe(message, sk, params);
    std::cout << "Message encrypted" << std::endl;

    // Decrypt
    Polynomial decrypted = schemes::decrypt_rlwe(ct, sk, params);
    polynomial::print_polynomial(decrypted, "Decrypted message");

    // Check correctness
    bool success = polynomial::is_equal(message, decrypted);
    std::cout << "Decryption " << (success ? "succeeded" : "failed") << std::endl;

    return 0;
}