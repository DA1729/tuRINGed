#include "turinged/schemes/rlwe.hpp"
#include "turinged/polynomial/polynomial.hpp"
#include "turinged/core/math_utils.hpp"
#include <random>
#include <chrono>
#include <cmath>
#include <stdexcept>

namespace turinged {
namespace schemes {

static std::mt19937_64 rng(static_cast<uint64>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

RLWECiphertext encrypt_rlwe(
    const Polynomial& message,
    const keys::RLWESecretKey& sk,
    const Parameters& params
) {
    std::size_t n = sk.s.size();
    if (message.size() != n) {
        throw std::runtime_error("Message size mismatch with key");
    }

    RLWECiphertext ct(n);

    std::uniform_int_distribution<int64> uniform_dist(0, params.q - 1);
    std::uniform_int_distribution<int64> noise_dist(-params.noise_bound, params.noise_bound);

    // Sample random polynomial a
    for (std::size_t i = 0; i < n; ++i) {
        ct.a[i] = uniform_dist(rng);
    }

    // Sample noise polynomial e
    Polynomial e(n);
    for (std::size_t i = 0; i < n; ++i) {
        e[i] = core::modq(noise_dist(rng), params.q);
    }

    // Scale message
    int64 delta = params.q / params.t;
    Polynomial scaled_m = polynomial::scalar_multiply(message, delta, params.q);

    // Compute a*s
    Polynomial as = polynomial::negacyclic_multiply(ct.a, sk.s, params.q);

    // Compute b = a*s + delta*m + e
    ct.b = polynomial::add(polynomial::add(as, scaled_m, params.q), e, params.q);

    return ct;
}

Polynomial decrypt_rlwe(
    const RLWECiphertext& ct,
    const keys::RLWESecretKey& sk,
    const Parameters& params
) {
    std::size_t n = sk.s.size();
    if (ct.a.size() != n || ct.b.size() != n) {
        throw std::runtime_error("Ciphertext size mismatch with key");
    }

    // Compute a*s
    Polynomial as = polynomial::negacyclic_multiply(ct.a, sk.s, params.q);

    // Compute b - a*s
    Polynomial diff = polynomial::subtract(ct.b, as, params.q);

    // Get centered representation
    std::vector<int64> centered = polynomial::center_representation(diff, params.q);

    // Scale down and round
    int64 delta = params.q / params.t;
    Polynomial m_hat(n);

    for (std::size_t i = 0; i < n; ++i) {
        double val = static_cast<double>(centered[i]) / static_cast<double>(delta);
        int64 rounded = static_cast<int64>(std::llround(val));
        int64 m_coeff = rounded % params.t;
        if (m_coeff < 0) m_coeff += params.t;
        m_hat[i] = m_coeff;
    }

    return m_hat;
}

}
}