#pragma once

#include "bigint.hpp"
#include <vector>
#include <cstdint>

namespace fhe {
namespace math {

class NTT {
private:
    uint64_t modulus;
    uint64_t rootOfUnity;
    std::vector<uint64_t> roots;
    std::vector<uint64_t> invRoots;

    uint64_t modPow(uint64_t base, uint64_t exp, uint64_t mod) const;
    uint64_t modInverse(uint64_t a, uint64_t mod) const;

public:
    NTT(uint64_t modulus, uint64_t primitiveRoot);

    void forward(std::vector<uint64_t>& data) const;
    void inverse(std::vector<uint64_t>& data) const;

    std::vector<uint64_t> multiply(const std::vector<uint64_t>& a,
                                   const std::vector<uint64_t>& b) const;

    static bool isPrime(uint64_t n);
    static uint64_t findPrimitiveRoot(uint64_t prime);
    static std::vector<uint64_t> generateNTTPrimes(size_t count, size_t bitSize = 50);

    uint64_t getModulus() const { return modulus; }
    size_t getMaxLength() const { return roots.size(); }
};

class CRT {
private:
    std::vector<uint64_t> moduli;
    std::vector<uint64_t> partialProducts;
    std::vector<uint64_t> inverses;

public:
    CRT(const std::vector<uint64_t>& moduli);

    BigInt reconstruct(const std::vector<uint64_t>& remainders) const;
    std::vector<uint64_t> reduce(const BigInt& value) const;

    size_t getNumModuli() const { return moduli.size(); }
    const std::vector<uint64_t>& getModuli() const { return moduli; }
};

} // namespace math
} // namespace fhe