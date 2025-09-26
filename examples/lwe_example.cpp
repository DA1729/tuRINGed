#include <iostream>
#include "turinged/turinged.hpp"

using namespace turinged;

int main() {
    std::cout << "Turinged LWE Example" << std::endl;
    std::cout << "Library Version: " << VERSION << std::endl;

    // Parameters
    std::size_t k = 256;
    int64 q = 1LL << 30;
    int64 t = 16;
    int64 noise_bound = q / (4 * t);

    Parameters params(0, q, t, noise_bound);

    std::cout << "Parameters: k=" << k << ", q=" << q << ", t=" << t
              << ", noise_bound=" << noise_bound << std::endl;

    // Key generation
    auto sk = keys::generate_lwe_secret_key(k);
    std::cout << "Secret key generated" << std::endl;

    // Test encryption/decryption
    std::vector<int64> test_messages = {0, 1, 2, 3, 5, 7, 10, 15};
    int successes = 0;

    for (int64 m : test_messages) {
        auto ct = schemes::encrypt_lwe(m, sk, params);
        int64 decrypted = schemes::decrypt_lwe(ct, sk, params);

        bool ok = (decrypted == m);
        std::cout << "m=" << m << " -> decrypted=" << decrypted
                  << " " << (ok ? "OK" : "FAIL") << std::endl;

        if (ok) successes++;
    }

    std::cout << "Success rate: " << successes << "/" << test_messages.size() << std::endl;

    return 0;
}