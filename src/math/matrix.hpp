#pragma once

#include "bigint.hpp"
#include <vector>
#include <iostream>

namespace fhe {
namespace math {

template<typename T>
class Matrix {
private:
    std::vector<std::vector<T>> data;
    size_t rows, cols;

public:
    Matrix();
    Matrix(size_t rows, size_t cols, const T& value = T());
    Matrix(const std::vector<std::vector<T>>& data);

    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }

    T& operator()(size_t row, size_t col);
    const T& operator()(size_t row, size_t col) const;

    std::vector<T>& operator[](size_t row);
    const std::vector<T>& operator[](size_t row) const;

    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix operator*(const T& scalar) const;

    Matrix& operator+=(const Matrix& other);
    Matrix& operator-=(const Matrix& other);
    Matrix& operator*=(const Matrix& other);
    Matrix& operator*=(const T& scalar);

    bool operator==(const Matrix& other) const;
    bool operator!=(const Matrix& other) const;

    Matrix transpose() const;
    T determinant() const;
    Matrix inverse() const;

    bool isSquare() const;
    bool isIdentity() const;
    bool isZero() const;

    static Matrix identity(size_t size);
    static Matrix zero(size_t rows, size_t cols);

    void resize(size_t newRows, size_t newCols, const T& value = T());

    template<typename U>
    friend std::ostream& operator<<(std::ostream& os, const Matrix<U>& matrix);
};

using MatrixZZ = Matrix<BigInt>;
using MatrixMod = Matrix<uint64_t>;

template<typename T>
Matrix<T>::Matrix() : rows(0), cols(0) {}

template<typename T>
Matrix<T>::Matrix(size_t rows, size_t cols, const T& value)
    : data(rows, std::vector<T>(cols, value)), rows(rows), cols(cols) {}

template<typename T>
Matrix<T>::Matrix(const std::vector<std::vector<T>>& data)
    : data(data), rows(data.size()), cols(data.empty() ? 0 : data[0].size()) {}

template<typename T>
T& Matrix<T>::operator()(size_t row, size_t col) {
    if (row >= rows || col >= cols) {
        throw std::out_of_range("Matrix index out of bounds");
    }
    return data[row][col];
}

template<typename T>
const T& Matrix<T>::operator()(size_t row, size_t col) const {
    if (row >= rows || col >= cols) {
        throw std::out_of_range("Matrix index out of bounds");
    }
    return data[row][col];
}

template<typename T>
std::vector<T>& Matrix<T>::operator[](size_t row) {
    if (row >= rows) {
        throw std::out_of_range("Matrix row index out of bounds");
    }
    return data[row];
}

template<typename T>
const std::vector<T>& Matrix<T>::operator[](size_t row) const {
    if (row >= rows) {
        throw std::out_of_range("Matrix row index out of bounds");
    }
    return data[row];
}

template<typename T>
Matrix<T> Matrix<T>::operator+(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw std::invalid_argument("Matrix dimensions must match for addition");
    }

    Matrix result(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result.data[i][j] = data[i][j] + other.data[i][j];
        }
    }

    return result;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(const Matrix& other) const {
    if (cols != other.rows) {
        throw std::invalid_argument("Invalid matrix dimensions for multiplication");
    }

    Matrix result(rows, other.cols, T());
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < other.cols; ++j) {
            for (size_t k = 0; k < cols; ++k) {
                result.data[i][j] += data[i][k] * other.data[k][j];
            }
        }
    }

    return result;
}

template<typename T>
Matrix<T> Matrix<T>::transpose() const {
    Matrix result(cols, rows);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result.data[j][i] = data[i][j];
        }
    }
    return result;
}

template<typename T>
bool Matrix<T>::isSquare() const {
    return rows == cols;
}

template<typename T>
Matrix<T> Matrix<T>::identity(size_t size) {
    Matrix result(size, size, T());
    for (size_t i = 0; i < size; ++i) {
        result.data[i][i] = T(1);
    }
    return result;
}

extern template class Matrix<BigInt>;
extern template class Matrix<uint64_t>;

} // namespace math
} // namespace fhe