#pragma once

#include "turinged/core/types.hpp"
#include "turinged/keys/keys.hpp"

namespace turinged {
namespace schemes {

struct LWECiphertext {
    std::vector<int64> a;
    int64 b;

    LWECiphertext() = default;
    LWECiphertext(std::size_t k) : a(k), b(0) {}
};

LWECiphertext encrypt_lwe(
    int64 message,
    const keys::LWESecretKey& sk,
    const Parameters& params
);

int64 decrypt_lwe(
    const LWECiphertext& ct,
    const keys::LWESecretKey& sk,
    const Parameters& params
);

}
}