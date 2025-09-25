#include "../math/bigint.hpp"
#include <cassert>
#include <iostream>
#include <string>

using namespace fhe::math;

void testBasicArithmetic() {
    std::cout << "Testing basic arithmetic..." << std::endl;

    BigInt a(123);
    BigInt b(456);

    // Addition
    BigInt sum = a + b;
    assert(sum == BigInt(579));

    // Subtraction
    BigInt diff = b - a;
    assert(diff == BigInt(333));

    // Multiplication
    BigInt prod = a * b;
    assert(prod == BigInt(56088));

    // Division
    BigInt quot = prod / a;
    assert(quot == b);

    // Modulo
    BigInt mod = BigInt(17) % BigInt(5);
    assert(mod == BigInt(2));

    std::cout << "Basic arithmetic tests passed!" << std::endl;
}

void testStringConstruction() {
    std::cout << "Testing string construction..." << std::endl;

    BigInt a("123456789012345678901234567890");
    BigInt b("-987654321098765432109876543210");

    assert(!a.isNegative());
    assert(b.isNegative());
    assert(!a.isZero());

    BigInt zero("0");
    assert(zero.isZero());

    std::cout << "String construction tests passed!" << std::endl;
}

void testModularArithmetic() {
    std::cout << "Testing modular arithmetic..." << std::endl;

    BigInt base(2);
    BigInt exponent(10);
    BigInt modulus(1000);

    BigInt result = base.powMod(exponent, modulus);
    assert(result == BigInt(24)); // 2^10 = 1024, 1024 % 1000 = 24

    // Test modular inverse
    BigInt a(3);
    BigInt m(11);
    BigInt inv = a.modInverse(m);
    BigInt product = (a * inv) % m;
    assert(product == BigInt(1));

    std::cout << "Modular arithmetic tests passed!" << std::endl;
}

void testGCD() {
    std::cout << "Testing GCD..." << std::endl;

    BigInt a(48);
    BigInt b(18);
    BigInt gcd = a.gcd(b);
    assert(gcd == BigInt(6));

    BigInt x(17);
    BigInt y(13);
    BigInt gcd2 = x.gcd(y);
    assert(gcd2 == BigInt(1)); // Coprime numbers

    std::cout << "GCD tests passed!" << std::endl;
}

void testComparison() {
    std::cout << "Testing comparison operators..." << std::endl;

    BigInt a(100);
    BigInt b(200);
    BigInt c(100);

    assert(a < b);
    assert(b > a);
    assert(a <= c);
    assert(a >= c);
    assert(a == c);
    assert(a != b);

    // Negative numbers
    BigInt neg(-50);
    assert(neg < a);
    assert(a > neg);

    std::cout << "Comparison tests passed!" << std::endl;
}

void testShiftOperations() {
    std::cout << "Testing shift operations..." << std::endl;

    BigInt a(8); // Binary: 1000
    BigInt left = a << 2; // Should be 32 (100000)
    assert(left == BigInt(32));

    BigInt right = left >> 3; // Should be 4 (100)
    assert(right == BigInt(4));

    std::cout << "Shift operation tests passed!" << std::endl;
}

void testLargeNumbers() {
    std::cout << "Testing large number operations..." << std::endl;

    BigInt large1("123456789012345678901234567890123456789");
    BigInt large2("987654321098765432109876543210987654321");

    BigInt sum = large1 + large2;
    BigInt diff = large2 - large1;
    BigInt prod = large1 * BigInt(2);

    // Verify operations don't crash with large numbers
    assert(!sum.isZero());
    assert(!diff.isZero());
    assert(prod > large1);

    std::cout << "Large number tests passed!" << std::endl;
}

int main() {
    std::cout << "Starting BigInt tests..." << std::endl;

    testBasicArithmetic();
    testStringConstruction();
    testModularArithmetic();
    testGCD();
    testComparison();
    testShiftOperations();
    testLargeNumbers();

    std::cout << "All BigInt tests passed!" << std::endl;
    return 0;
}