#include <ostream>
#include <algorithm>

#ifdef __INTEL_COMPILER
#include <mkl.h>
#else
extern "C"
{
#define lapack_complex_float float _Complex
#define lapack_complex_double double _Complex
#include <lapacke.h>
#include <cblas.h>
}
#endif

#include "Matrix.h"
#include "Exception.h"

Matrix::Matrix(Size m, Size n)
{
    resize(m, n);
}

void Matrix::resize(Size m, Size n)
{
    m_ = m;
    n_ = n;
    isSquare_ = m_ == n_;
    std::vector<Scalar>::resize(m_ * n_, 0.);
    ipiv_.resize(m_);
}

void Matrix::zero()
{
    std::fill(begin(), end(), 0.);
}

void Matrix::init(const Scalar *begin, const Scalar *end)
{
    std::vector<Scalar>::assign(begin, end);
}

Scalar &Matrix::operator()(Size i, Size j)
{
    return std::vector<Scalar>::operator[](i * n_ + j);
}

Scalar Matrix::operator()(Size i, Size j) const
{
    return std::vector<Scalar>::operator[](i * n_ + j);
}

Matrix &Matrix::operator=(const std::initializer_list<Scalar> &list)
{
    if (list.size() != m_ * n_)
        throw Exception("Matrix", "operator=", "initializer list size does not match matrix dimensions.");

    assign(list);
    return *this;
}

//- Operators
Matrix &Matrix::operator+=(const Matrix &rhs)
{
    if (m_ != rhs.m_ || n_ != rhs.n_)
        throw Exception("Matrix", "operator+=", "dimension mismatch.");

    std::transform(begin(), end(), rhs.begin(), begin(), std::plus<Scalar>());

    return *this;
}

Matrix &Matrix::operator-=(const Matrix &rhs)
{
    if (m_ != rhs.m_ || n_ != rhs.n_)
        throw Exception("Matrix", "operator-=", "dimension mismatch.");

    std::transform(begin(), end(), rhs.begin(), begin(), std::minus<Scalar>());

    return *this;
}

Matrix &Matrix::operator*=(Scalar rhs)
{
    std::transform(begin(), end(), begin(), [rhs](Scalar a){
        return a * rhs;
    });

    return *this;
}

Matrix &Matrix::operator/=(Scalar rhs)
{
    std::transform(begin(), end(), begin(), [rhs](Scalar a){
        return a / rhs;
    });

    return *this;
}

Matrix &Matrix::solve(Matrix &b)
{
    if (isSquare_)
        LAPACKE_dgesv(LAPACK_ROW_MAJOR, m_, b.n_, data(), n_, ipiv_.data(), b.data(), b.n_);
    else
    {
        LAPACKE_dgels(LAPACK_ROW_MAJOR, 'N', m_, n_, b.n_, data(), n_, b.data(), b.m_);
        b.m_ = n_; // A bit hackish, but resizes b apropriately. Only works so long as the data format is row major
        b.isSquare_ = b.m_ == n_;
    }

    return b;
}

Matrix &Matrix::transpose()
{
    if (isSquare_) // Square matrices
    {
        auto &self = *this;

        for (size_t m = 0; m < m_; ++m)
            for (size_t n = m + 1; n < n_; ++n)
                std::swap(self(m, n), self(n, m));
    }
    else // General non-square matrices
    {
        auto last = end();
        auto first = begin();
        int m = n_;

        const int mn1 = (last - first - 1);
        const int n = (last - first) / m;
        std::vector<bool> visited(last - first);
        auto cycle = first;
        while (++cycle != last)
        {
            if (visited[cycle - first])
                continue;
            int a = cycle - first;
            do
            {
                a = a == mn1 ? mn1 : (n * a) % mn1;
                std::swap(*(first + a), *cycle);
                visited[a] = true;
            } while ((first + a) != cycle);
        }

        std::swap(m_, n_);
    }

    return *this;
}

Matrix &Matrix::invert()
{
    lapack_int info1 = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, m_, n_, data(), n_, ipiv_.data());
    lapack_int info2 = LAPACKE_dgetri(LAPACK_ROW_MAJOR, m_, data(), n_, ipiv_.data());

    if (info1 != 0 || info2 != 0)
        throw Exception("Matrix", "invert", "inversion failed, matrix is singular to working precision.");

    return *this;
}

Scalar Matrix::norm(char type) const
{
    return LAPACKE_dlange(LAPACK_ROW_MAJOR, type, m_, n_, data(), n_);
}

Scalar Matrix::cond(char type) const
{
    Scalar rcond;
    LAPACKE_dgecon(LAPACK_ROW_MAJOR, type, m_, data(), n_, norm(type), &rcond);
    return rcond;
}

Matrix Matrix::subMatrix(size_t startRow, size_t startCol, size_t endRow, size_t endCol) const
{
    Matrix subMat(endRow - startRow, endCol - startCol);
    auto &self = *this;

    for (size_t m = startRow; m < endRow; ++m)
        for (size_t n = startCol; n < endCol; ++n)
            subMat(m - startRow, n - startCol) = self(m, n);

    return subMat;
}

//- External functions
Matrix eye(size_t nRows, size_t nCols)
{
    Matrix mat(nRows, nCols);
    for (size_t i = 0; i < nRows && i < nCols; ++i)
        mat(i, i) = 1.;

    return mat;
}

Matrix random(size_t nRows, size_t nCols, Scalar min, Scalar max)
{
    Matrix mat(nRows, nCols);

    for (size_t m = 0; m < nRows; ++m)
        for (size_t n = 0; n < nCols; ++n)
            mat(m, n) = (double) rand() / RAND_MAX * (max - min) + min;

    return mat;
}

Matrix transpose(Matrix mat)
{
    return mat.transpose();
}

Matrix inverse(Matrix mat)
{
    return mat.invert();
}

Matrix solve(Matrix A, Matrix b)
{
    return A.solve(b);
}

Matrix operator+(Matrix lhs, const Matrix &rhs)
{
    return lhs += rhs;
}

Matrix operator-(Matrix lhs, const Matrix &rhs)
{
    return lhs -= rhs;
}

Matrix operator*(Matrix lhs, Scalar rhs)
{
    return lhs *= rhs;
}

Matrix operator*(Scalar lhs, Matrix rhs)
{
    return rhs *= lhs;
}

Matrix operator*(const Matrix &A, const Matrix &B)
{
    Matrix C(A.m(), B.n());

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, A.m(), B.n(), A.n(), 1., A.data(), A.n(),
                B.data(), B.n(), 1., C.data(), C.n());

    // Works for sure
    //    const int nI = A.nRows(), nJ = B.nCols(), nK = A.nCols();

    //    for(int i = 0; i < nI; ++i)
    //        for(int j = 0; j < nJ; ++j)
    //            for(int k = 0; k < nK; ++k)
    //                C(i, j) += A(i, k)*B(k, j);

    return C;
}

Matrix operator/(Matrix lhs, Scalar rhs)
{
    return lhs /= rhs;
}

std::ostream &operator<<(std::ostream &os, const Matrix &mat)
{
    for (Size i = 0, m = mat.m(); i < m; ++i)
    {
        for (Size j = 0, n = mat.n(); j < n; ++j)
        {
            os << mat(i, j) << ' ';
        }
        os << '\n';
    }

    return os;
}
