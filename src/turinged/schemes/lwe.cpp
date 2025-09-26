#include "turinged/schemes/lwe.hpp"
#include "turinged/core/math_utils.hpp"
#include <random>
#include <chrono>
#include <cmath>
#include <stdexcept>

namespace turinged {
namespace schemes {

static std::mt19937_64 rng(static_cast<uint64>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

LWECiphertext encrypt_lwe(
    int64 message,
    const keys::LWESecretKey& sk,
    const Parameters& params
) {
    std::size_t k = sk.s.size();
    if (message < 0 || message >= params.t) {
        throw std::runtime_error("Message out of range");
    }

    LWECiphertext ct(k);

    std::uniform_int_distribution<int64> uniform_dist(0, params.q - 1);
    std::uniform_int_distribution<int64> noise_dist(-params.noise_bound, params.noise_bound);

    // Sample random a
    for (std::size_t i = 0; i < k; ++i) {
        ct.a[i] = uniform_dist(rng);
    }

    // Compute inner product a·s
    int64 inner = core::dot_product_modq(ct.a, sk.s, params.q);

    // Compute Delta*m + e
    int64 delta = params.q / params.t;
    int64 scaled_m = delta * message;
    int64 e = noise_dist(rng);

    // Compute b = inner + scaled_m + e (mod q)
    int128 s128 = static_cast<int128>(inner) + static_cast<int128>(scaled_m) + static_cast<int128>(e);
    int64 s64 = static_cast<int64>(s128 % params.q);
    if (s64 < 0) s64 += params.q;
    ct.b = s64;

    return ct;
}

int64 decrypt_lwe(
    const LWECiphertext& ct,
    const keys::LWESecretKey& sk,
    const Parameters& params
) {
    std::size_t k = sk.s.size();
    if (ct.a.size() != k) {
        throw std::runtime_error("Ciphertext size mismatch with secret key");
    }

    // Compute b - a·s (mod q)
    int64 inner = core::dot_product_modq(ct.a, sk.s, params.q);
    int64 diff = core::modq(ct.b - inner, params.q);

    // Choose centered representative
    int64 centered = core::center_rep(diff, params.q);

    // Recover message by rounding
    int64 delta = params.q / params.t;
    double val = static_cast<double>(centered) / static_cast<double>(delta);
    int64 m_hat = static_cast<int64>(std::llround(val));

    // Reduce modulo t
    m_hat %= params.t;
    if (m_hat < 0) m_hat += params.t;

    return m_hat;
}

}
}