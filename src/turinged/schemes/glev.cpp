#include "turinged/schemes/glev.hpp"
#include "turinged/polynomial/polynomial.hpp"
#include "turinged/core/math_utils.hpp"
#include <random>
#include <chrono>
#include <cmath>
#include <stdexcept>

namespace turinged {
namespace schemes {

static std::mt19937_64 rng(static_cast<uint64>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

GLevCiphertext encrypt_glev(
    const Polynomial& message,
    const keys::GLWEPublicKey& pk,
    const Parameters& params,
    int l,
    int64 beta
) {
    std::size_t k = pk.pk2.size();
    std::size_t n = params.n;

    GLevCiphertext glev_ct(l);

    std::uniform_int_distribution<int> binary_dist(0, 1);
    std::uniform_int_distribution<int64> noise_dist(-params.noise_bound, params.noise_bound);

    int64 beta_pow_j = 1;

    for (int j = 0; j <= l; ++j) {
        // Calculate scaling factor for this level
        int64 delta_j = params.q / (beta * beta_pow_j);
        if (delta_j == 0) delta_j = 1;

        Polynomial scaled_m = polynomial::scalar_multiply(message, delta_j, params.q);

        // Generate fresh randomness for each level
        Polynomial u(n);
        for (std::size_t i = 0; i < n; ++i) {
            u[i] = binary_dist(rng);
        }

        Polynomial e1(n);
        for (std::size_t i = 0; i < n; ++i) {
            e1[i] = core::modq(noise_dist(rng), params.q);
        }

        std::vector<Polynomial> e2(k, Polynomial(n));
        for (std::size_t i = 0; i < k; ++i) {
            for (std::size_t co = 0; co < n; ++co) {
                e2[i][co] = core::modq(noise_dist(rng), params.q);
            }
        }

        // Encrypt using GLWE formula
        Polynomial pk1u = polynomial::negacyclic_multiply(pk.pk1, u, params.q);
        Polynomial b = polynomial::add(pk1u, scaled_m, params.q);
        b = polynomial::add(b, e1, params.q);

        std::vector<Polynomial> d_tilde(k, Polynomial(n));
        for (std::size_t i = 0; i < k; ++i) {
            Polynomial tmp = polynomial::negacyclic_multiply(pk.pk2[i], u, params.q);
            d_tilde[i] = polynomial::add(tmp, e2[i], params.q);
        }

        glev_ct.levels[j] = GLWECiphertext();
        glev_ct.levels[j].b = b;
        glev_ct.levels[j].d_tilde = d_tilde;

        beta_pow_j *= beta;
    }

    return glev_ct;
}

Polynomial decrypt_glev_level(
    const GLevCiphertext& ct,
    const keys::GLWESecretKey& sk,
    const Parameters& params,
    int level_idx,
    int64 beta
) {
    if (level_idx >= static_cast<int>(ct.levels.size())) {
        throw std::runtime_error("Level index out of bounds");
    }

    std::size_t k = sk.s.size();
    std::size_t n = params.n;

    const GLWECiphertext& ct_i = ct.levels[level_idx];

    // Calculate scaling factor for this level
    int64 beta_pow_i = 1;
    for (int i = 0; i < level_idx; ++i) {
        beta_pow_i *= beta;
    }
    int64 delta_i = params.q / (beta * beta_pow_i);
    if (delta_i == 0) delta_i = 1;

    // Compute d_tilde · s
    Polynomial d_times_s(n, 0);
    for (std::size_t j = 0; j < k; ++j) {
        Polynomial prod = polynomial::negacyclic_multiply(ct_i.d_tilde[j], sk.s[j], params.q);
        d_times_s = polynomial::add(d_times_s, prod, params.q);
    }

    // Compute b - d_times_s
    Polynomial dec_intermediate = polynomial::subtract(ct_i.b, d_times_s, params.q);

    // Scale down and round
    Polynomial m_rec(n);
    for (std::size_t i = 0; i < n; ++i) {
        int64 centered = core::center_rep(dec_intermediate[i], params.q);
        int64 rounded = (centered >= 0) ? (centered + delta_i / 2) / delta_i : (centered - delta_i / 2) / delta_i;
        m_rec[i] = core::modq(rounded, params.t);
    }

    return m_rec;
}

}
}