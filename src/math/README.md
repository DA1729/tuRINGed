# Math Module

This module contains mathematical primitives and operations essential for Fully Homomorphic Encryption (FHE).

## Purpose

The math module provides fundamental mathematical components including:

- **Polynomial Arithmetic**: Operations on polynomials in various rings, particularly Z[X]/(X^n + 1)
- **Modular Arithmetic**: Efficient modular operations for large integers
- **Number Theory**: Prime generation, discrete logarithms, and other number-theoretic functions
- **Linear Algebra**: Matrix operations over finite fields and rings
- **Fast Transforms**: Number Theoretic Transform (NTT) for efficient polynomial multiplication
- **Random Number Generation**: Cryptographically secure random sampling from various distributions

## Key Components (To be implemented)

- `polynomial.h/.cpp` - Polynomial ring operations
- `modular.h/.cpp` - Modular arithmetic utilities
- `ntt.h/.cpp` - Number Theoretic Transform implementation
- `sampling.h/.cpp` - Random sampling from discrete Gaussian and uniform distributions
- `matrix.h/.cpp` - Matrix operations for key switching and bootstrapping
- `field.h/.cpp` - Finite field arithmetic

## Dependencies

This module will use optimized arithmetic libraries and may interface with hardware acceleration when available.

## Notes

Mathematical operations in this module are designed to be constant-time where cryptographically relevant to prevent side-channel attacks.