#pragma once

#include <complex>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "matrix.hpp"
#include "roots_of_unity.hpp"

// Матрица Вандермонда V[i][j] = ω^{ij}, ω — примитивный корень степени n из 1.
inline ComplexMatrix vandermonde_matrix(std::size_t n, std::complex<double> omega) {
    if (n == 0) throw std::invalid_argument("vandermonde_matrix: n must be > 0");
    ComplexMatrix v(n, std::vector<std::complex<double>>(n));
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            v[i][j] = std::pow(omega, static_cast<double>(i * j));
        }
    }
    return v;
}

inline ComplexMatrix vandermonde_matrix(std::size_t n) {
    return vandermonde_matrix(n, primitive_root(n));
}

// Обратная матрица DFT: (1/n) * ω^{-ij} = (1/n) * conj(ω^{ij}).
inline ComplexMatrix vandermonde_inverse(std::size_t n, std::complex<double> omega) {
    if (n == 0) throw std::invalid_argument("vandermonde_inverse: n must be > 0");
    const std::complex<double> inv_n = 1.0 / static_cast<double>(n);
    ComplexMatrix inv(n, std::vector<std::complex<double>>(n));
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            inv[i][j] = std::conj(std::pow(omega, static_cast<double>(i * j))) * inv_n;
        }
    }
    return inv;
}

inline ComplexMatrix vandermonde_inverse(std::size_t n) {
    return vandermonde_inverse(n, primitive_root(n));
}
