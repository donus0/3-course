#define _USE_MATH_DEFINES
#include "../include/laplace_function.h"
#include <cmath>
#include <stdexcept>

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

// Производная интегральной функции Лапласа: Φ'(x) = (1/√(2π)) * e^(-x²/2)
double laplaceFunctionDerivative(double x) {
	const double sqrt2pi = std::sqrt(2.0 * M_PI);
	return std::exp(-x * x / 2.0) / sqrt2pi;
}

// Поиск аргумента x по значению функции Φ(x) методом деления отрезка пополам
double laplaceInverseBisection(double targetValue, double a, double b, double epsilon, int n, int maxIterations) {
	// Функция для метода бисекции: f(x) = Φ(x) - targetValue
	auto f = [targetValue, n](double x) -> double {
		return laplaceFunction(x, n, true) - targetValue;
	};
	
	// Проверка знаков на концах отрезка
	double fa = f(a);
	double fb = f(b);
	
	if (fa * fb > 0) {
		// Если знаки одинаковые, корня нет на этом отрезке
		throw std::runtime_error("Функция имеет одинаковые знаки на концах отрезка. Выберите другой отрезок.");
	}
	
	int iterations = 0;
	while ((b - a) > epsilon && iterations < maxIterations) {
		double c = (a + b) / 2.0;
		double fc = f(c);
		
		if (std::abs(fc) < epsilon) {
			return c;
		}
		
		if (fa * fc < 0) {
			b = c;
			fb = fc;
		} else {
			a = c;
			fa = fc;
		}
		
		++iterations;
	}
	
	return (a + b) / 2.0;
}

// Поиск аргумента x по значению функции Φ(x) методом Ньютона
double laplaceInverseNewton(double targetValue, double x0, double epsilon, int n, int maxIterations) {
	double x = x0;
	
	for (int i = 0; i < maxIterations; ++i) {
		double fx = laplaceFunction(x, n, true) - targetValue;
		
		if (std::abs(fx) < epsilon) {
			return x;
		}
		
		double fpx = laplaceFunctionDerivative(x);
		
		if (std::abs(fpx) < 1e-15) {
			throw std::runtime_error("Производная слишком мала. Метод Ньютона не может быть применен.");
		}
		
		double xNew = x - fx / fpx;
		
		if (std::abs(xNew - x) < epsilon) {
			return xNew;
		}
		
		x = xNew;
	}
	
	return x;
}

