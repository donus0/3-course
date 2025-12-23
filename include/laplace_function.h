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

#endif

