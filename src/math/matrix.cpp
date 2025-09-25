#include "matrix.hpp"
#include <stdexcept>

namespace fhe {
namespace math {

template<typename T>
Matrix<T> Matrix<T>::operator-(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw std::invalid_argument("Matrix dimensions must match for subtraction");
    }

    Matrix result(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result.data[i][j] = data[i][j] - other.data[i][j];
        }
    }

    return result;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(const T& scalar) const {
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result.data[i][j] = data[i][j] * scalar;
        }
    }
    return result;
}

template<typename T>
Matrix<T>& Matrix<T>::operator+=(const Matrix& other) {
    *this = *this + other;
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator-=(const Matrix& other) {
    *this = *this - other;
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator*=(const Matrix& other) {
    *this = *this * other;
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator*=(const T& scalar) {
    *this = *this * scalar;
    return *this;
}

template<typename T>
bool Matrix<T>::operator==(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        return false;
    }

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if (data[i][j] != other.data[i][j]) {
                return false;
            }
        }
    }

    return true;
}

template<typename T>
bool Matrix<T>::operator!=(const Matrix& other) const {
    return !(*this == other);
}

template<typename T>
T Matrix<T>::determinant() const {
    if (!isSquare()) {
        throw std::invalid_argument("Determinant only defined for square matrices");
    }

    if (rows == 1) {
        return data[0][0];
    }

    if (rows == 2) {
        return data[0][0] * data[1][1] - data[0][1] * data[1][0];
    }

    T det = T();
    for (size_t j = 0; j < cols; ++j) {
        Matrix submatrix(rows - 1, cols - 1);
        for (size_t i = 1; i < rows; ++i) {
            size_t colIndex = 0;
            for (size_t k = 0; k < cols; ++k) {
                if (k != j) {
                    submatrix.data[i - 1][colIndex] = data[i][k];
                    colIndex++;
                }
            }
        }

        T cofactor = data[0][j] * submatrix.determinant();
        if (j % 2 == 1) {
            det -= cofactor;
        } else {
            det += cofactor;
        }
    }

    return det;
}

template<typename T>
bool Matrix<T>::isIdentity() const {
    if (!isSquare()) {
        return false;
    }

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if (i == j) {
                if (data[i][j] != T(1)) {
                    return false;
                }
            } else {
                if (data[i][j] != T()) {
                    return false;
                }
            }
        }
    }

    return true;
}

template<typename T>
bool Matrix<T>::isZero() const {
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if (data[i][j] != T()) {
                return false;
            }
        }
    }
    return true;
}

template<typename T>
Matrix<T> Matrix<T>::zero(size_t rows, size_t cols) {
    return Matrix(rows, cols, T());
}

template<typename T>
void Matrix<T>::resize(size_t newRows, size_t newCols, const T& value) {
    data.resize(newRows);
    for (size_t i = 0; i < newRows; ++i) {
        data[i].resize(newCols, value);
    }
    rows = newRows;
    cols = newCols;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix) {
    os << "[\n";
    for (size_t i = 0; i < matrix.rows; ++i) {
        os << "  [";
        for (size_t j = 0; j < matrix.cols; ++j) {
            os << matrix.data[i][j];
            if (j < matrix.cols - 1) os << ", ";
        }
        os << "]";
        if (i < matrix.rows - 1) os << ",";
        os << "\n";
    }
    os << "]";
    return os;
}

template class Matrix<BigInt>;
template class Matrix<uint64_t>;

} // namespace math
} // namespace fhe