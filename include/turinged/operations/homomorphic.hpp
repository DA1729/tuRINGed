#pragma once

#include "turinged/core/types.hpp"
#include "turinged/schemes/lwe.hpp"
#include "turinged/schemes/rlwe.hpp"
#include "turinged/schemes/glwe.hpp"
#include "turinged/schemes/glev.hpp"
#include "turinged/schemes/ggsw.hpp"

namespace turinged {
namespace operations {

// LWE Homomorphic Operations
schemes::LWECiphertext add_lwe(
    const schemes::LWECiphertext& ct1,
    const schemes::LWECiphertext& ct2,
    const Parameters& params
);

schemes::LWECiphertext subtract_lwe(
    const schemes::LWECiphertext& ct1,
    const schemes::LWECiphertext& ct2,
    const Parameters& params
);

schemes::LWECiphertext scalar_multiply_lwe(
    const schemes::LWECiphertext& ct,
    int64 scalar,
    const Parameters& params
);

// RLWE Homomorphic Operations
schemes::RLWECiphertext add_rlwe(
    const schemes::RLWECiphertext& ct1,
    const schemes::RLWECiphertext& ct2,
    const Parameters& params
);

schemes::RLWECiphertext subtract_rlwe(
    const schemes::RLWECiphertext& ct1,
    const schemes::RLWECiphertext& ct2,
    const Parameters& params
);

schemes::RLWECiphertext scalar_multiply_rlwe(
    const schemes::RLWECiphertext& ct,
    int64 scalar,
    const Parameters& params
);

schemes::RLWECiphertext multiply_rlwe(
    const schemes::RLWECiphertext& ct1,
    const schemes::RLWECiphertext& ct2,
    const Parameters& params
);

// GLWE Homomorphic Operations
schemes::GLWECiphertext add_glwe(
    const schemes::GLWECiphertext& ct1,
    const schemes::GLWECiphertext& ct2,
    const Parameters& params
);

schemes::GLWECiphertext subtract_glwe(
    const schemes::GLWECiphertext& ct1,
    const schemes::GLWECiphertext& ct2,
    const Parameters& params
);

schemes::GLWECiphertext scalar_multiply_glwe(
    const schemes::GLWECiphertext& ct,
    int64 scalar,
    const Parameters& params
);

// Key switching and decomposition functions (placeholders for future implementation)
schemes::LWECiphertext key_switch_lwe_to_lwe(
    const schemes::LWECiphertext& ct,
    const keys::LWESecretKey& from_key,
    const keys::LWESecretKey& to_key,
    const Parameters& params
);

std::vector<int64> decompose(int64 value, int64 base, int levels);

}
}