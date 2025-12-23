#ifndef SAMPLE_GENERATOR_H
#define SAMPLE_GENERATOR_H

#include <vector>
#include <map>
#include <string>

// Структура для представления выборки: значение и количество его вхождений
struct Sample {
	std::vector<std::pair<double, int>> data; // (значение, количество)
	
	// Преобразование в строку для вывода
	std::string toString() const;
	
	// Получение всех уникальных значений
	std::vector<double> getValues() const;
	
	// Получение частот (количеств)
	std::vector<int> getFrequencies() const;
	
	// Общий размер выборки
	int getTotalSize() const;
};

// Генерация выборки нормального распределения
// size - размер выборки
// mean - математическое ожидание
// stddev - среднеквадратическое отклонение
// seed - начальное значение для генератора случайных чисел (0 = случайное)
Sample generateNormalSample(int size, double mean = 0.0, double stddev = 1.0, unsigned int seed = 0);

// Генерация выборки пуассоновского распределения
// size - размер выборки
// lambda - параметр распределения (λ > 0)
// seed - начальное значение для генератора случайных чисел (0 = случайное)
Sample generatePoissonSample(int size, double lambda, unsigned int seed = 0);

// Генерация выборки геометрического распределения
// size - размер выборки
// p - вероятность успеха (0 < p <= 1)
// seed - начальное значение для генератора случайных чисел (0 = случайное)
Sample generateGeometricSample(int size, double p, unsigned int seed = 0);

// Генерация выборки гипергеометрического распределения
// size - размер выборки
// N - размер генеральной совокупности
// K - количество успешных элементов в генеральной совокупности
// n - размер выборки из генеральной совокупности
// seed - начальное значение для генератора случайных чисел (0 = случайное)
Sample generateHypergeometricSample(int size, int N, int K, int n, unsigned int seed = 0);

// Генерация выборки двойного пуассоновского распределения
// size - размер выборки
// lambda1, lambda2 - параметры двух пуассоновских распределений
// p - вероятность выбора первого распределения (0 <= p <= 1)
// seed - начальное значение для генератора случайных чисел (0 = случайное)
Sample generateDoublePoissonSample(int size, double lambda1, double lambda2, double p, unsigned int seed = 0);

// Генерация выборки двойного геометрического распределения
// size - размер выборки
// p1, p2 - вероятности успеха для двух геометрических распределений
// q - вероятность выбора первого распределения (0 <= q <= 1)
// seed - начальное значение для генератора случайных чисел (0 = случайное)
Sample generateDoubleGeometricSample(int size, double p1, double p2, double q, unsigned int seed = 0);

#endif

