#include "turinged/keys/keys.hpp"
#include "turinged/polynomial/polynomial.hpp"
#include "turinged/core/math_utils.hpp"
#include <random>
#include <chrono>

namespace turinged {
namespace keys {

static std::mt19937_64 rng(static_cast<uint64>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

LWESecretKey generate_lwe_secret_key(std::size_t k) {
    LWESecretKey sk(k);
    std::uniform_int_distribution<int> binary_dist(0, 1);

    for (std::size_t i = 0; i < k; ++i) {
        sk.s[i] = binary_dist(rng);
    }

    return sk;
}

RLWESecretKey generate_rlwe_secret_key(std::size_t n) {
    RLWESecretKey sk(n);
    std::uniform_int_distribution<int> binary_dist(0, 1);

    for (std::size_t i = 0; i < n; ++i) {
        sk.s[i] = binary_dist(rng);
    }

    return sk;
}

GLWESecretKey generate_glwe_secret_key(std::size_t k, std::size_t n) {
    GLWESecretKey sk(k, n);
    std::uniform_int_distribution<int> binary_dist(0, 1);

    for (std::size_t i = 0; i < k; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            sk.s[i][j] = binary_dist(rng);
        }
    }

    return sk;
}

GLWEPublicKey generate_glwe_public_key(const GLWESecretKey& sk, const Parameters& params) {
    std::size_t k = sk.s.size();
    std::size_t n = params.n;
    int64 q = params.q;

    GLWEPublicKey pk(k, n);

    std::uniform_int_distribution<int64> uniform_dist(0, q - 1);
    std::uniform_int_distribution<int64> noise_dist(-params.noise_bound, params.noise_bound);

    // Generate random A matrix
    for (std::size_t i = 0; i < k; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            pk.pk2[i][j] = uniform_dist(rng);
        }
    }

    // Generate error polynomial
    Polynomial e(n);
    for (std::size_t i = 0; i < n; ++i) {
        e[i] = core::modq(noise_dist(rng), q);
    }

    // Compute AS = sum_j A_j * S_j
    Polynomial as(n, 0);
    for (std::size_t i = 0; i < k; ++i) {
        Polynomial as_i = polynomial::negacyclic_multiply(pk.pk2[i], sk.s[i], q);
        as = polynomial::add(as, as_i, q);
    }

    // PK1 = AS + E
    pk.pk1 = polynomial::add(as, e, q);

    return pk;
}

}
}