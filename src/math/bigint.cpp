#include "bigint.hpp"
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace fhe {
namespace math {

BigInt::BigInt() : digits(1, 0), negative(false) {}

BigInt::BigInt(int64_t value) : negative(value < 0) {
    uint64_t absValue = negative ? -value : value;
    if (absValue == 0) {
        digits = {0};
    } else {
        digits.push_back(absValue);
    }
}

BigInt::BigInt(const std::string& str) {
    if (str.empty()) {
        throw std::invalid_argument("Empty string");
    }

    size_t start = 0;
    negative = (str[0] == '-');
    if (str[0] == '-' || str[0] == '+') {
        start = 1;
    }

    digits.clear();
    uint64_t current = 0;
    uint64_t multiplier = 1;

    for (int i = str.length() - 1; i >= static_cast<int>(start); --i) {
        if (!std::isdigit(str[i])) {
            throw std::invalid_argument("Invalid character in string");
        }

        current += (str[i] - '0') * multiplier;
        multiplier *= 10;

        if (multiplier > BASE / 10) {
            digits.push_back(current);
            current = 0;
            multiplier = 1;
        }
    }

    if (current > 0 || digits.empty()) {
        digits.push_back(current);
    }

    removeLeadingZeros();
}

BigInt::BigInt(const std::vector<uint64_t>& digits, bool neg) : digits(digits), negative(neg) {
    removeLeadingZeros();
}

void BigInt::removeLeadingZeros() {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
    if (digits.size() == 1 && digits[0] == 0) {
        negative = false;
    }
}

BigInt BigInt::operator+(const BigInt& other) const {
    if (negative != other.negative) {
        if (negative) {
            return other - BigInt(digits, false);
        } else {
            return *this - BigInt(other.digits, false);
        }
    }

    std::vector<uint64_t> result;
    uint64_t carry = 0;
    size_t maxSize = std::max(digits.size(), other.digits.size());

    for (size_t i = 0; i < maxSize || carry; ++i) {
        uint64_t sum = carry;
        if (i < digits.size()) sum += digits[i];
        if (i < other.digits.size()) sum += other.digits[i];

        result.push_back(sum);
        carry = (sum < digits[i] || sum < other.digits[i]) ? 1 : 0;
    }

    return BigInt(result, negative);
}

BigInt BigInt::operator-(const BigInt& other) const {
    if (negative != other.negative) {
        return *this + BigInt(other.digits, !other.negative);
    }

    if ((!negative && *this < other) || (negative && *this > other)) {
        return -(other - *this);
    }

    std::vector<uint64_t> result;
    uint64_t borrow = 0;

    for (size_t i = 0; i < digits.size(); ++i) {
        uint64_t sub = borrow;
        if (i < other.digits.size()) sub += other.digits[i];

        if (digits[i] >= sub) {
            result.push_back(digits[i] - sub);
            borrow = 0;
        } else {
            result.push_back(digits[i] + (BASE - sub) + 1);
            borrow = 1;
        }
    }

    return BigInt(result, negative);
}

BigInt BigInt::operator-() const {
    if (isZero()) return *this;
    return BigInt(digits, !negative);
}

BigInt BigInt::operator*(const BigInt& other) const {
    if (isZero() || other.isZero()) {
        return BigInt(0);
    }

    if (digits.size() > 10 && other.digits.size() > 10) {
        return multiplyKaratsuba(other);
    }

    std::vector<uint64_t> result(digits.size() + other.digits.size(), 0);

    for (size_t i = 0; i < digits.size(); ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < other.digits.size() || carry; ++j) {
            uint64_t prod = result[i + j] + carry;
            if (j < other.digits.size()) {
                prod += digits[i] * other.digits[j];
            }
            result[i + j] = prod;
            carry = (prod < result[i + j]) ? 1 : 0;
        }
    }

    return BigInt(result, negative != other.negative);
}

BigInt BigInt::powMod(const BigInt& exponent, const BigInt& modulus) const {
    if (modulus.isZero()) {
        throw std::invalid_argument("Modulus cannot be zero");
    }
    if (exponent.isNegative()) {
        return modInverse(modulus).powMod(-exponent, modulus);
    }

    BigInt result(1);
    BigInt base = *this % modulus;
    BigInt exp = exponent;

    while (!exp.isZero()) {
        if (exp.digits[0] & 1) {
            result = (result * base) % modulus;
        }
        base = (base * base) % modulus;
        exp >>= 1;
    }

    return result;
}

BigInt BigInt::modInverse(const BigInt& modulus) const {
    if (modulus.isZero()) {
        throw std::invalid_argument("Modulus cannot be zero");
    }

    BigInt a = *this % modulus;
    BigInt m = modulus;
    BigInt x0(0), x1(1);

    if (m == BigInt(1)) return BigInt(0);

    while (a > BigInt(1)) {
        BigInt q = a / m;
        BigInt t = m;
        m = a % m;
        a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < BigInt(0)) x1 += modulus;
    return x1;
}

BigInt BigInt::gcd(const BigInt& other) const {
    BigInt a = *this;
    BigInt b = other;

    if (a.isNegative()) a = -a;
    if (b.isNegative()) b = -b;

    while (!b.isZero()) {
        BigInt temp = b;
        b = a % b;
        a = temp;
    }

    return a;
}

bool BigInt::isZero() const {
    return digits.size() == 1 && digits[0] == 0;
}

bool BigInt::isOne() const {
    return !negative && digits.size() == 1 && digits[0] == 1;
}

bool BigInt::isNegative() const {
    return negative && !isZero();
}

std::string BigInt::toString() const {
    if (isZero()) return "0";

    std::string result;
    if (negative) result += "-";

    std::vector<uint64_t> temp = digits;
    while (!temp.empty() && !(temp.size() == 1 && temp[0] == 0)) {
        uint64_t remainder = 0;
        for (int i = temp.size() - 1; i >= 0; --i) {
            uint64_t current = remainder * (BASE + 1) + temp[i];
            temp[i] = current / 10;
            remainder = current % 10;
        }

        result = char('0' + remainder) + result;

        while (!temp.empty() && temp.back() == 0) {
            temp.pop_back();
        }
    }

    return result;
}

std::ostream& operator<<(std::ostream& os, const BigInt& bi) {
    os << bi.toString();
    return os;
}

BigInt BigInt::operator%(const BigInt& other) const {
    if (other.isZero()) {
        throw std::invalid_argument("Division by zero");
    }
    return *this - (*this / other) * other;
}

BigInt BigInt::operator/(const BigInt& other) const {
    if (other.isZero()) {
        throw std::invalid_argument("Division by zero");
    }

    if (isZero()) return BigInt(0);

    BigInt dividend = *this;
    BigInt divisor = other;
    bool resultNegative = dividend.negative != divisor.negative;

    dividend.negative = false;
    divisor.negative = false;

    if (dividend < divisor) {
        return BigInt(0);
    }

    // Simple division algorithm for now
    BigInt quotient(0);
    BigInt remainder(0);

    // Long division approach
    while (dividend >= divisor) {
        BigInt temp = divisor;
        BigInt count(1);

        // Find the largest multiple
        while (dividend >= (temp << 1)) {
            temp <<= 1;
            count <<= 1;
        }

        dividend -= temp;
        quotient += count;
    }

    quotient.negative = resultNegative;
    quotient.removeLeadingZeros();
    return quotient;
}

bool BigInt::operator<(const BigInt& other) const {
    if (negative != other.negative) {
        return negative && !other.negative;
    }

    if (digits.size() != other.digits.size()) {
        return negative ? digits.size() > other.digits.size() : digits.size() < other.digits.size();
    }

    for (int i = digits.size() - 1; i >= 0; --i) {
        if (digits[i] != other.digits[i]) {
            return negative ? digits[i] > other.digits[i] : digits[i] < other.digits[i];
        }
    }

    return false;
}

bool BigInt::operator==(const BigInt& other) const {
    return negative == other.negative && digits == other.digits;
}

bool BigInt::operator!=(const BigInt& other) const {
    return !(*this == other);
}

bool BigInt::operator>(const BigInt& other) const {
    return other < *this;
}

bool BigInt::operator<=(const BigInt& other) const {
    return !(other < *this);
}

bool BigInt::operator>=(const BigInt& other) const {
    return !(*this < other);
}

BigInt& BigInt::operator+=(const BigInt& other) {
    *this = *this + other;
    return *this;
}

BigInt& BigInt::operator-=(const BigInt& other) {
    *this = *this - other;
    return *this;
}

BigInt& BigInt::operator*=(const BigInt& other) {
    *this = *this * other;
    return *this;
}

BigInt& BigInt::operator/=(const BigInt& other) {
    *this = *this / other;
    return *this;
}

BigInt& BigInt::operator%=(const BigInt& other) {
    *this = *this % other;
    return *this;
}

BigInt BigInt::operator<<(size_t shift) const {
    if (isZero() || shift == 0) return *this;

    BigInt result = *this;
    result <<= shift;
    return result;
}

BigInt BigInt::operator>>(size_t shift) const {
    if (isZero() || shift == 0) return *this;

    BigInt result = *this;
    result >>= shift;
    return result;
}

BigInt& BigInt::operator<<=(size_t shift) {
    if (isZero() || shift == 0) return *this;

    for (size_t i = 0; i < shift; ++i) {
        *this = *this * BigInt(2);
    }
    return *this;
}

BigInt& BigInt::operator>>=(size_t shift) {
    if (isZero() || shift == 0) return *this;

    for (size_t i = 0; i < shift; ++i) {
        *this = *this / BigInt(2);
    }
    return *this;
}

BigInt BigInt::multiplyKaratsuba(const BigInt& other) const {
    // For now, fallback to regular multiplication
    return *this * other;
}

} // namespace math
} // namespace fhe