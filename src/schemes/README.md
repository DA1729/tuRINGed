# Schemes Module

This module implements various Fully Homomorphic Encryption (FHE) schemes.

## Purpose

The schemes module contains implementations of different FHE schemes, each with their own security assumptions, efficiency characteristics, and use cases.

## Supported Schemes (To be implemented)

### Learning With Errors (LWE) Based Schemes
- **BGV**: Brakerski-Gentry-Vaikuntanathan scheme
- **BFV**: Brakerski/Fan-Vercauteren scheme
- **CKKS**: Cheon-Kim-Kim-Song scheme for approximate arithmetic
- **FHEW**: Fast Homomorphic Encryption over the integers using LWE
- **TFHE**: Torus-based FHE with very fast bootstrapping

### Ring Learning With Errors (RLWE) Based Schemes
- **Ring-LWE variants**: Polynomial ring versions of LWE schemes

## Key Components (To be implemented)

- `bgv.h/.cpp` - BGV scheme implementation
- `bfv.h/.cpp` - BFV scheme implementation
- `ckks.h/.cpp` - CKKS scheme for floating-point operations
- `fhew.h/.cpp` - FHEW implementation
- `tfhe.h/.cpp` - TFHE implementation
- `base_scheme.h` - Abstract base class for all FHE schemes

## Features

Each scheme implementation will provide:
- Key generation (public key, secret key, evaluation key)
- Encryption and decryption
- Homomorphic operations (addition, multiplication)
- Bootstrapping (where applicable)
- Parameter selection utilities

## Security Considerations

All implementations follow current security best practices and parameter recommendations from the FHE community.