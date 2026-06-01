#pragma once

#include <cmath>
#include <complex>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "matrix.hpp"

inline bool is_power_of_two(std::size_t n) { return n > 0 && (n & (n - 1)) == 0; }

// БПФ (Cooley–Tukey, radix-2) над ℝ^n; n — степень двойки.
inline void fft_inplace(std::vector<std::complex<double>>& a, bool inverse) {
    const std::size_t n = a.size();
    if (!is_power_of_two(n)) {
        throw std::invalid_argument("fft_inplace: size must be a power of two");
    }

    for (std::size_t i = 1, j = 0; i < n; ++i) {
        std::size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) std::swap(a[i], a[j]);
    }

    for (std::size_t len = 2; len <= n; len <<= 1) {
        const double ang = (inverse ? 2.0 : -2.0) * std::acos(-1.0) / static_cast<double>(len);
        const std::complex<double> wlen(std::cos(ang), std::sin(ang));
        for (std::size_t i = 0; i < n; i += len) {
            std::complex<double> w(1.0, 0.0);
            for (std::size_t j = 0; j < len / 2; ++j) {
                const std::complex<double> u = a[i + j];
                const std::complex<double> v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }

    if (inverse) {
        for (auto& z : a) z /= static_cast<double>(n);
    }
}

inline std::vector<std::complex<double>> fft_forward(const std::vector<double>& x) {
    if (!is_power_of_two(x.size())) {
        throw std::invalid_argument("fft_forward: size must be a power of two");
    }
    std::vector<std::complex<double>> a(x.begin(), x.end());
    fft_inplace(a, false);
    return a;
}

inline std::vector<double> fft_inverse(const std::vector<std::complex<double>>& y) {
    if (!is_power_of_two(y.size())) {
        throw std::invalid_argument("fft_inverse: size must be a power of two");
    }
    std::vector<std::complex<double>> a = y;
    fft_inplace(a, true);
    std::vector<double> x(a.size());
    for (std::size_t i = 0; i < a.size(); ++i) x[i] = a[i].real();
    return x;
}
