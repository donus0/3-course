#pragma once

#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "matrix.hpp"
#include "primitive_root_mod.hpp"
#include "vandermonde_mod.hpp"

// Прямое ДПФ над ℝ^n через матрицу Вандермонда (mod n).
inline std::vector<double> dft_forward_mod(const std::vector<double>& x, std::int64_t modulus,
                                           std::int64_t primitive_root) {
    const std::size_t n = x.size();
    if (n == 0) throw std::invalid_argument("dft_forward_mod: vector must be non-empty");
    const RealMatrix v = vandermonde_matrix_mod(n, modulus, primitive_root);
    return mat_vec_mul(v, x);
}

// Обратное ДПФ.
inline std::vector<double> dft_inverse_mod(const std::vector<double>& y, std::int64_t modulus,
                                           std::int64_t primitive_root) {
    const std::size_t n = y.size();
    if (n == 0) throw std::invalid_argument("dft_inverse_mod: vector must be non-empty");
    const RealMatrix inv = vandermonde_inverse_mod(n, modulus, primitive_root);
    return mat_vec_mul(inv, y);
}
