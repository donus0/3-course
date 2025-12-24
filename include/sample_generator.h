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

// Загрузка выборки из текстового файла
// Формат файла: каждая строка содержит "значение количество" (разделенные пробелом или табуляцией)
// Пустые строки и строки, начинающиеся с #, игнорируются
// filename - путь к файлу
// requireUniqueValues - требовать уникальность значений xi (по умолчанию true)
// requireNonNegativeValues - требовать неотрицательность значений xi (по умолчанию false)
// requireNonNegativeCounts - требовать неотрицательность количеств xj (по умолчанию true)
Sample loadSampleFromFile(const std::string& filename, 
                          bool requireUniqueValues = true,
                          bool requireNonNegativeValues = false,
                          bool requireNonNegativeCounts = true);

// Статистические оценки для выборки

// Выборочное среднее (несмещённая оценка математического ожидания)
// x̄ = (1/n) * Σ(xi * ni), где n - общий размер выборки
double sampleMean(const Sample& sample);

// Выборочная дисперсия (смещённая оценка дисперсии)
// D = (1/n) * Σ(ni * (xi - x̄)²)
double sampleVariance(const Sample& sample);

// Исправленная дисперсия (несмещённая оценка дисперсии)
// s² = (1/(n-1)) * Σ(ni * (xi - x̄)²) = (n/(n-1)) * D
double correctedVariance(const Sample& sample);

// Выборочное среднеквадратическое отклонение (на основе выборочной дисперсии)
double sampleStandardDeviation(const Sample& sample);

// Исправленное среднеквадратическое отклонение (на основе исправленной дисперсии)
double correctedStandardDeviation(const Sample& sample);

// Структура для результата проверки гипотезы
struct HypothesisTestResult {
	double chiSquare;        // Статистика хи-квадрат
	int degreesOfFreedom;   // Число степеней свободы
	double pValue;          // p-value
	bool hypothesisAccepted; // Принята ли гипотеза (при уровне значимости 0.05)
	double significanceLevel; // Уровень значимости
	
	std::string toString() const;
};

// Проверка гипотезы о нормальном распределении
// sample - выборка для проверки
// significanceLevel - уровень значимости (по умолчанию 0.05)
HypothesisTestResult testNormalDistribution(const Sample& sample, double significanceLevel = 0.05);

// Проверка гипотезы о пуассоновском распределении
HypothesisTestResult testPoissonDistribution(const Sample& sample, double significanceLevel = 0.05);

// Проверка гипотезы о геометрическом распределении
HypothesisTestResult testGeometricDistribution(const Sample& sample, double significanceLevel = 0.05);

// Проверка гипотезы о гипергеометрическом распределении
// N, K, n - параметры распределения (должны быть заданы)
HypothesisTestResult testHypergeometricDistribution(const Sample& sample, int N, int K, int n, double significanceLevel = 0.05);

// Проверка гипотезы о двойном пуассоновском распределении
// lambda1, lambda2, p - параметры распределения (должны быть заданы)
HypothesisTestResult testDoublePoissonDistribution(const Sample& sample, double lambda1, double lambda2, double p, double significanceLevel = 0.05);

// Проверка гипотезы о двойном геометрическом распределении
// p1, p2, q - параметры распределения (должны быть заданы)
HypothesisTestResult testDoubleGeometricDistribution(const Sample& sample, double p1, double p2, double q, double significanceLevel = 0.05);

// ============================================================================
// Задание 9: Оценка параметров распределений по выборке
// ============================================================================

// Структура для хранения оцененных параметров нормального распределения
struct NormalDistributionParams {
	double mean;      // μ - математическое ожидание
	double stddev;    // σ - среднеквадратическое отклонение
	
	std::string toString() const;
};

// Структура для хранения оцененных параметров пуассоновского распределения
struct PoissonDistributionParams {
	double lambda;   // λ - параметр распределения
	
	std::string toString() const;
};

// Структура для хранения оцененных параметров геометрического распределения
struct GeometricDistributionParams {
	double p;        // p - вероятность успеха
	
	std::string toString() const;
};

// Структура для хранения оцененных параметров гипергеометрического распределения
struct HypergeometricDistributionParams {
	int N;           // Размер генеральной совокупности
	int K;           // Количество успешных элементов
	int n;           // Размер выборки
	
	std::string toString() const;
};

// Структура для хранения оцененных параметров двойного пуассоновского распределения
struct DoublePoissonDistributionParams {
	double lambda1;  // λ1 - параметр первого распределения
	double lambda2;  // λ2 - параметр второго распределения
	double p;        // p - вероятность выбора первого распределения
	
	std::string toString() const;
};

// Структура для хранения оцененных параметров двойного геометрического распределения
struct DoubleGeometricDistributionParams {
	double p1;       // p1 - вероятность успеха первого распределения
	double p2;       // p2 - вероятность успеха второго распределения
	double q;        // q - вероятность выбора первого распределения
	
	std::string toString() const;
};

// Оценка параметров нормального распределения по выборке
// Использует выборочное среднее и исправленное СКО
NormalDistributionParams estimateNormalDistribution(const Sample& sample);

// Оценка параметров пуассоновского распределения по выборке
// Использует метод моментов: λ = выборочное среднее
PoissonDistributionParams estimatePoissonDistribution(const Sample& sample);

// Оценка параметров геометрического распределения по выборке
// Использует метод моментов: p = 1/(1 + выборочное среднее)
GeometricDistributionParams estimateGeometricDistribution(const Sample& sample);

// Оценка параметров гипергеометрического распределения по выборке
// Использует метод моментов (приближенная оценка)
// Примечание: для точной оценки требуется дополнительная информация
HypergeometricDistributionParams estimateHypergeometricDistribution(const Sample& sample);

// Оценка параметров двойного пуассоновского распределения по выборке
// Использует метод моментов и метод максимального правдоподобия
DoublePoissonDistributionParams estimateDoublePoissonDistribution(const Sample& sample);

// Оценка параметров двойного геометрического распределения по выборке
// Использует метод моментов и метод максимального правдоподобия
DoubleGeometricDistributionParams estimateDoubleGeometricDistribution(const Sample& sample);

#endif

