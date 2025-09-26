#pragma once

#include "turinged/core/types.hpp"
#include <string>

namespace turinged {
namespace polynomial {

Polynomial add(const Polynomial& a, const Polynomial& b, int64 q);

Polynomial subtract(const Polynomial& a, const Polynomial& b, int64 q);

Polynomial scalar_multiply(const Polynomial& a, int64 scalar, int64 q);

Polynomial negate(const Polynomial& a, int64 q);

Polynomial negacyclic_multiply(const Polynomial& a, const Polynomial& b, int64 q);

std::vector<int64> center_representation(const Polynomial& a, int64 q);

bool is_equal(const Polynomial& a, const Polynomial& b);

void print_polynomial(const Polynomial& poly, const std::string& name = "", std::size_t max_coeffs = 10);

}
}