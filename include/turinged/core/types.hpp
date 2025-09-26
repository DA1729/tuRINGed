#pragma once

#include <vector>
#include <cstdint>

namespace turinged {

using int64 = std::int64_t;
using uint64 = std::uint64_t;
using int128 = __int128;

using Polynomial = std::vector<int64>;

struct Parameters {
    std::size_t n;          // polynomial degree
    int64 q;                // ciphertext modulus
    int64 t;                // plaintext modulus
    int64 noise_bound;      // noise sampling bound

    Parameters(std::size_t n, int64 q, int64 t, int64 noise_bound)
        : n(n), q(q), t(t), noise_bound(noise_bound) {}
};

}