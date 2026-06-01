#pragma once

#include <cmath>
#include <complex>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "matrix.hpp"
#include "vandermonde.hpp"

// Прямое ДПФ: y = V * x, x ∈ ℝ^n.
inline std::vector<std::complex<double>> dft_forward(const std::vector<double>& x) {
    const std::size_t n = x.size();
    if (n == 0) throw std::invalid_argument("dft_forward: vector must be non-empty");
    const ComplexMatrix v = vandermonde_matrix(n);
    return mat_vec_mul(v, x);
}

inline std::vector<std::complex<double>> dft_forward(const std::vector<double>& x,
                                                    const ComplexMatrix& vandermonde) {
    if (x.size() != vandermonde.size()) {
        throw std::invalid_argument("dft_forward: dimension mismatch");
    }
    return mat_vec_mul(vandermonde, x);
}

// Обратное ДПФ: x = Re(V^{-1} * y).
inline std::vector<double> dft_inverse(const std::vector<std::complex<double>>& y) {
    const std::size_t n = y.size();
    if (n == 0) throw std::invalid_argument("dft_inverse: vector must be non-empty");
    const ComplexMatrix inv = vandermonde_inverse(n);
    const auto x_complex = mat_vec_mul(inv, y);
    std::vector<double> x(n);
    for (std::size_t i = 0; i < n; ++i) x[i] = x_complex[i].real();
    return x;
}

inline std::vector<double> dft_inverse(const std::vector<std::complex<double>>& y,
                                       const ComplexMatrix& inverse_vandermonde) {
    if (y.size() != inverse_vandermonde.size()) {
        throw std::invalid_argument("dft_inverse: dimension mismatch");
    }
    const auto x_complex = mat_vec_mul(inverse_vandermonde, y);
    std::vector<double> x(y.size());
    for (std::size_t i = 0; i < y.size(); ++i) x[i] = x_complex[i].real();
    return x;
}
