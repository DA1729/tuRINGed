# Math Module

The math module provides fundamental mathematical operations and data structures required for Fully Homomorphic Encryption (FHE) implementations.

## Components

### BigInt (`bigint.hpp`, `bigint.cpp`)
Arbitrary precision integer arithmetic implementation supporting:
- Basic arithmetic operations (+, -, *, /, %)
- Modular arithmetic (modular exponentiation, modular inverse)
- Bitwise operations and shifts
- GCD computation
- String conversion and I/O

**Key Features:**
- Optimized for cryptographic operations
- Karatsuba multiplication for large numbers
- Montgomery modular arithmetic support
- Thread-safe operations

### Polynomial (`polynomial.hpp`, `polynomial.cpp`)
Polynomial arithmetic over various coefficient rings:
- Addition, subtraction, multiplication of polynomials
- Modular polynomial operations
- Polynomial evaluation and derivatives
- Cyclotomic polynomial generation
- NTT-based fast polynomial multiplication

**Supported Types:**
- `PolynomialZZ`: Polynomials over arbitrary precision integers
- `PolynomialMod`: Polynomials over finite fields

### Matrix (`matrix.hpp`, `matrix.cpp`)
Dense matrix operations for lattice-based cryptography:
- Basic matrix arithmetic (+, -, *)
- Matrix transposition and determinant calculation
- Identity and zero matrix generation
- Scalar multiplication
- Template-based for different coefficient types

### Number Theoretic Transform (`ntt.hpp`, `ntt.cpp`)
Fast convolution and polynomial multiplication using NTT:
- Forward and inverse NTT transforms
- Optimized polynomial multiplication
- Chinese Remainder Theorem (CRT) implementation
- Prime generation utilities for NTT-friendly moduli

**Key Classes:**
- `NTT`: Number Theoretic Transform operations
- `CRT`: Chinese Remainder Theorem reconstruction

## Usage Examples

### BigInt Operations
```cpp
#include "math/bigint.hpp"
using namespace fhe::math;

BigInt a("12345678901234567890");
BigInt b("98765432109876543210");
BigInt result = a * b;
BigInt modResult = a.powMod(b, BigInt("1000000007"));
```

### Polynomial Arithmetic
```cpp
#include "math/polynomial.hpp"
using namespace fhe::math;

PolynomialZZ p1({BigInt(1), BigInt(2), BigInt(3)});  // 1 + 2x + 3x^2
PolynomialZZ p2({BigInt(4), BigInt(5)});             // 4 + 5x
PolynomialZZ product = p1 * p2;

// Cyclotomic polynomial
auto cyclotomic = PolynomialZZ::generateCyclotomicPolynomial(8);
```

### Matrix Operations
```cpp
#include "math/matrix.hpp"
using namespace fhe::math;

MatrixZZ A(3, 3, BigInt(0));
MatrixZZ B = MatrixZZ::identity(3);
MatrixZZ C = A * B;
```

### NTT-based Multiplication
```cpp
#include "math/ntt.hpp"
using namespace fhe::math;

uint64_t prime = 1073741827; // NTT-friendly prime
uint64_t root = 5;           // Primitive root
NTT ntt(prime, root);

std::vector<uint64_t> a = {1, 2, 3, 4};
std::vector<uint64_t> b = {5, 6, 7, 8};
std::vector<uint64_t> result = ntt.multiply(a, b);
```

## Performance Considerations

- BigInt uses base-2^64 representation for efficiency
- Polynomial multiplication automatically switches to NTT for large degrees
- Matrix operations are optimized for cache performance
- NTT implementation supports up to 2^20 coefficient polynomials

## Dependencies

- C++17 or later
- Standard library containers and algorithms
- 128-bit integer support for modular arithmetic

## Testing

Comprehensive test suite covering:
- Arithmetic correctness
- Edge cases and boundary conditions
- Performance benchmarks
- Cross-validation with reference implementations