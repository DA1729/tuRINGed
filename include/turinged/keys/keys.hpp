#pragma once

#include "turinged/core/types.hpp"

namespace turinged {
namespace keys {

struct LWESecretKey {
    std::vector<int64> s;

    LWESecretKey() = default;
    explicit LWESecretKey(std::size_t k) : s(k) {}
};

struct LWEPublicKey {
    std::vector<int64> a;

    LWEPublicKey() = default;
    explicit LWEPublicKey(std::size_t k) : a(k) {}
};

struct RLWESecretKey {
    Polynomial s;

    RLWESecretKey() = default;
    explicit RLWESecretKey(std::size_t n) : s(n) {}
};

struct GLWESecretKey {
    std::vector<Polynomial> s;

    GLWESecretKey() = default;
    GLWESecretKey(std::size_t k, std::size_t n) : s(k, Polynomial(n)) {}
};

struct GLWEPublicKey {
    Polynomial pk1;                    // AS + E
    std::vector<Polynomial> pk2;       // A

    GLWEPublicKey() = default;
    GLWEPublicKey(std::size_t k, std::size_t n) : pk1(n), pk2(k, Polynomial(n)) {}
};

LWESecretKey generate_lwe_secret_key(std::size_t k);

RLWESecretKey generate_rlwe_secret_key(std::size_t n);

GLWESecretKey generate_glwe_secret_key(std::size_t k, std::size_t n);

GLWEPublicKey generate_glwe_public_key(const GLWESecretKey& sk, const Parameters& params);

}
}