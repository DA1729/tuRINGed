# Utils Module

The utils module provides general-purpose utilities and helper functions for the FHE library.

## Components

### Serialization
- Binary serialization for all FHE objects
- JSON export for debugging and analysis
- Cross-platform compatibility
- Compression support for large objects

### Memory Management
- Custom allocators for sensitive data
- Secure memory wiping
- Memory pool management for performance
- RAII wrappers for automatic cleanup

### Threading and Parallelization
- Thread pool for parallel operations
- OpenMP integration where available
- Lock-free data structures for hot paths
- NUMA-aware memory allocation

### Logging and Debugging
- Structured logging with multiple levels
- Performance profiling utilities
- Debug assertions and validation
- Timing and benchmarking tools

### Configuration Management
- Parameter file parsing
- Environment variable integration
- Runtime configuration validation
- Default parameter sets for common use cases

### Error Handling
- Comprehensive error codes and messages
- Exception safety guarantees
- Error recovery mechanisms
- Debug information preservation

## Planned Features

### Hardware Acceleration
- AVX2/AVX-512 optimized routines
- GPU acceleration hooks (CUDA/OpenCL)
- ARM NEON optimizations
- Specialized instruction detection

### Networking
- Secure parameter exchange protocols
- Distributed computation support
- Multi-party computation primitives
- Network serialization formats

## Usage Examples

```cpp
#include "utils/serialization.hpp"
#include "utils/logging.hpp"

// Serialization
Ciphertext ct = ...;
std::vector<uint8_t> data = serialize(ct);
Ciphertext restored = deserialize<Ciphertext>(data);

// Logging
LOG_INFO("Starting homomorphic multiplication");
LOG_DEBUG("Noise level: {} bits", estimateNoise(ct));
```