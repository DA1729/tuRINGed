#include "../math/polynomial.hpp"
#include <cassert>
#include <iostream>
#include <vector>

using namespace fhe::math;

void testPolynomialConstruction() {
    std::cout << "Testing polynomial construction..." << std::endl;

    // Default constructor
    PolynomialZZ p1;
    assert(p1.isZero());

    // Vector constructor
    std::vector<BigInt> coeffs = {BigInt(1), BigInt(2), BigInt(3)};
    PolynomialZZ p2(coeffs);
    assert(p2.degree() == 2);
    assert(p2[0] == BigInt(1));
    assert(p2[1] == BigInt(2));
    assert(p2[2] == BigInt(3));

    // Degree constructor
    PolynomialZZ p3(3, BigInt(5));
    assert(p3.degree() == 3);
    assert(p3[0] == BigInt(5));
    assert(p3[3] == BigInt(5));

    std::cout << "Polynomial construction tests passed!" << std::endl;
}

void testPolynomialArithmetic() {
    std::cout << "Testing polynomial arithmetic..." << std::endl;

    // Create polynomials: p1 = 1 + 2x, p2 = 3 + 4x
    PolynomialZZ p1({BigInt(1), BigInt(2)});
    PolynomialZZ p2({BigInt(3), BigInt(4)});

    // Addition: (1 + 2x) + (3 + 4x) = 4 + 6x
    PolynomialZZ sum = p1 + p2;
    assert(sum[0] == BigInt(4));
    assert(sum[1] == BigInt(6));

    // Subtraction: (3 + 4x) - (1 + 2x) = 2 + 2x
    PolynomialZZ diff = p2 - p1;
    assert(diff[0] == BigInt(2));
    assert(diff[1] == BigInt(2));

    // Multiplication: (1 + 2x) * (3 + 4x) = 3 + 10x + 8x^2
    PolynomialZZ prod = p1 * p2;
    assert(prod.degree() == 2);
    assert(prod[0] == BigInt(3));
    assert(prod[1] == BigInt(10));
    assert(prod[2] == BigInt(8));

    std::cout << "Polynomial arithmetic tests passed!" << std::endl;
}

void testPolynomialEvaluation() {
    std::cout << "Testing polynomial evaluation..." << std::endl;

    // Create polynomial: p = 1 + 2x + 3x^2
    PolynomialZZ p({BigInt(1), BigInt(2), BigInt(3)});

    // Evaluate at x = 2: 1 + 2*2 + 3*2^2 = 1 + 4 + 12 = 17
    BigInt result = p.evaluate(BigInt(2));
    assert(result == BigInt(17));

    // Evaluate at x = 0: should be constant term
    BigInt result0 = p.evaluate(BigInt(0));
    assert(result0 == BigInt(1));

    std::cout << "Polynomial evaluation tests passed!" << std::endl;
}

void testPolynomialModulo() {
    std::cout << "Testing polynomial modulo..." << std::endl;

    // Create dividend: x^3 + x^2 + x + 1
    PolynomialZZ dividend({BigInt(1), BigInt(1), BigInt(1), BigInt(1)});

    // Create divisor: x^2 + 1
    PolynomialZZ divisor({BigInt(1), BigInt(0), BigInt(1)});

    // Compute remainder
    PolynomialZZ remainder = dividend % divisor;

    // The remainder should have degree less than divisor degree (which is 2)
    assert(remainder.degree() < 2);

    std::cout << "Polynomial modulo tests passed!" << std::endl;
}

void testCyclotomicPolynomial() {
    std::cout << "Testing cyclotomic polynomial generation..." << std::endl;

    // Generate first cyclotomic polynomial: Φ₁(x) = x - 1
    PolynomialZZ phi1 = generateCyclotomicPolynomialZZ(1);
    assert(phi1.degree() == 1);
    assert(phi1[0] == BigInt(-1));
    assert(phi1[1] == BigInt(1));

    // Generate 4th cyclotomic polynomial: Φ₄(x) = x⁴ - x³ + x² - x + 1
    PolynomialZZ phi4 = generateCyclotomicPolynomialZZ(4);
    assert(phi4.degree() == 4);
    assert(phi4[0] == BigInt(-1));
    assert(phi4[4] == BigInt(1));

    std::cout << "Cyclotomic polynomial tests passed!" << std::endl;
}

void testPolynomialDerivative() {
    std::cout << "Testing polynomial derivative..." << std::endl;

    // Create polynomial: p = 1 + 2x + 3x^2 + 4x^3
    PolynomialZZ p({BigInt(1), BigInt(2), BigInt(3), BigInt(4)});

    // Derivative: p' = 2 + 6x + 12x^2
    PolynomialZZ derivative = p.derivative();
    assert(derivative.degree() == 2);
    assert(derivative[0] == BigInt(2));
    assert(derivative[1] == BigInt(6));
    assert(derivative[2] == BigInt(12));

    // Derivative of constant should be zero
    PolynomialZZ constant({BigInt(42)});
    PolynomialZZ constDeriv = constant.derivative();
    assert(constDeriv.isZero());

    std::cout << "Polynomial derivative tests passed!" << std::endl;
}

void testPolynomialComparison() {
    std::cout << "Testing polynomial comparison..." << std::endl;

    PolynomialZZ p1({BigInt(1), BigInt(2), BigInt(3)});
    PolynomialZZ p2({BigInt(1), BigInt(2), BigInt(3)});
    PolynomialZZ p3({BigInt(1), BigInt(2), BigInt(4)});

    assert(p1 == p2);
    assert(p1 != p3);

    std::cout << "Polynomial comparison tests passed!" << std::endl;
}

int main() {
    std::cout << "Starting Polynomial tests..." << std::endl;

    testPolynomialConstruction();
    testPolynomialArithmetic();
    testPolynomialEvaluation();
    testPolynomialModulo();
    testCyclotomicPolynomial();
    testPolynomialDerivative();
    testPolynomialComparison();

    std::cout << "All Polynomial tests passed!" << std::endl;
    return 0;
}