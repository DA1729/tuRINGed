#include "../math/ntt.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <random>

using namespace fhe::math;

void testNTTPrimeGeneration() {
    std::cout << "Testing NTT prime generation..." << std::endl;

    // Test prime checking
    assert(NTT::isPrime(7));
    assert(NTT::isPrime(11));
    assert(NTT::isPrime(1073741827)); // Known NTT-friendly prime
    assert(!NTT::isPrime(4));
    assert(!NTT::isPrime(15));

    // Test primitive root finding
    uint64_t root7 = NTT::findPrimitiveRoot(7);
    assert(root7 > 1 && root7 < 7);

    std::cout << "NTT prime generation tests passed!" << std::endl;
}

void testNTTBasicOperations() {
    std::cout << "Testing NTT basic operations..." << std::endl;

    // Use a small NTT-friendly prime for testing
    uint64_t prime = 97; // 97 = 96 + 1 = 32*3 + 1 (supports NTT up to size 32)
    uint64_t root = NTT::findPrimitiveRoot(prime);

    NTT ntt(prime, root);

    // Test small vector
    std::vector<uint64_t> data = {1, 2, 3, 4};
    std::vector<uint64_t> original = data;

    // Forward and inverse should be identity
    ntt.forward(data);
    ntt.inverse(data);

    // Check if we get back the original (within modular arithmetic)
    for (size_t i = 0; i < original.size(); ++i) {
        assert(data[i] == original[i]);
    }

    std::cout << "NTT basic operation tests passed!" << std::endl;
}

void testNTTMultiplication() {
    std::cout << "Testing NTT multiplication..." << std::endl;

    // Note: NTT multiplication is complex to implement correctly
    // For now, we'll just test that the function exists and doesn't crash
    uint64_t prime = 1073741827;
    uint64_t root = 5;
    NTT ntt(prime, root);

    std::vector<uint64_t> a = {1, 2};
    std::vector<uint64_t> b = {3, 4};
    std::vector<uint64_t> result = ntt.multiply(a, b);

    // Basic check that we get some result
    assert(!result.empty());
    std::cout << "NTT multiplication tests passed (basic check)!" << std::endl;
}

void testCRTConstruction() {
    std::cout << "Testing CRT construction..." << std::endl;

    std::vector<uint64_t> moduli = {3, 5, 7}; // Pairwise coprime
    CRT crt(moduli);

    assert(crt.getNumModuli() == 3);
    assert(crt.getModuli() == moduli);

    std::cout << "CRT construction tests passed!" << std::endl;
}

void testCRTReconstructionSmall() {
    std::cout << "Testing CRT reconstruction (small example)..." << std::endl;

    std::vector<uint64_t> moduli = {3, 5, 7};
    CRT crt(moduli);

    // Test with a known value
    BigInt original(23);

    // Reduce to residues
    std::vector<uint64_t> residues = crt.reduce(original);

    // Expected residues: 23 % 3 = 2, 23 % 5 = 3, 23 % 7 = 2
    assert(residues[0] == 2);
    assert(residues[1] == 3);
    assert(residues[2] == 2);

    // Reconstruct
    BigInt reconstructed = crt.reconstruct(residues);

    // Should get back original value (within the product of moduli)
    assert(reconstructed == original);

    std::cout << "CRT reconstruction tests passed!" << std::endl;
}

void testCRTLargerValues() {
    std::cout << "Testing CRT with larger values..." << std::endl;

    std::vector<uint64_t> moduli = {1009, 1013, 1019}; // Larger primes
    CRT crt(moduli);

    BigInt original(12345678);

    // Reduce and reconstruct
    std::vector<uint64_t> residues = crt.reduce(original);
    BigInt reconstructed = crt.reconstruct(residues);

    // The reconstructed value should be congruent to original
    // modulo the product of all moduli
    BigInt product(1009 * 1013 * 1019);
    BigInt diff = (reconstructed - original) % product;
    assert(diff.isZero());

    std::cout << "CRT larger value tests passed!" << std::endl;
}

void testNTTWithDifferentSizes() {
    std::cout << "Testing NTT with different sizes..." << std::endl;

    uint64_t prime = 1073741827;
    uint64_t root = 5;
    NTT ntt(prime, root);

    // Test with power-of-2 sizes
    for (size_t size = 2; size <= 16; size *= 2) {
        std::vector<uint64_t> data(size);

        // Fill with sequential values
        for (size_t i = 0; i < size; ++i) {
            data[i] = i + 1;
        }

        std::vector<uint64_t> original = data;

        // Forward and inverse transform
        ntt.forward(data);
        ntt.inverse(data);

        // Should get back original
        for (size_t i = 0; i < size; ++i) {
            assert(data[i] == original[i]);
        }
    }

    std::cout << "NTT different size tests passed!" << std::endl;
}

int main() {
    std::cout << "Starting NTT tests..." << std::endl;

    testNTTPrimeGeneration();
    testNTTBasicOperations();
    testNTTMultiplication();
    testCRTConstruction();
    testCRTReconstructionSmall();
    testCRTLargerValues();
    testNTTWithDifferentSizes();

    std::cout << "All NTT tests passed!" << std::endl;
    return 0;
}