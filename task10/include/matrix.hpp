#pragma once

#include <complex>
#include <cstddef>
#include <stdexcept>
#include <vector>

using ComplexMatrix = std::vector<std::vector<std::complex<double>>>;

inline ComplexMatrix mat_mul(const ComplexMatrix& a, const ComplexMatrix& b) {
    const std::size_t n = a.size();
    if (n == 0 || b.size() != n || b[0].size() != n) {
        throw std::invalid_argument("mat_mul: matrices must be square and same size");
    }
    for (const auto& row : a) {
        if (row.size() != n) throw std::invalid_argument("mat_mul: matrices must be square and same size");
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

inline std::vector<std::complex<double>> mat_vec_mul(const ComplexMatrix& a,
                                                     const std::vector<double>& x) {
    std::vector<std::complex<double>> xc(x.begin(), x.end());
    return mat_vec_mul(a, xc);
}
