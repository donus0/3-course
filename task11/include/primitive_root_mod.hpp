#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "math_util.hpp"

// Существуют ли первообразные корни по модулю n: n ∈ {2,4,p^k,2p^k}, p — нечётное простое.
inline bool has_primitive_roots_mod(std::int64_t n) {
    if (n < 2) return false;
    if (n == 2 || n == 4) return true;

    int exp2 = 0;
    while (n % 2 == 0) {
        n /= 2;
        ++exp2;
    }
    if (n == 1) return false; // n = 2^k, k >= 3

    int odd_prime_factors = 0;
    for (std::int64_t p = 3; p * p <= n; p += 2) {
        if (n % p == 0) {
            ++odd_prime_factors;
            while (n % p == 0) n /= p;
        }
    }
    if (n > 1) ++odd_prime_factors;
    if (odd_prime_factors != 1) return false;

    return exp2 == 0 || exp2 == 1;
}

inline void prime_divisors(std::int64_t m, std::vector<std::int64_t>& out) {
    out.clear();
    for (std::int64_t p = 2; p * p <= m; ++p) {
        if (m % p == 0) {
            out.push_back(p);
            while (m % p == 0) m /= p;
        }
    }
    if (m > 1) out.push_back(m);
}

// Указание 2: g — первообразный корень ⇔ g^φ(n) ≡ 1 и g^{φ(n)/p} ≢ 1 для всех p|φ(n).
inline bool is_primitive_root_mod(std::int64_t g, std::int64_t n) {
    if (n < 2) return false;
    if (gcd_int(g, n) != 1) return false;
    const std::int64_t phi = euler_phi(n);
    if (mod_pow(g, phi, n) != 1) return false;

    std::vector<std::int64_t> primes;
    prime_divisors(phi, primes);
    for (std::int64_t p : primes) {
        if (mod_pow(g, phi / p, n) == 1) return false;
    }
    return true;
}

inline std::int64_t find_primitive_root_mod(std::int64_t n) {
    if (!has_primitive_roots_mod(n)) {
        throw std::runtime_error("find_primitive_root_mod: primitive roots do not exist for this n");
    }
    for (std::int64_t g = 2; g < n; ++g) {
        if (is_primitive_root_mod(g, n)) return g;
    }
    return 1; // n = 2
}

// Указание 3: все первообразные корни — g^j mod n, gcd(j, φ(n)) = 1.
inline std::vector<std::int64_t> all_primitive_roots_mod(std::int64_t n) {
    if (!has_primitive_roots_mod(n)) return {};
    const std::int64_t g = find_primitive_root_mod(n);
    const std::int64_t phi = euler_phi(n);

    std::vector<std::int64_t> roots;
    roots.reserve(static_cast<std::size_t>(phi));
    for (std::int64_t j = 1; j <= phi; ++j) {
        if (gcd_int(j, phi) == 1) roots.push_back(mod_pow(g, j, n));
    }
    return roots;
}
