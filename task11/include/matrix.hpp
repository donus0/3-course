#pragma once

#include <cmath>
#include <complex>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

using RealMatrix = std::vector<std::vector<double>>;
using ComplexMatrix = std::vector<std::vector<std::complex<double>>>;

inline RealMatrix mat_mul(const RealMatrix& a, const RealMatrix& b) {
    const std::size_t n = a.size();
    if (n == 0 || b.size() != n || b[0].size() != n) {
        throw std::invalid_argument("mat_mul: matrices must be square and same size");
    }
    RealMatrix c(n, std::vector<double>(n, 0.0));
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t k = 0; k < n; ++k) {
            for (std::size_t j = 0; j < n; ++j) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return c;
}

inline ComplexMatrix mat_mul(const ComplexMatrix& a, const ComplexMatrix& b) {
    const std::size_t n = a.size();
    if (n == 0 || b.size() != n || b[0].size() != n) {
        throw std::invalid_argument("mat_mul: matrices must be square and same size");
    }
    ComplexMatrix c(n, std::vector<std::complex<double>>(n, {0.0, 0.0}));
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t k = 0; k < n; ++k) {
            for (std::size_t j = 0; j < n; ++j) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return c;
}

inline std::vector<double> mat_vec_mul(const RealMatrix& a, const std::vector<double>& x) {
    const std::size_t n = a.size();
    if (n == 0 || x.size() != n) throw std::invalid_argument("mat_vec_mul: dimension mismatch");
    std::vector<double> y(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            y[i] += a[i][j] * x[j];
        }
    }
    return y;
}

inline std::vector<std::complex<double>> mat_vec_mul(const ComplexMatrix& a,
                                                     const std::vector<double>& x) {
    const std::size_t n = a.size();
    if (n == 0 || x.size() != n) throw std::invalid_argument("mat_vec_mul: dimension mismatch");
    std::vector<std::complex<double>> y(n, {0.0, 0.0});
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            y[i] += a[i][j] * static_cast<double>(x[j]);
        }
    }
    return y;
}

inline std::vector<std::complex<double>> mat_vec_mul(const ComplexMatrix& a,
                                                     const std::vector<std::complex<double>>& x) {
    const std::size_t n = a.size();
    if (n == 0 || x.size() != n) throw std::invalid_argument("mat_vec_mul: dimension mismatch");
    std::vector<std::complex<double>> y(n, {0.0, 0.0});
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            y[i] += a[i][j] * x[j];
        }
    }
    return y;
}

// Обращение невырожденной вещественной матрицы (Гаусс–Жордан).
inline RealMatrix invert_matrix(const RealMatrix& a, double eps = 1e-9) {
    const std::size_t n = a.size();
    if (n == 0) throw std::invalid_argument("invert_matrix: empty matrix");

    RealMatrix aug(n, std::vector<double>(2 * n, 0.0));
    for (std::size_t i = 0; i < n; ++i) {
        if (a[i].size() != n) throw std::invalid_argument("invert_matrix: matrix must be square");
        for (std::size_t j = 0; j < n; ++j) aug[i][j] = a[i][j];
        aug[i][n + i] = 1.0;
    }

    for (std::size_t col = 0; col < n; ++col) {
        std::size_t pivot = col;
        for (std::size_t row = col + 1; row < n; ++row) {
            if (std::abs(aug[row][col]) > std::abs(aug[pivot][col])) pivot = row;
        }
        if (std::abs(aug[pivot][col]) < eps) {
            throw std::runtime_error("invert_matrix: matrix is singular");
        }
        if (pivot != col) std::swap(aug[pivot], aug[col]);

        const double div = aug[col][col];
        for (std::size_t j = 0; j < 2 * n; ++j) aug[col][j] /= div;

        for (std::size_t row = 0; row < n; ++row) {
            if (row == col) continue;
            const double factor = aug[row][col];
            for (std::size_t j = 0; j < 2 * n; ++j) {
                aug[row][j] -= factor * aug[col][j];
            }
        }
    }

    RealMatrix inv(n, std::vector<double>(n, 0.0));
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) inv[i][j] = aug[i][n + j];
    }
    return inv;
}
