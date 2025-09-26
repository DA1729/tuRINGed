#pragma once

// Core types and utilities
#include "turinged/core/types.hpp"
#include "turinged/core/math_utils.hpp"

// Polynomial operations
#include "turinged/polynomial/polynomial.hpp"

// Key management
#include "turinged/keys/keys.hpp"

// Cryptographic schemes
#include "turinged/schemes/lwe.hpp"
#include "turinged/schemes/rlwe.hpp"
#include "turinged/schemes/glwe.hpp"
#include "turinged/schemes/glev.hpp"
#include "turinged/schemes/ggsw.hpp"

// Homomorphic operations
#include "turinged/operations/homomorphic.hpp"

namespace turinged {

constexpr const char* VERSION = "0.1.0";

// Library-wide initialization (if needed in the future)
void initialize();
void cleanup();

}