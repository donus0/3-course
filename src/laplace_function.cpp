#define _USE_MATH_DEFINES
#include "../include/laplace_function.h"
#include <cmath>

// Подынтегральная функция для интегральной функции Лапласа: e^(-t²/2)
double laplaceIntegrand(double t) {
	return std::exp(-t * t / 2.0);
}

// Метод прямоугольников для вычисления интеграла
double rectangleMethod(double (*f)(double), double a, double b, int n) {
	if (n <= 0) return 0.0;
	double h = (b - a) / n;
	double sum = 0.0;
	
	// Метод средних прямоугольников (более точный)
	for (int i = 0; i < n; ++i) {
		double x = a + (i + 0.5) * h;
		sum += f(x);
	}
	
	return sum * h;
}

// Метод трапеций для вычисления интеграла
double trapezoidalMethod(double (*f)(double), double a, double b, int n) {
	if (n <= 0) return 0.0;
	double h = (b - a) / n;
	double sum = (f(a) + f(b)) / 2.0;
	
	for (int i = 1; i < n; ++i) {
		double x = a + i * h;
		sum += f(x);
	}
	
	return sum * h;
}

// Вычисление интегральной функции Лапласа: Φ(x) = (1/√(2π)) * ∫[0 to x] e^(-t²/2) dt
double laplaceFunction(double x, int n, bool useTrapezoidal) {
	if (x == 0.0) return 0.0;
	
	const double sqrt2pi = std::sqrt(2.0 * M_PI);
	double integral = 0.0;
	
	if (useTrapezoidal) {
		integral = trapezoidalMethod(laplaceIntegrand, 0.0, x, n);
	} else {
		integral = rectangleMethod(laplaceIntegrand, 0.0, x, n);
	}
	
	return integral / sqrt2pi;
}

