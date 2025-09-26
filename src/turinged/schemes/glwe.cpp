#include "turinged/schemes/glwe.hpp"
#include "turinged/polynomial/polynomial.hpp"
#include "turinged/core/math_utils.hpp"
#include <random>
#include <chrono>
#include <cmath>
#include <stdexcept>

namespace turinged {
namespace schemes {

static std::mt19937_64 rng(static_cast<uint64>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

GLWECiphertext encrypt_glwe(
    const Polynomial& message,
    const keys::GLWEPublicKey& pk,
    const Parameters& params
) {
    std::size_t k = pk.pk2.size();
    std::size_t n = params.n;

    if (message.size() != n) {
        throw std::runtime_error("Message size mismatch");
    }

    GLWECiphertext ct(k, n);

    std::uniform_int_distribution<int> binary_dist(0, 1);
    std::uniform_int_distribution<int64> noise_dist(-params.noise_bound, params.noise_bound);

    // Scale message
    int64 delta = params.q / params.t;
    Polynomial scaled_m = polynomial::scalar_multiply(message, delta, params.q);

    // Sample binary polynomial u
    Polynomial u(n);
    for (std::size_t i = 0; i < n; ++i) {
        u[i] = binary_dist(rng);
    }

    // Sample noise polynomials
    Polynomial e1(n);
    for (std::size_t i = 0; i < n; ++i) {
        e1[i] = core::modq(noise_dist(rng), params.q);
    }

    std::vector<Polynomial> e2(k, Polynomial(n));
    for (std::size_t i = 0; i < k; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            e2[i][j] = core::modq(noise_dist(rng), params.q);
        }
    }

    // Compute b = pk1 * u + scaled_m + e1
    Polynomial pk1u = polynomial::negacyclic_multiply(pk.pk1, u, params.q);
    ct.b = polynomial::add(polynomial::add(pk1u, scaled_m, params.q), e1, params.q);

    // Compute d_tilde = pk2 * u + e2
    for (std::size_t i = 0; i < k; ++i) {
        Polynomial tmp = polynomial::negacyclic_multiply(pk.pk2[i], u, params.q);
        ct.d_tilde[i] = polynomial::add(tmp, e2[i], params.q);
    }

    return ct;
}

Polynomial decrypt_glwe(
    const GLWECiphertext& ct,
    const keys::GLWESecretKey& sk,
    const Parameters& params
) {
    std::size_t k = sk.s.size();
    std::size_t n = params.n;

    if (ct.d_tilde.size() != k || ct.b.size() != n) {
        throw std::runtime_error("Ciphertext size mismatch with key");
    }

    // Compute d_tilde · s = sum_j d_tilde[j] * s[j]
    Polynomial d_times_s(n, 0);
    for (std::size_t j = 0; j < k; ++j) {
        Polynomial prod = polynomial::negacyclic_multiply(ct.d_tilde[j], sk.s[j], params.q);
        d_times_s = polynomial::add(d_times_s, prod, params.q);
    }

    // Compute b - d_times_s
    Polynomial diff = polynomial::subtract(ct.b, d_times_s, params.q);

    // Scale down and round
    int64 delta = params.q / params.t;
    Polynomial m_rec(n);

    for (std::size_t i = 0; i < n; ++i) {
        int64 centered = core::center_rep(diff[i], params.q);
        int64 rounded = (centered >= 0) ? (centered + delta / 2) / delta : (centered - delta / 2) / delta;
        int64 mm = rounded % params.t;
        if (mm < 0) mm += params.t;
        m_rec[i] = mm;
    }

    return m_rec;
}

}
}