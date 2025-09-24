# Crypto Module

This module provides cryptographic primitives and utilities supporting the FHE implementations.

## Purpose

The crypto module contains cryptographic building blocks that are used throughout the FHE library, including both standard cryptographic primitives and specialized FHE-related cryptographic operations.

## Key Components (To be implemented)

### Random Number Generation
- `prng.h/.cpp` - Pseudorandom number generators
- `secure_random.h/.cpp` - Cryptographically secure randomness
- `distribution.h/.cpp` - Various probability distributions for sampling

### Hash Functions and PRFs
- `hash.h/.cpp` - SHA-3, BLAKE2, and other hash functions
- `prf.h/.cpp` - Pseudorandom functions for key derivation

### Key Derivation
- `kdf.h/.cpp` - Key derivation functions
- `seed_expansion.h/.cpp` - Seed expansion for key generation

### Error Sampling
- `gaussian_sampling.h/.cpp` - Discrete Gaussian sampling
- `error_distribution.h/.cpp` - Various error distributions for LWE

### Specialized Cryptographic Operations
- `commitment.h/.cpp` - Commitment schemes
- `zero_knowledge.h/.cpp` - Zero-knowledge proof systems (if needed)

### Security Utilities
- `constant_time.h/.cpp` - Constant-time operation utilities
- `secure_memory.h/.cpp` - Secure memory handling and zeroization

## Security Features

- **Side-channel Resistance**: Implementations avoid timing and cache-based side channels
- **Secure Randomness**: Uses system entropy sources and cryptographic PRNGs
- **Memory Safety**: Secure memory allocation and deallocation practices
- **Parameter Validation**: Rigorous validation of cryptographic parameters

## Standards Compliance

Where applicable, implementations follow established cryptographic standards and best practices from NIST, IETF, and the academic cryptographic community.