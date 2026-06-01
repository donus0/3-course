#include <cmath>
#include <complex>
#include <iomanip>
#include <iostream>
#include <vector>

#include "dft.hpp"
#include "fft.hpp"
#include "matrix.hpp"
#include "primitive_root_mod.hpp"
#include "vandermonde_mod.hpp"

static void print_vector(const std::vector<double>& v, const char* label) {
    std::cout << label;
    for (double x : v) std::cout << " " << std::fixed << std::setprecision(4) << x;
    std::cout << "\n";
}

int main() {
    // Пример 1: все первообразные корни по модулю 11.
    const std::int64_t modulus = 11;
    const auto roots = all_primitive_roots_mod(modulus);
    std::cout << "primitive roots mod " << modulus << " (count=" << roots.size()
              << ", phi=" << euler_phi(modulus) << "): ";
    for (std::int64_t g : roots) std::cout << g << " ";
    std::cout << "\n";

    const std::int64_t g = find_primitive_root_mod(modulus);
    std::cout << "chosen generator g = " << g << "\n";

    // Пример 2: матрица Вандермонда 5x5 mod 11 и обратная (n = φ(11) + 1 для невырожденности в ℝ).
    const std::size_t dim = 5;
    const auto v = vandermonde_matrix_mod(dim, modulus, g);
    const auto inv = vandermonde_inverse_mod(dim, modulus, g);
    std::cout << "Vandermonde_mod[0][*]: ";
    for (std::size_t j = 0; j < dim; ++j) std::cout << v[0][j] << " ";
    std::cout << "\nInverse_mod[0][*]:     ";
    for (std::size_t j = 0; j < dim; ++j) std::cout << inv[0][j] << " ";
    std::cout << "\n";

    const auto prod = mat_mul(v, inv);
    std::cout << "(V * V^-1)[0][0] = " << prod[0][0] << "\n";

    // Пример 3: прямое и обратное ДПФ (mod 11).
    const std::vector<double> signal = {1.0, 2.0, 3.0, 4.0, 5.0};
    const auto spectrum = dft_forward_mod(signal, modulus, g);
    const auto restored = dft_inverse_mod(spectrum, modulus, g);
    print_vector(signal, "DFT mod: original:      ");
    print_vector(spectrum, "DFT mod: forward:       ");
    print_vector(restored, "DFT mod: after inverse: ");

    // Пример 4: БПФ над ℝ^8 (степень двойки; сигнал дополняем нулями).
    const std::vector<double> signal8 = {1.0, 2.0, 3.0, 4.0, 5.0, 0.0, 0.0, 0.0};
    const auto fft_spec = fft_forward(signal8);
    const auto fft_restored = fft_inverse(fft_spec);

    std::cout << "FFT forward (complex): ";
    for (const auto& c : fft_spec) {
        std::cout << std::fixed << std::setprecision(4) << c.real();
        if (c.imag() >= 0) {
            std::cout << "+" << c.imag() << "i ";
        } else {
            std::cout << c.imag() << "i ";
        }
    }
    std::cout << "\n";
    print_vector(fft_restored, "FFT after inverse:      ");
    print_vector(signal8, "FFT original (padded):  ");

    // Пример 5: для n=12 первообразных корней нет.
    std::cout << "has_primitive_roots_mod(12) = "
              << (has_primitive_roots_mod(12) ? "true" : "false") << "\n";

    return 0;
}
