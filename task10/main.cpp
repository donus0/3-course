#include <cmath>
#include <complex>
#include <iomanip>
#include <iostream>
#include <vector>

#include "dft.hpp"
#include "euler_phi.hpp"
#include "roots_of_unity.hpp"
#include "vandermonde.hpp"

static void print_complex(const std::complex<double>& z) {
    std::cout << std::fixed << std::setprecision(4) << z.real();
    if (z.imag() >= 0) {
        std::cout << "+" << z.imag() << "i";
    } else {
        std::cout << z.imag() << "i";
    }
}

static void print_vector(const std::vector<double>& v, const char* label) {
    std::cout << label;
    for (double x : v) std::cout << " " << x;
    std::cout << "\n";
}

int main() {
    // Пример 1: φ(12) тремя способами.
    const std::int64_t n = 12;
    const auto phi_def = euler_phi_by_definition(n);
    const auto phi_fact = euler_phi_by_factorization(n);
    const auto phi_dft = euler_phi_by_dft_formula(n);
    std::cout << "phi(" << n << ") by definition      = " << phi_def << "\n";
    std::cout << "phi(" << n << ") by factorization   = " << phi_fact << "\n";
    std::cout << "phi(" << n << ") by DFT formula     = " << std::llround(phi_dft) << "\n";

    // Пример 2: все корни 4-й степени из 1.
    std::cout << "4th roots of unity:\n";
    for (const auto& z : roots_of_unity(4)) {
        print_complex(z);
        std::cout << "  ";
    }
    std::cout << "\n";

    // Пример 3: примитивные корни 8-й степени из 1.
    std::cout << "primitive 8th roots of unity:\n";
    for (const auto& z : primitive_roots_of_unity(8)) {
        print_complex(z);
        std::cout << "  ";
    }
    std::cout << "\n";

    // Пример 4: матрица Вандермонда 4x4 и обратная (фрагмент).
    const std::size_t dim = 4;
    const auto v = vandermonde_matrix(dim);
    const auto inv = vandermonde_inverse(dim);
    std::cout << "Vandermonde[0][*]: ";
    for (std::size_t j = 0; j < dim; ++j) {
        print_complex(v[0][j]);
        std::cout << "  ";
    }
    std::cout << "\nInverse[0][*]:     ";
    for (std::size_t j = 0; j < dim; ++j) {
        print_complex(inv[0][j]);
        std::cout << "  ";
    }
    std::cout << "\n";

    // Проверка V * V^{-1} ≈ I (первый диагональный элемент).
    const auto prod = mat_mul(v, inv);
    std::cout << "(V * V^-1)[0][0] = ";
    print_complex(prod[0][0]);
    std::cout << "\n";

    // Пример 5: прямое и обратное ДПФ над ℝ^4.
    const std::vector<double> signal = {1.0, 2.0, 3.0, 4.0};
    const auto spectrum = dft_forward(signal);
    const auto restored = dft_inverse(spectrum);

    std::cout << "DFT forward (complex): ";
    for (const auto& c : spectrum) {
        print_complex(c);
        std::cout << "  ";
    }
    std::cout << "\n";
    print_vector(signal, "original: ");
    print_vector(restored, "after inverse DFT: ");

    return 0;
}
