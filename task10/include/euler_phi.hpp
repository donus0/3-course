#pragma once

#include <cmath>
#include <cstdint>

#include "math_util.hpp"

// φ(n) по определению: число единиц в кольце вычетов по модулю n.
inline std::int64_t euler_phi_by_definition(std::int64_t n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    std::int64_t count = 0;
    for (std::int64_t k = 1; k <= n; ++k) {
        if (gcd_int(k, n) == 1) ++count;
    }
    return count;
}

// φ(n) по основной теореме арифметики: n * Π_{p|n} (1 - 1/p).
inline std::int64_t euler_phi_by_factorization(std::int64_t n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    std::int64_t result = n;
    std::int64_t x = n;
    for (std::int64_t p = 2; p * p <= x; ++p) {
        if (x % p == 0) {
            while (x % p == 0) x /= p;
            result -= result / p;
        }
    }
    if (x > 1) result -= result / x;
    return result;
}

// φ(n) через дискретное преобразование: Σ_{k=1}^{n} gcd(k,n) * cos(2πk/n).
inline double euler_phi_by_dft_formula(std::int64_t n) {
    if (n <= 0) return 0.0;
    if (n == 1) return 1.0;
    const double two_pi = 2.0 * std::acos(-1.0);
    double sum = 0.0;
    for (std::int64_t k = 1; k <= n; ++k) {
        sum += static_cast<double>(gcd_int(k, n)) * std::cos(two_pi * static_cast<double>(k) / static_cast<double>(n));
    }
    return sum;
}
