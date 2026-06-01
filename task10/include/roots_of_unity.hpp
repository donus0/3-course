#pragma once

#include <cmath>
#include <complex>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "math_util.hpp"

// Все корни степени n из 1 в ℂ: решения z^n = 1.
inline std::vector<std::complex<double>> roots_of_unity(std::size_t n) {
    if (n == 0) throw std::invalid_argument("roots_of_unity: n must be > 0");
    std::vector<std::complex<double>> roots;
    roots.reserve(n);
    const double two_pi = 2.0 * std::acos(-1.0);
    for (std::size_t k = 0; k < n; ++k) {
        const double angle = two_pi * static_cast<double>(k) / static_cast<double>(n);
        roots.emplace_back(std::cos(angle), std::sin(angle));
    }
    return roots;
}

// Первообразные (примитивные) корни степени n из 1: ω^k, где gcd(k,n)=1.
inline std::vector<std::complex<double>> primitive_roots_of_unity(std::size_t n) {
    if (n == 0) throw std::invalid_argument("primitive_roots_of_unity: n must be > 0");
    const auto all = roots_of_unity(n);
    if (n == 1) return all;

    std::vector<std::complex<double>> primitive;
    primitive.reserve(n);
    for (std::size_t k = 0; k < n; ++k) {
        if (gcd_int(static_cast<std::int64_t>(k), static_cast<std::int64_t>(n)) == 1) {
            primitive.push_back(all[k]);
        }
    }
    return primitive;
}

// Стандартный примитивный корень ω_n = e^{2πi/n}.
inline std::complex<double> primitive_root(std::size_t n) {
    if (n == 0) throw std::invalid_argument("primitive_root: n must be > 0");
    const double two_pi = 2.0 * std::acos(-1.0);
    return {std::cos(two_pi / static_cast<double>(n)), std::sin(two_pi / static_cast<double>(n))};
}
