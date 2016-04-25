#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

#include "Types.h"

class Matrix : private std::vector<Scalar>
{
public:

    Matrix(size_t nRows = 0, size_t nCols = 0);

    void resize(size_t nRows, size_t nCols);
    void zero();

    size_t nRows() const { return nRows_; }
    size_t nCols() const { return nCols_; }
    bool isSquare() const { return isSquare_; }

    Scalar& operator()(size_t m, size_t n);
    const Scalar& operator()(size_t m, size_t n) const;

    Matrix& operator+=(const Matrix& rhs);
    Matrix& operator-=(const Matrix& rhs);
    Matrix& operator*=(Scalar rhs);
    Matrix& operator/=(Scalar rhs);

    Matrix& solve(Matrix& b);

    Matrix& transpose();
    Matrix& invert();

    Matrix subMatrix(size_t startRow, size_t startCol, size_t endRow, size_t endCol) const;

    Scalar* data() { return std::vector<Scalar>::data(); }
    const Scalar* data() const { return std::vector<Scalar>::data(); }

private:

    size_t nRows_, nCols_;
    bool isSquare_;
    mutable std::vector<int> ipiv_;

};

//- External functions
Matrix eye(size_t nRows, size_t nCols);
Matrix random(size_t nRows, size_t nCols, Scalar min, Scalar max);
Matrix transpose(Matrix mat);
Matrix inverse(Matrix mat);
Matrix solve(Matrix A, Matrix b);

Matrix operator+(Matrix lhs, const Matrix& rhs);
Matrix operator-(Matrix lhs, const Matrix& rhs);
Matrix operator*(Matrix lhs, Scalar rhs);
Matrix operator*(Scalar lhs, Matrix rhs);
Matrix operator*(const Matrix& lhs, const Matrix& rhs);
Matrix operator/(Matrix lhs, Scalar rhs);

std::ostream& operator<<(std::ostream& os, const Matrix& mat);

#endif
