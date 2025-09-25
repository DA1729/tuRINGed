#include "../math/matrix.hpp"
#include <cassert>
#include <iostream>

using namespace fhe::math;

void testMatrixConstruction() {
    std::cout << "Testing matrix construction..." << std::endl;

    // Default constructor
    MatrixZZ m1;
    assert(m1.getRows() == 0);
    assert(m1.getCols() == 0);

    // Size constructor
    MatrixZZ m2(3, 4, BigInt(5));
    assert(m2.getRows() == 3);
    assert(m2.getCols() == 4);
    assert(m2(0, 0) == BigInt(5));
    assert(m2(2, 3) == BigInt(5));

    // Vector constructor
    std::vector<std::vector<BigInt>> data = {
        {BigInt(1), BigInt(2)},
        {BigInt(3), BigInt(4)}
    };
    MatrixZZ m3(data);
    assert(m3.getRows() == 2);
    assert(m3.getCols() == 2);
    assert(m3(0, 0) == BigInt(1));
    assert(m3(1, 1) == BigInt(4));

    std::cout << "Matrix construction tests passed!" << std::endl;
}

void testMatrixArithmetic() {
    std::cout << "Testing matrix arithmetic..." << std::endl;

    MatrixZZ A(2, 2);
    A(0, 0) = BigInt(1); A(0, 1) = BigInt(2);
    A(1, 0) = BigInt(3); A(1, 1) = BigInt(4);

    MatrixZZ B(2, 2);
    B(0, 0) = BigInt(5); B(0, 1) = BigInt(6);
    B(1, 0) = BigInt(7); B(1, 1) = BigInt(8);

    // Addition
    MatrixZZ C = A + B;
    assert(C(0, 0) == BigInt(6));
    assert(C(0, 1) == BigInt(8));
    assert(C(1, 0) == BigInt(10));
    assert(C(1, 1) == BigInt(12));

    // Subtraction
    MatrixZZ D = B - A;
    assert(D(0, 0) == BigInt(4));
    assert(D(0, 1) == BigInt(4));
    assert(D(1, 0) == BigInt(4));
    assert(D(1, 1) == BigInt(4));

    // Scalar multiplication
    MatrixZZ E = A * BigInt(2);
    assert(E(0, 0) == BigInt(2));
    assert(E(0, 1) == BigInt(4));
    assert(E(1, 0) == BigInt(6));
    assert(E(1, 1) == BigInt(8));

    std::cout << "Matrix arithmetic tests passed!" << std::endl;
}

void testMatrixMultiplication() {
    std::cout << "Testing matrix multiplication..." << std::endl;

    MatrixZZ A(2, 3);
    A(0, 0) = BigInt(1); A(0, 1) = BigInt(2); A(0, 2) = BigInt(3);
    A(1, 0) = BigInt(4); A(1, 1) = BigInt(5); A(1, 2) = BigInt(6);

    MatrixZZ B(3, 2);
    B(0, 0) = BigInt(7); B(0, 1) = BigInt(8);
    B(1, 0) = BigInt(9); B(1, 1) = BigInt(10);
    B(2, 0) = BigInt(11); B(2, 1) = BigInt(12);

    MatrixZZ C = A * B;
    assert(C.getRows() == 2);
    assert(C.getCols() == 2);

    // Verify specific elements
    // C(0,0) = 1*7 + 2*9 + 3*11 = 7 + 18 + 33 = 58
    assert(C(0, 0) == BigInt(58));
    // C(0,1) = 1*8 + 2*10 + 3*12 = 8 + 20 + 36 = 64
    assert(C(0, 1) == BigInt(64));

    std::cout << "Matrix multiplication tests passed!" << std::endl;
}

void testMatrixTranspose() {
    std::cout << "Testing matrix transpose..." << std::endl;

    MatrixZZ A(2, 3);
    A(0, 0) = BigInt(1); A(0, 1) = BigInt(2); A(0, 2) = BigInt(3);
    A(1, 0) = BigInt(4); A(1, 1) = BigInt(5); A(1, 2) = BigInt(6);

    MatrixZZ At = A.transpose();
    assert(At.getRows() == 3);
    assert(At.getCols() == 2);
    assert(At(0, 0) == BigInt(1));
    assert(At(1, 0) == BigInt(2));
    assert(At(2, 0) == BigInt(3));
    assert(At(0, 1) == BigInt(4));
    assert(At(1, 1) == BigInt(5));
    assert(At(2, 1) == BigInt(6));

    std::cout << "Matrix transpose tests passed!" << std::endl;
}

void testIdentityMatrix() {
    std::cout << "Testing identity matrix..." << std::endl;

    MatrixZZ I = MatrixZZ::identity(3);
    assert(I.getRows() == 3);
    assert(I.getCols() == 3);
    assert(I.isSquare());
    assert(I.isIdentity());

    // Check diagonal elements
    for (size_t i = 0; i < 3; ++i) {
        assert(I(i, i) == BigInt(1));
        for (size_t j = 0; j < 3; ++j) {
            if (i != j) {
                assert(I(i, j) == BigInt(0));
            }
        }
    }

    std::cout << "Identity matrix tests passed!" << std::endl;
}

void testMatrixDeterminant() {
    std::cout << "Testing matrix determinant..." << std::endl;

    // 1x1 matrix
    MatrixZZ M1(1, 1);
    M1(0, 0) = BigInt(5);
    assert(M1.determinant() == BigInt(5));

    // 2x2 matrix
    MatrixZZ M2(2, 2);
    M2(0, 0) = BigInt(1); M2(0, 1) = BigInt(2);
    M2(1, 0) = BigInt(3); M2(1, 1) = BigInt(4);
    // det = 1*4 - 2*3 = 4 - 6 = -2
    assert(M2.determinant() == BigInt(-2));

    // Identity matrix should have determinant 1
    MatrixZZ I = MatrixZZ::identity(3);
    assert(I.determinant() == BigInt(1));

    std::cout << "Matrix determinant tests passed!" << std::endl;
}

void testMatrixComparison() {
    std::cout << "Testing matrix comparison..." << std::endl;

    MatrixZZ A(2, 2, BigInt(1));
    MatrixZZ B(2, 2, BigInt(1));
    MatrixZZ C(2, 2, BigInt(2));

    assert(A == B);
    assert(A != C);

    // Different dimensions
    MatrixZZ D(3, 3, BigInt(1));
    assert(A != D);

    std::cout << "Matrix comparison tests passed!" << std::endl;
}

void testZeroMatrix() {
    std::cout << "Testing zero matrix..." << std::endl;

    MatrixZZ Z = MatrixZZ::zero(2, 3);
    assert(Z.getRows() == 2);
    assert(Z.getCols() == 3);
    assert(Z.isZero());

    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            assert(Z(i, j) == BigInt(0));
        }
    }

    std::cout << "Zero matrix tests passed!" << std::endl;
}

int main() {
    std::cout << "Starting Matrix tests..." << std::endl;

    testMatrixConstruction();
    testMatrixArithmetic();
    testMatrixMultiplication();
    testMatrixTranspose();
    testIdentityMatrix();
    testMatrixDeterminant();
    testMatrixComparison();
    testZeroMatrix();

    std::cout << "All Matrix tests passed!" << std::endl;
    return 0;
}