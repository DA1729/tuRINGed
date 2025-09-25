#include "polynomial.hpp"
#include <algorithm>
#include <cmath>

namespace fhe {
namespace math {

template<typename T>
Polynomial<T> Polynomial<T>::operator-(const Polynomial& other) const {
    size_t maxDegree = std::max(degree(), other.degree());
    std::vector<T> result(maxDegree + 1);

    for (size_t i = 0; i <= maxDegree; ++i) {
        result[i] = (*this)[i] - other[i];
    }

    return Polynomial(result);
}

template<typename T>
Polynomial<T> Polynomial<T>::operator%(const Polynomial& modulus) const {
    if (modulus.isZero()) {
        throw std::invalid_argument("Division by zero polynomial");
    }

    Polynomial dividend = *this;
    const size_t divisorDegree = modulus.degree();

    while (!dividend.isZero() && dividend.degree() >= divisorDegree) {
        size_t dividendDegree = dividend.degree();
        T leadCoeff = dividend.coefficients[dividendDegree] / modulus.coefficients[divisorDegree];

        Polynomial term(dividendDegree - divisorDegree + 1, T());
        term.coefficients[dividendDegree - divisorDegree] = leadCoeff;

        dividend = dividend - term * modulus;
        dividend.trim();
    }

    return dividend;
}

template<typename T>
Polynomial<T>& Polynomial<T>::operator+=(const Polynomial& other) {
    *this = *this + other;
    return *this;
}

template<typename T>
Polynomial<T>& Polynomial<T>::operator-=(const Polynomial& other) {
    *this = *this - other;
    return *this;
}

template<typename T>
Polynomial<T>& Polynomial<T>::operator*=(const Polynomial& other) {
    *this = *this * other;
    return *this;
}

template<typename T>
Polynomial<T>& Polynomial<T>::operator%=(const Polynomial& modulus) {
    *this = *this % modulus;
    return *this;
}

template<typename T>
bool Polynomial<T>::operator==(const Polynomial& other) const {
    if (coefficients.size() != other.coefficients.size()) {
        return false;
    }

    for (size_t i = 0; i < coefficients.size(); ++i) {
        if (coefficients[i] != other.coefficients[i]) {
            return false;
        }
    }

    return true;
}

template<typename T>
bool Polynomial<T>::operator!=(const Polynomial& other) const {
    return !(*this == other);
}

template<typename T>
Polynomial<T> Polynomial<T>::derivative() const {
    if (coefficients.size() <= 1) {
        return Polynomial();
    }

    std::vector<T> result(coefficients.size() - 1);
    for (size_t i = 1; i < coefficients.size(); ++i) {
        result[i - 1] = coefficients[i] * T(i);
    }

    return Polynomial(result);
}

template<typename T>
Polynomial<T> Polynomial<T>::modPower(const BigInt& exponent, const Polynomial& modulus) const {
    if (exponent.isZero()) {
        return Polynomial({T(1)});
    }

    Polynomial result({T(1)});
    Polynomial base = *this % modulus;
    BigInt exp = exponent;

    while (!exp.isZero()) {
        if (exp.toString().back() == '1') {
            result = (result * base) % modulus;
        }
        base = (base * base) % modulus;
        exp = exp / BigInt(2);
    }

    return result;
}

template<typename T>
void Polynomial<T>::setCoefficients(const std::vector<T>& coeffs) {
    coefficients = coeffs;
    trim();
}

template<typename T>
void Polynomial<T>::ntt(std::vector<std::complex<double>>& a, bool invert) {
    int n = a.size();
    if (n == 1) return;

    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;

        if (i < j) {
            std::swap(a[i], a[j]);
        }
    }

    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2 * M_PI / len * (invert ? -1 : 1);
        std::complex<double> wlen(cos(ang), sin(ang));

        for (int i = 0; i < n; i += len) {
            std::complex<double> w(1);
            for (int j = 0; j < len / 2; j++) {
                std::complex<double> u = a[i + j];
                std::complex<double> v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }

    if (invert) {
        for (auto& x : a) {
            x /= n;
        }
    }
}

template<typename T>
Polynomial<T> Polynomial<T>::multiplyNTT(const Polynomial& other) const {
    // This method is only meaningful for numeric types that can convert to/from double
    // For BigInt, we'll use regular multiplication
    return *this * other;
}

// Cyclotomic polynomial generation functions
PolynomialZZ generateCyclotomicPolynomialZZ(size_t n) {
    if (n == 1) {
        std::vector<BigInt> coeffs = {BigInt(-1), BigInt(1)};
        return PolynomialZZ(coeffs);
    }

    std::vector<BigInt> coeffs(n + 1, BigInt(0));
    coeffs[0] = BigInt(-1);
    coeffs[n] = BigInt(1);

    return PolynomialZZ(coeffs);
}

PolynomialMod generateCyclotomicPolynomialMod(size_t n) {
    if (n == 1) {
        std::vector<uint64_t> coeffs = {UINT64_MAX, 1}; // UINT64_MAX represents -1 in modular arithmetic
        return PolynomialMod(coeffs);
    }

    std::vector<uint64_t> coeffs(n + 1, 0);
    coeffs[0] = UINT64_MAX; // UINT64_MAX represents -1 in modular arithmetic
    coeffs[n] = 1;

    return PolynomialMod(coeffs);
}

// Explicit instantiation must come after specializations
template class Polynomial<BigInt>;
template class Polynomial<uint64_t>;

} // namespace math
} // namespace fhe