#pragma once

#include "turinged/core/types.hpp"
#include "turinged/keys/keys.hpp"
#include "turinged/schemes/glwe.hpp"

namespace turinged {
namespace schemes {

struct GLevCiphertext {
    std::vector<GLWECiphertext> levels;

    GLevCiphertext() = default;
    explicit GLevCiphertext(int l) : levels(l + 1) {}
};

GLevCiphertext encrypt_glev(
    const Polynomial& message,
    const keys::GLWEPublicKey& pk,
    const Parameters& params,
    int l,
    int64 beta
);

Polynomial decrypt_glev_level(
    const GLevCiphertext& ct,
    const keys::GLWESecretKey& sk,
    const Parameters& params,
    int level_idx,
    int64 beta
);

}
}