#pragma once

#include "turinged/core/types.hpp"
#include "turinged/keys/keys.hpp"
#include "turinged/schemes/glev.hpp"

namespace turinged {
namespace schemes {

struct GGSWCiphertext {
    std::vector<GLevCiphertext> glev_rows;

    GGSWCiphertext() = default;
    explicit GGSWCiphertext(std::size_t k) : glev_rows(k + 1) {}
};

GGSWCiphertext encrypt_ggsw(
    const Polynomial& message,
    const keys::GLWEPublicKey& pk,
    const keys::GLWESecretKey& sk,
    const Parameters& params,
    int l,
    int64 beta
);

Polynomial decrypt_ggsw(
    const GGSWCiphertext& ct,
    const keys::GLWESecretKey& sk,
    const Parameters& params,
    int level_idx,
    int64 beta
);

}
}