#pragma once

#include <cstdint>

inline std::int64_t gcd_int(std::int64_t a, std::int64_t b) {
    a = a < 0 ? -a : a;
    b = b < 0 ? -b : b;
    while (b != 0) {
        const std::int64_t t = a % b;
        a = b;
        b = t;
    }
    return a;
}

inline std::int64_t mod_pow(std::int64_t base, std::int64_t exp, std::int64_t mod) {
    if (mod <= 0) return 0;
    base %= mod;
    if (base < 0) base += mod;
    std::int64_t result = 1 % mod;
    while (exp > 0) {
        if (exp & 1) result = (result * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

inline std::int64_t euler_phi(std::int64_t n) {
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
