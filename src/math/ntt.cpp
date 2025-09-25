#include "ntt.hpp"
#include <algorithm>
#include <stdexcept>
#include <cmath>

namespace fhe {
namespace math {

NTT::NTT(uint64_t modulus, uint64_t primitiveRoot)
    : modulus(modulus), rootOfUnity(primitiveRoot) {

    if (!isPrime(modulus)) {
        throw std::invalid_argument("Modulus must be prime");
    }

    size_t maxN = 1;
    uint64_t temp = modulus - 1;
    while (temp % 2 == 0) {
        maxN *= 2;
        temp /= 2;
    }

    roots.resize(maxN);
    invRoots.resize(maxN);

    uint64_t root = modPow(primitiveRoot, (modulus - 1) / maxN, modulus);
    uint64_t invRoot = modInverse(root, modulus);

    roots[0] = invRoots[0] = 1;
    for (size_t i = 1; i < maxN; ++i) {
        roots[i] = (roots[i - 1] * root) % modulus;
        invRoots[i] = (invRoots[i - 1] * invRoot) % modulus;
    }
}

uint64_t NTT::modPow(uint64_t base, uint64_t exp, uint64_t mod) const {
    uint64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) {
            result = (__uint128_t(result) * base) % mod;
        }
        base = (__uint128_t(base) * base) % mod;
        exp >>= 1;
    }
    return result;
}

uint64_t NTT::modInverse(uint64_t a, uint64_t mod) const {
    return modPow(a, mod - 2, mod);
}

void NTT::forward(std::vector<uint64_t>& data) const {
    size_t n = data.size();
    if (n == 1 || (n & (n - 1)) != 0) {
        throw std::invalid_argument("Data size must be a power of 2");
    }

    for (size_t i = 1, j = 0; i < n; ++i) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;
        if (i < j) {
            std::swap(data[i], data[j]);
        }
    }

    for (size_t len = 2; len <= n; len <<= 1) {
        size_t step = roots.size() / len;
        for (size_t i = 0; i < n; i += len) {
            for (size_t j = 0; j < len / 2; ++j) {
                uint64_t u = data[i + j];
                uint64_t v = (__uint128_t(data[i + j + len / 2]) * roots[step * j]) % modulus;
                data[i + j] = (u + v) % modulus;
                data[i + j + len / 2] = (u - v + modulus) % modulus;
            }
        }
    }
}

void NTT::inverse(std::vector<uint64_t>& data) const {
    size_t n = data.size();
    if (n == 1 || (n & (n - 1)) != 0) {
        throw std::invalid_argument("Data size must be a power of 2");
    }

    for (size_t i = 1, j = 0; i < n; ++i) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;
        if (i < j) {
            std::swap(data[i], data[j]);
        }
    }

    for (size_t len = 2; len <= n; len <<= 1) {
        size_t step = invRoots.size() / len;
        for (size_t i = 0; i < n; i += len) {
            for (size_t j = 0; j < len / 2; ++j) {
                uint64_t u = data[i + j];
                uint64_t v = (__uint128_t(data[i + j + len / 2]) * invRoots[step * j]) % modulus;
                data[i + j] = (u + v) % modulus;
                data[i + j + len / 2] = (u - v + modulus) % modulus;
            }
        }
    }

    uint64_t invN = modInverse(n, modulus);
    for (uint64_t& x : data) {
        x = (__uint128_t(x) * invN) % modulus;
    }
}

std::vector<uint64_t> NTT::multiply(const std::vector<uint64_t>& a,
                                    const std::vector<uint64_t>& b) const {
    size_t n = 1;
    while (n < a.size() + b.size()) {
        n <<= 1;
    }

    std::vector<uint64_t> fa(a.begin(), a.end());
    std::vector<uint64_t> fb(b.begin(), b.end());
    fa.resize(n);
    fb.resize(n);

    forward(fa);
    forward(fb);

    for (size_t i = 0; i < n; ++i) {
        fa[i] = (__uint128_t(fa[i]) * fb[i]) % modulus;
    }

    inverse(fa);

    while (fa.size() > 1 && fa.back() == 0) {
        fa.pop_back();
    }

    return fa;
}

bool NTT::isPrime(uint64_t n) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (uint64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

uint64_t NTT::findPrimitiveRoot(uint64_t prime) {
    if (!isPrime(prime)) {
        throw std::invalid_argument("Input must be prime");
    }

    uint64_t phi = prime - 1;
    std::vector<uint64_t> factors;

    uint64_t temp = phi;
    for (uint64_t i = 2; i * i <= temp; ++i) {
        if (temp % i == 0) {
            factors.push_back(i);
            while (temp % i == 0) {
                temp /= i;
            }
        }
    }
    if (temp > 1) {
        factors.push_back(temp);
    }

    for (uint64_t g = 2; g < prime; ++g) {
        bool isPrimitive = true;
        for (uint64_t factor : factors) {
            NTT helper(prime, 2);
            if (helper.modPow(g, phi / factor, prime) == 1) {
                isPrimitive = false;
                break;
            }
        }
        if (isPrimitive) {
            return g;
        }
    }

    throw std::runtime_error("No primitive root found");
}

CRT::CRT(const std::vector<uint64_t>& moduli) : moduli(moduli) {
    size_t n = moduli.size();
    partialProducts.resize(n);
    inverses.resize(n);

    BigInt product(1);
    for (uint64_t mod : moduli) {
        product *= BigInt(mod);
    }

    for (size_t i = 0; i < n; ++i) {
        BigInt Mi = product / BigInt(moduli[i]);
        partialProducts[i] = (Mi % BigInt(moduli[i])).toString().empty() ? 0 :
                            std::stoull((Mi % BigInt(moduli[i])).toString());

        BigInt inv = Mi.modInverse(BigInt(moduli[i]));
        inverses[i] = inv.toString().empty() ? 0 : std::stoull(inv.toString());
    }
}

BigInt CRT::reconstruct(const std::vector<uint64_t>& remainders) const {
    if (remainders.size() != moduli.size()) {
        throw std::invalid_argument("Number of remainders must match number of moduli");
    }

    BigInt result(0);
    BigInt product(1);
    for (uint64_t mod : moduli) {
        product *= BigInt(mod);
    }

    for (size_t i = 0; i < moduli.size(); ++i) {
        BigInt Mi = product / BigInt(moduli[i]);
        BigInt yi = Mi.modInverse(BigInt(moduli[i]));
        result += BigInt(remainders[i]) * Mi * yi;
    }

    return result % product;
}

std::vector<uint64_t> CRT::reduce(const BigInt& value) const {
    std::vector<uint64_t> remainders(moduli.size());
    for (size_t i = 0; i < moduli.size(); ++i) {
        BigInt remainder = value % BigInt(moduli[i]);
        remainders[i] = remainder.toString().empty() ? 0 : std::stoull(remainder.toString());
    }
    return remainders;
}

} // namespace math
} // namespace fhe