#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cstdint>

namespace fhe {
namespace math {

class BigInt {
private:
    std::vector<uint64_t> digits;
    bool negative;
    static const uint64_t BASE = UINT64_MAX;

    void removeLeadingZeros();
    BigInt multiplyKaratsuba(const BigInt& other) const;

public:
    BigInt();
    BigInt(int64_t value);
    BigInt(const std::string& str);
    BigInt(const std::vector<uint64_t>& digits, bool neg = false);

    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator-() const; // Unary minus
    BigInt operator*(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator%(const BigInt& other) const;

    BigInt& operator+=(const BigInt& other);
    BigInt& operator-=(const BigInt& other);
    BigInt& operator*=(const BigInt& other);
    BigInt& operator/=(const BigInt& other);
    BigInt& operator%=(const BigInt& other);

    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;
    bool operator<(const BigInt& other) const;
    bool operator>(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator>=(const BigInt& other) const;

    BigInt operator<<(size_t shift) const;
    BigInt operator>>(size_t shift) const;
    BigInt& operator<<=(size_t shift);
    BigInt& operator>>=(size_t shift);

    BigInt pow(const BigInt& exponent) const;
    BigInt powMod(const BigInt& exponent, const BigInt& modulus) const;
    BigInt modInverse(const BigInt& modulus) const;
    BigInt gcd(const BigInt& other) const;

    bool isZero() const;
    bool isOne() const;
    bool isNegative() const;
    size_t bitLength() const;

    std::string toString() const;
    std::string toHexString() const;

    friend std::ostream& operator<<(std::ostream& os, const BigInt& bi);
    friend std::istream& operator>>(std::istream& is, BigInt& bi);
};

} // namespace math
} // namespace fhe