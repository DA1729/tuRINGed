#pragma once

#include "turinged/core/types.hpp"
#include "turinged/keys/keys.hpp"

namespace turinged {
namespace schemes {

struct RLWECiphertext {
    Polynomial a;
    Polynomial b;

    RLWECiphertext() = default;
    RLWECiphertext(std::size_t n) : a(n), b(n) {}
};

RLWECiphertext encrypt_rlwe(
    const Polynomial& message,
    const keys::RLWESecretKey& sk,
    const Parameters& params
);

Polynomial decrypt_rlwe(
    const RLWECiphertext& ct,
    const keys::RLWESecretKey& sk,
    const Parameters& params
);

}
}