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
