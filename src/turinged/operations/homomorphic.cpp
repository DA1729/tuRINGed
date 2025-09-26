#include "turinged/operations/homomorphic.hpp"
#include "turinged/polynomial/polynomial.hpp"
#include "turinged/core/math_utils.hpp"
#include <stdexcept>

namespace turinged {
namespace operations {

// LWE Homomorphic Operations
schemes::LWECiphertext add_lwe(
    const schemes::LWECiphertext& ct1,
    const schemes::LWECiphertext& ct2,
    const Parameters& params
) {
    if (ct1.a.size() != ct2.a.size()) {
        throw std::runtime_error("LWE ciphertext size mismatch");
    }

    schemes::LWECiphertext result(ct1.a.size());

    for (std::size_t i = 0; i < ct1.a.size(); ++i) {
        result.a[i] = core::modq(ct1.a[i] + ct2.a[i], params.q);
    }
    result.b = core::modq(ct1.b + ct2.b, params.q);

    return result;
}

schemes::LWECiphertext subtract_lwe(
    const schemes::LWECiphertext& ct1,
    const schemes::LWECiphertext& ct2,
    const Parameters& params
) {
    if (ct1.a.size() != ct2.a.size()) {
        throw std::runtime_error("LWE ciphertext size mismatch");
    }

    schemes::LWECiphertext result(ct1.a.size());

    for (std::size_t i = 0; i < ct1.a.size(); ++i) {
        result.a[i] = core::modq(ct1.a[i] - ct2.a[i], params.q);
    }
    result.b = core::modq(ct1.b - ct2.b, params.q);

    return result;
}

schemes::LWECiphertext scalar_multiply_lwe(
    const schemes::LWECiphertext& ct,
    int64 scalar,
    const Parameters& params
) {
    schemes::LWECiphertext result(ct.a.size());

    for (std::size_t i = 0; i < ct.a.size(); ++i) {
        int128 tmp = static_cast<int128>(ct.a[i]) * scalar;
        result.a[i] = core::modq(static_cast<int64>(tmp % params.q), params.q);
    }

    int128 tmp_b = static_cast<int128>(ct.b) * scalar;
    result.b = core::modq(static_cast<int64>(tmp_b % params.q), params.q);

    return result;
}

// RLWE Homomorphic Operations
schemes::RLWECiphertext add_rlwe(
    const schemes::RLWECiphertext& ct1,
    const schemes::RLWECiphertext& ct2,
    const Parameters& params
) {
    schemes::RLWECiphertext result(params.n);

    result.a = polynomial::add(ct1.a, ct2.a, params.q);
    result.b = polynomial::add(ct1.b, ct2.b, params.q);

    return result;
}

schemes::RLWECiphertext subtract_rlwe(
    const schemes::RLWECiphertext& ct1,
    const schemes::RLWECiphertext& ct2,
    const Parameters& params
) {
    schemes::RLWECiphertext result(params.n);

    result.a = polynomial::subtract(ct1.a, ct2.a, params.q);
    result.b = polynomial::subtract(ct1.b, ct2.b, params.q);

    return result;
}

schemes::RLWECiphertext scalar_multiply_rlwe(
    const schemes::RLWECiphertext& ct,
    int64 scalar,
    const Parameters& params
) {
    schemes::RLWECiphertext result(params.n);

    result.a = polynomial::scalar_multiply(ct.a, scalar, params.q);
    result.b = polynomial::scalar_multiply(ct.b, scalar, params.q);

    return result;
}

schemes::RLWECiphertext multiply_rlwe(
    const schemes::RLWECiphertext& ct1,
    const schemes::RLWECiphertext& ct2,
    const Parameters& params
) {
    // Basic polynomial multiplication - this would need more sophisticated implementation
    // for noise management in practice
    schemes::RLWECiphertext result(params.n);

    // This is a simplified version - real implementation needs careful noise analysis
    Polynomial a_mult = polynomial::negacyclic_multiply(ct1.a, ct2.a, params.q);
    Polynomial b_mult = polynomial::negacyclic_multiply(ct1.b, ct2.b, params.q);

    result.a = a_mult;
    result.b = b_mult;

    return result;
}

// GLWE Homomorphic Operations
schemes::GLWECiphertext add_glwe(
    const schemes::GLWECiphertext& ct1,
    const schemes::GLWECiphertext& ct2,
    const Parameters& params
) {
    if (ct1.d_tilde.size() != ct2.d_tilde.size()) {
        throw std::runtime_error("GLWE ciphertext size mismatch");
    }

    std::size_t k = ct1.d_tilde.size();
    schemes::GLWECiphertext result(k, params.n);

    result.b = polynomial::add(ct1.b, ct2.b, params.q);

    for (std::size_t i = 0; i < k; ++i) {
        result.d_tilde[i] = polynomial::add(ct1.d_tilde[i], ct2.d_tilde[i], params.q);
    }

    return result;
}

schemes::GLWECiphertext subtract_glwe(
    const schemes::GLWECiphertext& ct1,
    const schemes::GLWECiphertext& ct2,
    const Parameters& params
) {
    if (ct1.d_tilde.size() != ct2.d_tilde.size()) {
        throw std::runtime_error("GLWE ciphertext size mismatch");
    }

    std::size_t k = ct1.d_tilde.size();
    schemes::GLWECiphertext result(k, params.n);

    result.b = polynomial::subtract(ct1.b, ct2.b, params.q);

    for (std::size_t i = 0; i < k; ++i) {
        result.d_tilde[i] = polynomial::subtract(ct1.d_tilde[i], ct2.d_tilde[i], params.q);
    }

    return result;
}

schemes::GLWECiphertext scalar_multiply_glwe(
    const schemes::GLWECiphertext& ct,
    int64 scalar,
    const Parameters& params
) {
    std::size_t k = ct.d_tilde.size();
    schemes::GLWECiphertext result(k, params.n);

    result.b = polynomial::scalar_multiply(ct.b, scalar, params.q);

    for (std::size_t i = 0; i < k; ++i) {
        result.d_tilde[i] = polynomial::scalar_multiply(ct.d_tilde[i], scalar, params.q);
    }

    return result;
}

// Placeholder implementations for advanced operations
schemes::LWECiphertext key_switch_lwe_to_lwe(
    const schemes::LWECiphertext& ct,
    const keys::LWESecretKey& from_key,
    const keys::LWESecretKey& to_key,
    const Parameters& params
) {
    // This is a placeholder - real key switching requires additional key switching keys
    // For now, just return the original ciphertext
    // TODO: Implement proper key switching
    return ct;
}

std::vector<int64> decompose(int64 value, int64 base, int levels) {
    std::vector<int64> result(levels);

    for (int i = 0; i < levels; ++i) {
        result[i] = value % base;
        value /= base;
    }

    return result;
}

}
}