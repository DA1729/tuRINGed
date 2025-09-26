#include "turinged/polynomial/polynomial.hpp"
#include "turinged/core/math_utils.hpp"
#include <iostream>
#include <algorithm>
#include <stdexcept>

namespace turinged {
namespace polynomial {

Polynomial add(const Polynomial& a, const Polynomial& b, int64 q) {
    if (a.size() != b.size()) {
        throw std::runtime_error("Polynomial size mismatch in addition");
    }

    std::size_t n = a.size();
    Polynomial result(n);
    for (std::size_t i = 0; i < n; i++) {
        result[i] = core::modq(a[i] + b[i], q);
    }
    return result;
}

Polynomial subtract(const Polynomial& a, const Polynomial& b, int64 q) {
    if (a.size() != b.size()) {
        throw std::runtime_error("Polynomial size mismatch in subtraction");
    }

    std::size_t n = a.size();
    Polynomial result(n);
    for (std::size_t i = 0; i < n; i++) {
        result[i] = core::modq(a[i] - b[i], q);
    }
    return result;
}

Polynomial scalar_multiply(const Polynomial& a, int64 scalar, int64 q) {
    std::size_t n = a.size();
    Polynomial result(n);
    for (std::size_t i = 0; i < n; i++) {
        int128 tmp = static_cast<int128>(a[i]) * scalar;
        result[i] = core::modq(static_cast<int64>(tmp % q), q);
    }
    return result;
}

Polynomial negate(const Polynomial& a, int64 q) {
    std::size_t n = a.size();
    Polynomial result(n);
    for (std::size_t i = 0; i < n; i++) {
        result[i] = core::modq(-a[i], q);
    }
    return result;
}

Polynomial negacyclic_multiply(const Polynomial& a, const Polynomial& b, int64 q) {
    if (a.size() != b.size()) {
        throw std::runtime_error("Polynomial size mismatch in multiplication");
    }

    std::size_t n = a.size();
    Polynomial result(n, 0);

    for (std::size_t i = 0; i < n; i++) {
        for (std::size_t j = 0; j < n; j++) {
            std::size_t idx = j + i;
            int128 prod = static_cast<int128>(a[i]) * static_cast<int128>(b[j]);

            if (idx < n) {
                int128 tmp = static_cast<int128>(result[idx]) + prod;
                result[idx] = static_cast<int64>(tmp % q);
            } else {
                std::size_t idx2 = idx - n;
                int128 tmp = static_cast<int128>(result[idx2]) - prod;
                result[idx2] = static_cast<int64>(((tmp % q) + q) % q);
            }
        }
    }

    for (std::size_t i = 0; i < n; i++) {
        result[i] = core::modq(result[i], q);
    }

    return result;
}

std::vector<int64> center_representation(const Polynomial& a, int64 q) {
    std::size_t n = a.size();
    std::vector<int64> result(n);
    for (std::size_t i = 0; i < n; i++) {
        result[i] = core::center_rep(a[i], q);
    }
    return result;
}

bool is_equal(const Polynomial& a, const Polynomial& b) {
    if (a.size() != b.size()) return false;
    return std::equal(a.begin(), a.end(), b.begin());
}

void print_polynomial(const Polynomial& poly, const std::string& name, std::size_t max_coeffs) {
    if (!name.empty()) {
        std::cout << name << " (first " << std::min(poly.size(), max_coeffs) << " coeffs): ";
    }

    for (std::size_t i = 0; i < std::min(poly.size(), max_coeffs); i++) {
        std::cout << poly[i] << " ";
    }

    if (poly.size() > max_coeffs) {
        std::cout << "...";
    }
    std::cout << std::endl;
}

}
}