# Turinged

Work-in-progress C++17 implementation of Fully Homomorphic Encryption schemes for learning purposes.

## Implemented Schemes

- LWE, RLWE, GLWE, GLev, and GGSW
- Basic homomorphic operations (addition, subtraction, scalar multiplication)
- Negacyclic polynomial arithmetic

## Library Structure

```
turinged/
├── include/turinged/          # Public headers
│   ├── core/                  # Core types and utilities
│   ├── polynomial/            # Polynomial operations
│   ├── keys/                  # Key management
│   ├── schemes/               # Cryptographic schemes
│   └── operations/            # Homomorphic operations
├── src/turinged/              # Implementation files
├── examples/                  # Usage examples
└── tests/                     # Test programs
```

## Schemes Implemented

### LWE (Learning With Errors)
- Basic lattice-based encryption over vectors
- Support for homomorphic addition and scalar multiplication

### RLWE (Ring Learning With Errors)
- Polynomial-based variant of LWE
- More efficient for batch operations

### GLWE (Generalized Learning With Errors)
- Multi-polynomial extension of RLWE
- Foundation for more advanced schemes

### GLev (Leveled GLWE)
- Multi-level GLWE ciphertexts
- Support for decomposition-based operations

### GGSW (GSW over polynomials)
- Advanced scheme supporting homomorphic multiplication
- Built on top of GLev ciphertexts

## Building

Requires C++17 compiler and CMake 3.16+.

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Usage

```cpp
#include "turinged/turinged.hpp"
using namespace turinged;

Parameters params(0, 1LL << 30, 16, 1000);
auto sk = keys::generate_lwe_secret_key(256);

auto ct = schemes::encrypt_lwe(5, sk, params);
int64 result = schemes::decrypt_lwe(ct, sk, params);
```

Examples available in `./examples/` directory.

## Status

Incomplete implementation. Missing key features like bootstrapping, NTT-based multiplication, and proper parameter selection.

## Disclaimer

Educational implementation for learning FHE concepts. Not production-ready. Use established libraries for real applications.

## License

MIT
