#pragma once

#include "bigint.hpp"
#include <vector>
#include <complex>

namespace fhe {
namespace math {

template<typename T>
class Polynomial {
private:
    std::vector<T> coefficients;

    void trim();

public:
    Polynomial();
    Polynomial(const std::vector<T>& coeffs);
    Polynomial(size_t degree, const T& value = T());

    size_t degree() const;
    bool isZero() const;

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    Polynomial operator+(const Polynomial& other) const;
    Polynomial operator-(const Polynomial& other) const;
    Polynomial operator*(const Polynomial& other) const;
    Polynomial operator%(const Polynomial& modulus) const;

    Polynomial& operator+=(const Polynomial& other);
    Polynomial& operator-=(const Polynomial& other);
    Polynomial& operator*=(const Polynomial& other);
    Polynomial& operator%=(const Polynomial& modulus);

    bool operator==(const Polynomial& other) const;
    bool operator!=(const Polynomial& other) const;

    T evaluate(const T& x) const;

    Polynomial derivative() const;

    Polynomial modPower(const BigInt& exponent, const Polynomial& modulus) const;

    std::vector<T> getCoefficients() const { return coefficients; }
    void setCoefficients(const std::vector<T>& coeffs);

    // Cyclotomic polynomial generation moved to separate functions

    static void ntt(std::vector<std::complex<double>>& a, bool invert = false);
    Polynomial multiplyNTT(const Polynomial& other) const;
};

using PolynomialZZ = Polynomial<BigInt>;
using PolynomialMod = Polynomial<uint64_t>;

template<typename T>
Polynomial<T>::Polynomial() : coefficients(1, T()) {}

template<typename T>
Polynomial<T>::Polynomial(const std::vector<T>& coeffs) : coefficients(coeffs) {
    trim();
}

template<typename T>
Polynomial<T>::Polynomial(size_t degree, const T& value) : coefficients(degree + 1, value) {}

template<typename T>
void Polynomial<T>::trim() {
    while (coefficients.size() > 1 && coefficients.back() == T()) {
        coefficients.pop_back();
    }
}

template<typename T>
size_t Polynomial<T>::degree() const {
    return coefficients.size() > 0 ? coefficients.size() - 1 : 0;
}

template<typename T>
bool Polynomial<T>::isZero() const {
    return coefficients.size() == 1 && coefficients[0] == T();
}

template<typename T>
T& Polynomial<T>::operator[](size_t index) {
    if (index >= coefficients.size()) {
        coefficients.resize(index + 1, T());
    }
    return coefficients[index];
}

template<typename T>
const T& Polynomial<T>::operator[](size_t index) const {
    static T zero;
    return index < coefficients.size() ? coefficients[index] : zero;
}

template<typename T>
Polynomial<T> Polynomial<T>::operator+(const Polynomial& other) const {
    size_t maxDegree = std::max(degree(), other.degree());
    std::vector<T> result(maxDegree + 1);

    for (size_t i = 0; i <= maxDegree; ++i) {
        result[i] = (*this)[i] + other[i];
    }

    return Polynomial(result);
}

template<typename T>
Polynomial<T> Polynomial<T>::operator*(const Polynomial& other) const {
    if (isZero() || other.isZero()) {
        return Polynomial();
    }

    std::vector<T> result(degree() + other.degree() + 1, T());

    for (size_t i = 0; i <= degree(); ++i) {
        for (size_t j = 0; j <= other.degree(); ++j) {
            result[i + j] += coefficients[i] * other.coefficients[j];
        }
    }

    return Polynomial(result);
}

template<typename T>
T Polynomial<T>::evaluate(const T& x) const {
    if (coefficients.empty()) return T();

    T result = coefficients.back();
    for (int i = coefficients.size() - 2; i >= 0; --i) {
        result = result * x + coefficients[i];
    }

    return result;
}

// Cyclotomic polynomial generation functions
PolynomialZZ generateCyclotomicPolynomialZZ(size_t n);
PolynomialMod generateCyclotomicPolynomialMod(size_t n);

extern template class Polynomial<BigInt>;
extern template class Polynomial<uint64_t>;

} // namespace math
} // namespace fhe