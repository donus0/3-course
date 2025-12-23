#ifndef LAPLACE_FUNCTION_H
#define LAPLACE_FUNCTION_H

// Подынтегральная функция для интегральной функции Лапласа: e^(-t²/2)
double laplaceIntegrand(double t);

// Метод прямоугольников для вычисления интеграла
double rectangleMethod(double (*f)(double), double a, double b, int n);

// Метод трапеций для вычисления интеграла
double trapezoidalMethod(double (*f)(double), double a, double b, int n);

// Вычисление интегральной функции Лапласа: Φ(x) = (1/√(2π)) * ∫[0 to x] e^(-t²/2) dt
double laplaceFunction(double x, int n, bool useTrapezoidal);

// Производная интегральной функции Лапласа: Φ'(x) = (1/√(2π)) * e^(-x²/2)
double laplaceFunctionDerivative(double x);

// Поиск аргумента x по значению функции Φ(x) методом деления отрезка пополам
// targetValue - значение функции, которое нужно найти
// a, b - границы начального отрезка
// epsilon - требуемая точность
// n - количество разбиений для численного интегрирования
// maxIterations - максимальное количество итераций
double laplaceInverseBisection(double targetValue, double a, double b, double epsilon, int n, int maxIterations = 1000);

// Поиск аргумента x по значению функции Φ(x) методом Ньютона
// targetValue - значение функции, которое нужно найти
// x0 - начальное приближение
// epsilon - требуемая точность
// n - количество разбиений для численного интегрирования
// maxIterations - максимальное количество итераций
double laplaceInverseNewton(double targetValue, double x0, double epsilon, int n, int maxIterations = 1000);

#endif

