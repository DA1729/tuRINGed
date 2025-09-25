# Schemes Module

The schemes module contains implementations of specific FHE schemes and protocols.

## Supported Schemes

### BGV (Brakerski-Gentry-Vaikuntanathan)
- Ring-LWE based construction
- Supports modulus switching
- Optimized for arithmetic circuits
- Batching support via Chinese Remainder Theorem

### BFV (Brakerski/Fan-Vercauteren)
- Scale-invariant operations
- Efficient plaintext space utilization
- Optimized for integer arithmetic
- SIMD operations support

### CKKS (Cheon-Kim-Kim-Song)
- Approximate arithmetic over complex numbers
- Native support for floating-point operations
- Rescaling for noise management
- Optimized for machine learning applications

### GSW (Gentry-Sahai-Waters)
- Matrix-based construction
- Supports arbitrary branching programs
- Asymmetric noise growth
- Useful for boolean circuits

## Planned Features

### TFHE (Torus-based FHE)
- Fast bootstrapping
- Gate-by-gate evaluation
- Low-latency operations
- Boolean circuit optimization

### FHEW
- Bootstrapping in ~0.1 seconds
- Boolean gate evaluation
- Compact ciphertexts
- Minimal memory footprint

## Architecture

Each scheme implementation includes:
- Parameter generation and validation
- Key generation (secret, public, evaluation keys)
- Encryption and decryption operations
- Homomorphic operation implementations
- Bootstrapping procedures (where applicable)
- Serialization and I/O operations

## Usage Guidelines

1. Choose scheme based on application requirements:
   - **BGV/BFV**: Integer arithmetic, exact computation
   - **CKKS**: Floating-point, approximate computation
   - **GSW**: Boolean circuits, branching programs
   - **TFHE**: Low-latency boolean operations

2. Select security parameters appropriate for threat model
3. Configure batching and packing for SIMD parallelism
4. Plan bootstrapping strategy for deep computations