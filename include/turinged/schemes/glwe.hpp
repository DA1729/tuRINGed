#pragma once

#include "turinged/core/types.hpp"
#include "turinged/keys/keys.hpp"

namespace turinged {
namespace schemes {

struct GLWECiphertext {
    Polynomial b;
    std::vector<Polynomial> d_tilde;

    GLWECiphertext() = default;
    GLWECiphertext(std::size_t k, std::size_t n) : b(n), d_tilde(k, Polynomial(n)) {}
};

GLWECiphertext encrypt_glwe(
    const Polynomial& message,
    const keys::GLWEPublicKey& pk,
    const Parameters& params
);

Polynomial decrypt_glwe(
    const GLWECiphertext& ct,
    const keys::GLWESecretKey& sk,
    const Parameters& params
);

}
}