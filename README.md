# Turinged

A modern C++17 library for Fully Homomorphic Encryption (FHE) schemes, implementing various lattice-based cryptographic primitives.

## Features

- **Multiple FHE Schemes**: LWE, RLWE, GLWE, GLev, and GGSW
- **Polynomial Operations**: Optimized negacyclic polynomial arithmetic
- **Homomorphic Operations**: Addition, subtraction, scalar multiplication
- **Modular Design**: Clean separation between core utilities, schemes, and operations
- **Modern C++**: Written in C++17 with clear, maintainable code

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

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 6+, MSVC 2017+)
- CMake 3.16+

### Build Instructions

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Build Options

- `BUILD_EXAMPLES=ON/OFF` - Build example programs (default: ON)
- `BUILD_TESTS=ON/OFF` - Build test programs (default: ON)
- `BUILD_SHARED_LIBS=ON/OFF` - Build shared library (default: OFF)

## Usage

### Basic LWE Example

```cpp
#include "turinged/turinged.hpp"
using namespace turinged;

// Set parameters
Parameters params(0, 1LL << 30, 16, 1000);

// Generate keys
auto sk = keys::generate_lwe_secret_key(256);

// Encrypt and decrypt
auto ct = schemes::encrypt_lwe(5, sk, params);
int64 result = schemes::decrypt_lwe(ct, sk, params);
```

### Homomorphic Operations

```cpp
// Encrypt two values
auto ct1 = schemes::encrypt_lwe(3, sk, params);
auto ct2 = schemes::encrypt_lwe(5, sk, params);

// Homomorphic addition
auto ct_sum = operations::add_lwe(ct1, ct2, params);
int64 sum = schemes::decrypt_lwe(ct_sum, sk, params); // Result: 8
```

## Examples

Run the included examples:

```bash
# LWE basic encryption/decryption
./examples/lwe_example

# RLWE polynomial operations
./examples/rlwe_example

# Homomorphic operations
./examples/homomorphic_ops_example
```

## Parameter Guidelines

### Security Considerations
- Use appropriate modulus sizes for your security level
- Noise bounds should be carefully chosen to prevent decryption failures
- These are toy implementations - use established parameter sets for production

### Performance vs Security
- Larger polynomial degrees (n) provide better security but slower operations
- Smaller modulus (q) improves performance but may limit noise tolerance

## Roadmap

### Planned Features
- [ ] Advanced key switching operations
- [ ] Bootstrapping procedures
- [ ] NTT-based polynomial multiplication
- [ ] Parameter selection utilities
- [ ] Security analysis tools
- [ ] GPU acceleration support

### Schemes to Add
- [ ] TFHE-style schemes
- [ ] BFV/BGV schemes
- [ ] CKKS for approximate arithmetic

## Contributing

This library is designed to be educational and extensible. Contributions are welcome for:

- Performance optimizations
- Additional FHE schemes
- Better parameter selection
- Documentation improvements
- Test coverage expansion

## License

MIT

## References

This implementation follows the constructions described in various FHE literature. Key references include:

- Brakerski, Z., & Vaikuntanathan, V. (2011). Efficient fully homomorphic encryption from (standard) LWE
- Gentry, C., Sahai, A., & Waters, B. (2013). Homomorphic encryption from learning with errors
- Chillotti, I., et al. (2016). Faster fully homomorphic encryption: Bootstrapping in less than 0.1 seconds

## Disclaimer

This is an educational/research implementation. For production use, consider established libraries like:
- Microsoft SEAL
- IBM HElib
- PALISADE/OpenFHE
- TFHE-rs
