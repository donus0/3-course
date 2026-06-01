#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "math_util.hpp"
#include "matrix.hpp"

// V[i][j] = g^{ij} mod m, g — первообразный корень по модулю m.
inline RealMatrix vandermonde_matrix_mod(std::size_t n, std::int64_t modulus, std::int64_t g) {
    if (n == 0) throw std::invalid_argument("vandermonde_matrix_mod: n must be > 0");
    if (modulus < 2) throw std::invalid_argument("vandermonde_matrix_mod: modulus must be >= 2");

    RealMatrix v(n, std::vector<double>(n));
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            v[i][j] = static_cast<double>(mod_pow(g, static_cast<std::int64_t>(i * j), modulus));
        }
    }
    return v;
}

// Обратная через степени g^{-ij} mod m (как в задании), с нормировкой 1/n.
inline RealMatrix vandermonde_inverse_mod_formula(std::size_t n, std::int64_t modulus, std::int64_t g) {
    if (n == 0) throw std::invalid_argument("vandermonde_inverse_mod_formula: n must be > 0");
    if (modulus < 2) throw std::invalid_argument("vandermonde_inverse_mod_formula: modulus must be >= 2");

    const std::int64_t phi = euler_phi(modulus);
    const double inv_n = 1.0 / static_cast<double>(n);
    RealMatrix inv(n, std::vector<double>(n));

    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            const std::int64_t ij = static_cast<std::int64_t>(i * j);
            std::int64_t neg = (phi - (ij % phi)) % phi;
            if (neg < 0) neg += phi;
            const std::int64_t residue = (ij % phi == 0) ? 1 : mod_pow(g, neg, modulus);
            inv[i][j] = inv_n * static_cast<double>(residue);
        }
    }
    return inv;
}

// Обратная как линейный оператор на ℝ^n (численно, если формула даёт плохую обратимость).
inline RealMatrix vandermonde_inverse_mod(std::size_t n, std::int64_t modulus, std::int64_t g) {
    const RealMatrix v = vandermonde_matrix_mod(n, modulus, g);
    try {
        return invert_matrix(v);
    } catch (const std::runtime_error&) {
        return vandermonde_inverse_mod_formula(n, modulus, g);
    }
}
