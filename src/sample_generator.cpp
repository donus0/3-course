#define _USE_MATH_DEFINES
#include "../include/sample_generator.h"
#include <random>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <vector>
#include <map>
#include <fstream>
#include <set>

// Преобразование выборки в строку
std::string Sample::toString() const {
	std::ostringstream oss;
	oss << "Выборка (размер: " << getTotalSize() << "):\n";
	for (const auto& pair : data) {
		oss << "  " << std::fixed << std::setprecision(6) << pair.first 
		    << " : " << pair.second << "\n";
	}
	return oss.str();
}

// Получение всех уникальных значений
std::vector<double> Sample::getValues() const {
	std::vector<double> values;
	values.reserve(data.size());
	for (const auto& pair : data) {
		values.push_back(pair.first);
	}
	return values;
}

// Получение частот
std::vector<int> Sample::getFrequencies() const {
	std::vector<int> frequencies;
	frequencies.reserve(data.size());
	for (const auto& pair : data) {
		frequencies.push_back(pair.second);
	}
	return frequencies;
}

// Общий размер выборки
int Sample::getTotalSize() const {
	int total = 0;
	for (const auto& pair : data) {
		total += pair.second;
	}
	return total;
}

// Генерация выборки нормального распределения
Sample generateNormalSample(int size, double mean, double stddev, unsigned int seed) {
	if (size <= 0) {
		throw std::invalid_argument("Размер выборки должен быть положительным");
	}
	if (stddev <= 0) {
		throw std::invalid_argument("Среднеквадратическое отклонение должно быть положительным");
	}
	
	std::mt19937 gen;
	if (seed == 0) {
		std::random_device rd;
		gen.seed(rd());
	} else {
		gen.seed(seed);
	}
	
	std::normal_distribution<double> dist(mean, stddev);
	std::map<double, int> frequencyMap;
	
	for (int i = 0; i < size; ++i) {
		double value = dist(gen);
		// Округляем до 2 знаков после запятой для группировки
		value = std::round(value * 100.0) / 100.0;
		frequencyMap[value]++;
	}
	
	Sample sample;
	for (const auto& pair : frequencyMap) {
		sample.data.emplace_back(pair.first, pair.second);
	}
	
	return sample;
}

// Генерация выборки пуассоновского распределения
Sample generatePoissonSample(int size, double lambda, unsigned int seed) {
	if (size <= 0) {
		throw std::invalid_argument("Размер выборки должен быть положительным");
	}
	if (lambda <= 0) {
		throw std::invalid_argument("Параметр lambda должен быть положительным");
	}
	
	std::mt19937 gen;
	if (seed == 0) {
		std::random_device rd;
		gen.seed(rd());
	} else {
		gen.seed(seed);
	}
	
	std::poisson_distribution<int> dist(lambda);
	std::map<int, int> frequencyMap;
	
	for (int i = 0; i < size; ++i) {
		int value = dist(gen);
		frequencyMap[value]++;
	}
	
	Sample sample;
	for (const auto& pair : frequencyMap) {
		sample.data.emplace_back(static_cast<double>(pair.first), pair.second);
	}
	
	return sample;
}

// Генерация выборки геометрического распределения
Sample generateGeometricSample(int size, double p, unsigned int seed) {
	if (size <= 0) {
		throw std::invalid_argument("Размер выборки должен быть положительным");
	}
	if (p <= 0 || p > 1) {
		throw std::invalid_argument("Вероятность p должна быть в диапазоне (0, 1]");
	}
	
	std::mt19937 gen;
	if (seed == 0) {
		std::random_device rd;
		gen.seed(rd());
	} else {
		gen.seed(seed);
	}
	
	std::geometric_distribution<int> dist(p);
	std::map<int, int> frequencyMap;
	
	for (int i = 0; i < size; ++i) {
		int value = dist(gen);
		frequencyMap[value]++;
	}
	
	Sample sample;
	for (const auto& pair : frequencyMap) {
		sample.data.emplace_back(static_cast<double>(pair.first), pair.second);
	}
	
	return sample;
}

// Генерация выборки гипергеометрического распределения
// Реализовано через метод отбора без возвращения
Sample generateHypergeometricSample(int size, int N, int K, int n, unsigned int seed) {
	if (size <= 0) {
		throw std::invalid_argument("Размер выборки должен быть положительным");
	}
	if (N <= 0 || K < 0 || n < 0 || K > N || n > N) {
		throw std::invalid_argument("Некорректные параметры гипергеометрического распределения");
	}
	
	std::mt19937 gen;
	if (seed == 0) {
		std::random_device rd;
		gen.seed(rd());
	} else {
		gen.seed(seed);
	}
	
	std::map<int, int> frequencyMap;
	
	// Генерируем size выборок размера n из генеральной совокупности N с K успешными элементами
	for (int sampleIdx = 0; sampleIdx < size; ++sampleIdx) {
		// Создаем генеральную совокупность: K успешных и (N-K) неуспешных
		std::vector<bool> population(N, false);
		for (int i = 0; i < K; ++i) {
			population[i] = true; // Помечаем первые K элементов как успешные
		}
		
		// Перемешиваем генеральную совокупность
		std::shuffle(population.begin(), population.end(), gen);
		
		// Выбираем n элементов без возвращения
		int successes = 0;
		for (int i = 0; i < n; ++i) {
			if (population[i]) {
				successes++;
			}
		}
		
		frequencyMap[successes]++;
	}
	
	Sample sample;
	for (const auto& pair : frequencyMap) {
		sample.data.emplace_back(static_cast<double>(pair.first), pair.second);
	}
	
	return sample;
}

// Генерация выборки двойного пуассоновского распределения
Sample generateDoublePoissonSample(int size, double lambda1, double lambda2, double p, unsigned int seed) {
	if (size <= 0) {
		throw std::invalid_argument("Размер выборки должен быть положительным");
	}
	if (lambda1 <= 0 || lambda2 <= 0) {
		throw std::invalid_argument("Параметры lambda должны быть положительными");
	}
	if (p < 0 || p > 1) {
		throw std::invalid_argument("Вероятность p должна быть в диапазоне [0, 1]");
	}
	
	std::mt19937 gen;
	if (seed == 0) {
		std::random_device rd;
		gen.seed(rd());
	} else {
		gen.seed(seed);
	}
	
	std::bernoulli_distribution chooseDist(p);
	std::poisson_distribution<int> dist1(lambda1);
	std::poisson_distribution<int> dist2(lambda2);
	std::map<int, int> frequencyMap;
	
	for (int i = 0; i < size; ++i) {
		int value;
		if (chooseDist(gen)) {
			value = dist1(gen);
		} else {
			value = dist2(gen);
		}
		frequencyMap[value]++;
	}
	
	Sample sample;
	for (const auto& pair : frequencyMap) {
		sample.data.emplace_back(static_cast<double>(pair.first), pair.second);
	}
	
	return sample;
}

// Генерация выборки двойного геометрического распределения
Sample generateDoubleGeometricSample(int size, double p1, double p2, double q, unsigned int seed) {
	if (size <= 0) {
		throw std::invalid_argument("Размер выборки должен быть положительным");
	}
	if (p1 <= 0 || p1 > 1 || p2 <= 0 || p2 > 1) {
		throw std::invalid_argument("Вероятности p1 и p2 должны быть в диапазоне (0, 1]");
	}
	if (q < 0 || q > 1) {
		throw std::invalid_argument("Вероятность q должна быть в диапазоне [0, 1]");
	}
	
	std::mt19937 gen;
	if (seed == 0) {
		std::random_device rd;
		gen.seed(rd());
	} else {
		gen.seed(seed);
	}
	
	std::bernoulli_distribution chooseDist(q);
	std::geometric_distribution<int> dist1(p1);
	std::geometric_distribution<int> dist2(p2);
	std::map<int, int> frequencyMap;
	
	for (int i = 0; i < size; ++i) {
		int value;
		if (chooseDist(gen)) {
			value = dist1(gen);
		} else {
			value = dist2(gen);
		}
		frequencyMap[value]++;
	}
	
	Sample sample;
	for (const auto& pair : frequencyMap) {
		sample.data.emplace_back(static_cast<double>(pair.first), pair.second);
	}
	
	return sample;
}

// Загрузка выборки из текстового файла
Sample loadSampleFromFile(const std::string& filename, 
                          bool requireUniqueValues,
                          bool requireNonNegativeValues,
                          bool requireNonNegativeCounts) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		throw std::runtime_error("Не удалось открыть файл: " + filename);
	}
	
	Sample sample;
	std::set<double> seenValues;
	std::string line;
	int lineNumber = 0;
	
	while (std::getline(file, line)) {
		lineNumber++;
		
		// Пропускаем пустые строки и комментарии
		if (line.empty() || line[0] == '#') {
			continue;
		}
		
		// Удаляем пробелы в начале и конце строки
		line.erase(0, line.find_first_not_of(" \t"));
		line.erase(line.find_last_not_of(" \t") + 1);
		
		if (line.empty()) {
			continue;
		}
		
		// Парсим строку: значение и количество
		std::istringstream iss(line);
		double value;
		int count;
		
		if (!(iss >> value >> count)) {
			throw std::runtime_error("Ошибка парсинга строки " + std::to_string(lineNumber) + 
			                        " в файле " + filename + ": ожидается формат 'значение количество'");
		}
		
		// Проверка на неотрицательность значений xi
		if (requireNonNegativeValues && value < 0) {
			throw std::runtime_error("Ошибка в строке " + std::to_string(lineNumber) + 
			                        ": значение xi = " + std::to_string(value) + " отрицательно");
		}
		
		// Проверка на неотрицательность количеств xj
		if (requireNonNegativeCounts && count < 0) {
			throw std::runtime_error("Ошибка в строке " + std::to_string(lineNumber) + 
			                        ": количество xj = " + std::to_string(count) + " отрицательно");
		}
		
		// Проверка на уникальность значений xi
		if (requireUniqueValues) {
			if (seenValues.find(value) != seenValues.end()) {
				throw std::runtime_error("Ошибка в строке " + std::to_string(lineNumber) + 
				                        ": значение xi = " + std::to_string(value) + " уже встречалось");
			}
			seenValues.insert(value);
		}
		
		// Проверка на нулевое количество (может быть допустимо, но обычно не нужно)
		if (count == 0) {
			continue; // Пропускаем записи с нулевым количеством
		}
		
		sample.data.emplace_back(value, count);
	}
	
	file.close();
	
	if (sample.data.empty()) {
		throw std::runtime_error("Файл " + filename + " не содержит корректных данных");
	}
	
	return sample;
}

// Выборочное среднее (несмещённая оценка математического ожидания)
double sampleMean(const Sample& sample) {
	int n = sample.getTotalSize();
	if (n == 0) {
		throw std::runtime_error("Нельзя вычислить среднее для пустой выборки");
	}
	
	double sum = 0.0;
	for (const auto& pair : sample.data) {
		sum += pair.first * pair.second;
	}
	
	return sum / n;
}

// Выборочная дисперсия (смещённая оценка дисперсии)
double sampleVariance(const Sample& sample) {
	int n = sample.getTotalSize();
	if (n == 0) {
		throw std::runtime_error("Нельзя вычислить дисперсию для пустой выборки");
	}
	if (n == 1) {
		return 0.0; // Для выборки из одного элемента дисперсия равна 0
	}
	
	double mean = sampleMean(sample);
	double sum = 0.0;
	
	for (const auto& pair : sample.data) {
		double diff = pair.first - mean;
		sum += pair.second * diff * diff;
	}
	
	return sum / n;
}

// Исправленная дисперсия (несмещённая оценка дисперсии)
double correctedVariance(const Sample& sample) {
	int n = sample.getTotalSize();
	if (n == 0) {
		throw std::runtime_error("Нельзя вычислить исправленную дисперсию для пустой выборки");
	}
	if (n == 1) {
		return 0.0; // Для выборки из одного элемента дисперсия равна 0
	}
	
	// Исправленная дисперсия = (n/(n-1)) * выборочная дисперсия
	double biasedVariance = sampleVariance(sample);
	return biasedVariance * n / (n - 1);
}

// Выборочное среднеквадратическое отклонение
double sampleStandardDeviation(const Sample& sample) {
	return std::sqrt(sampleVariance(sample));
}

// Исправленное среднеквадратическое отклонение
double correctedStandardDeviation(const Sample& sample) {
	return std::sqrt(correctedVariance(sample));
}

// Вспомогательная функция для вычисления p-value распределения хи-квадрат
// Используется приближение через гамма-функцию
double chiSquarePValue(double chiSquare, int degreesOfFreedom) {
	// Упрощенное приближение для вычисления p-value
	// Для более точных результатов можно использовать библиотеки, но для демонстрации используем приближение
	
	if (degreesOfFreedom <= 0) {
		return 1.0;
	}
	
	// Простое приближение: если chiSquare очень большой, p-value близок к 0
	// Если chiSquare очень маленький, p-value близок к 1
	// Используем приближение через нормальное распределение для больших степеней свободы
	if (degreesOfFreedom > 30) {
		double z = (chiSquare - degreesOfFreedom) / std::sqrt(2.0 * degreesOfFreedom);
		// Приближение через нормальное распределение
		// P(Z > z) ≈ 0.5 * erfc(z / sqrt(2))
		return 0.5 * std::erfc(z / std::sqrt(2.0));
	}
	
	// Для малых степеней свободы используем табличные значения и интерполяцию
	// Упрощенная версия: если chiSquare < degreesOfFreedom, p-value > 0.5
	// Если chiSquare > 2*degreesOfFreedom, p-value < 0.05
	if (chiSquare < degreesOfFreedom * 0.5) {
		return 0.9; // Приблизительно
	} else if (chiSquare > degreesOfFreedom * 2.0) {
		return 0.01; // Приблизительно
	} else {
		// Линейная интерполяция между известными точками
		double ratio = chiSquare / degreesOfFreedom;
		if (ratio < 1.0) {
			return 0.5 + 0.4 * (1.0 - ratio);
		} else {
			return 0.5 - 0.45 * (ratio - 1.0);
		}
	}
}

// Критическое значение хи-квадрат для заданного уровня значимости
// Упрощенная версия с табличными значениями
double chiSquareCriticalValue(int degreesOfFreedom, double significanceLevel) {
	// Табличные значения для уровня значимости 0.05
	static const double criticalValues_005[] = {
		3.84, 5.99, 7.81, 9.49, 11.07, 12.59, 14.07, 15.51, 16.92, 18.31,
		19.68, 21.03, 22.36, 23.68, 25.00, 26.30, 27.59, 28.87, 30.14, 31.41,
		32.67, 33.92, 35.17, 36.42, 37.65, 38.89, 40.11, 41.34, 42.56, 43.77
	};
	
	if (significanceLevel == 0.05) {
		if (degreesOfFreedom >= 1 && degreesOfFreedom <= 30) {
			return criticalValues_005[degreesOfFreedom - 1];
		} else if (degreesOfFreedom > 30) {
			// Приближение для больших степеней свободы
			return degreesOfFreedom + 1.96 * std::sqrt(2.0 * degreesOfFreedom);
		}
	}
	
	// Для других уровней значимости используем приближение
	if (degreesOfFreedom > 30) {
		double z = (significanceLevel == 0.01) ? 2.58 : 1.96;
		return degreesOfFreedom + z * std::sqrt(2.0 * degreesOfFreedom);
	}
	
	// Для малых степеней свободы возвращаем приблизительное значение
	return degreesOfFreedom * 1.5;
}

// Преобразование результата проверки гипотезы в строку
std::string HypothesisTestResult::toString() const {
	std::ostringstream oss;
	oss << "Результат проверки гипотезы:\n";
	oss << "  Статистика хи-квадрат: " << std::fixed << std::setprecision(6) << chiSquare << "\n";
	oss << "  Число степеней свободы: " << degreesOfFreedom << "\n";
	oss << "  p-value: " << std::scientific << std::setprecision(6) << pValue << "\n";
	oss << "  Уровень значимости: " << std::fixed << std::setprecision(4) << significanceLevel << "\n";
	oss << "  Гипотеза: " << (hypothesisAccepted ? "ПРИНЯТА" : "ОТКЛОНЕНА") << "\n";
	return oss.str();
}

// Проверка гипотезы о нормальном распределении
HypothesisTestResult testNormalDistribution(const Sample& sample, double significanceLevel) {
	HypothesisTestResult result;
	result.significanceLevel = significanceLevel;
	
	int n = sample.getTotalSize();
	if (n < 10) {
		throw std::runtime_error("Выборка слишком мала для проверки гипотезы (минимум 10 элементов)");
	}
	
	// Оцениваем параметры по выборке
	double mean = sampleMean(sample);
	double stddev = correctedStandardDeviation(sample);
	
	if (stddev <= 0) {
		throw std::runtime_error("Нельзя проверить гипотезу: стандартное отклонение равно нулю");
	}
	
	// Группируем данные в интервалы (для непрерывного распределения)
	// Используем правило Стёрджеса: k = 1 + 3.322*log10(n)
	int numBins = static_cast<int>(1 + 3.322 * std::log10(n));
	if (numBins < 5) numBins = 5;
	if (numBins > 20) numBins = 20;
	
	// Находим диапазон данных
	double minVal = sample.data[0].first;
	double maxVal = sample.data[0].first;
	for (const auto& pair : sample.data) {
		if (pair.first < minVal) minVal = pair.first;
		if (pair.first > maxVal) maxVal = pair.first;
	}
	
	double binWidth = (maxVal - minVal) / numBins;
	if (binWidth <= 0) binWidth = 1.0;
	
	// Подсчитываем наблюдаемые частоты в интервалах
	std::vector<int> observedFreq(numBins, 0);
	for (const auto& pair : sample.data) {
		int bin = static_cast<int>((pair.first - minVal) / binWidth);
		if (bin >= numBins) bin = numBins - 1;
		observedFreq[bin] += pair.second;
	}
	
	// Вычисляем теоретические частоты
	std::vector<double> expectedFreq(numBins, 0.0);
	double chiSquare = 0.0;
	int validBins = 0;
	
	for (int i = 0; i < numBins; ++i) {
		double binStart = minVal + i * binWidth;
		double binEnd = minVal + (i + 1) * binWidth;
		
		// Вероятность попадания в интервал для нормального распределения
		// Используем функцию Лапласа (нужно будет добавить её использование)
		// Для упрощения используем приближение через плотность
		double midPoint = (binStart + binEnd) / 2.0;
		double density = std::exp(-0.5 * std::pow((midPoint - mean) / stddev, 2)) / (stddev * std::sqrt(2.0 * M_PI));
		expectedFreq[i] = density * binWidth * n;
		
		// Объединяем интервалы с малой ожидаемой частотой (правило: минимум 5)
		if (expectedFreq[i] < 5.0 && i < numBins - 1) {
			// Объединяем с следующим интервалом
			continue;
		}
		
		if (expectedFreq[i] > 0.1) { // Минимальный порог
			chiSquare += std::pow(observedFreq[i] - expectedFreq[i], 2) / expectedFreq[i];
			validBins++;
		}
	}
	
	result.chiSquare = chiSquare;
	result.degreesOfFreedom = validBins - 1 - 2; // -2 за оцененные параметры (mean, stddev)
	if (result.degreesOfFreedom < 1) result.degreesOfFreedom = 1;
	
	result.pValue = chiSquarePValue(chiSquare, result.degreesOfFreedom);
	double criticalValue = chiSquareCriticalValue(result.degreesOfFreedom, significanceLevel);
	result.hypothesisAccepted = (chiSquare < criticalValue);
	
	return result;
}

// Проверка гипотезы о пуассоновском распределении
HypothesisTestResult testPoissonDistribution(const Sample& sample, double significanceLevel) {
	HypothesisTestResult result;
	result.significanceLevel = significanceLevel;
	
	int n = sample.getTotalSize();
	if (n < 10) {
		throw std::runtime_error("Выборка слишком мала для проверки гипотезы");
	}
	
	// Оцениваем параметр lambda по выборке (методом моментов: lambda = среднее)
	double lambda = sampleMean(sample);
	if (lambda <= 0) {
		throw std::runtime_error("Оценка параметра lambda должна быть положительной");
	}
	
	// Вычисляем теоретические вероятности для каждого наблюдаемого значения
	double chiSquare = 0.0;
	int validValues = 0;
	
	// Находим максимальное значение в выборке
	int maxValue = static_cast<int>(sample.data[0].first);
	for (const auto& pair : sample.data) {
		int val = static_cast<int>(pair.first);
		if (val > maxValue) maxValue = val;
	}
	
	// Объединяем значения с малой ожидаемой частотой
	std::map<int, int> groupedObserved;
	std::map<int, double> groupedExpected;
	
	for (const auto& pair : sample.data) {
		int val = static_cast<int>(pair.first);
		// Вероятность для пуассоновского распределения: P(k) = (lambda^k * e^(-lambda)) / k!
		double prob = std::pow(lambda, val) * std::exp(-lambda);
		for (int i = 2; i <= val; ++i) {
			prob /= i;
		}
		
		double expected = prob * n;
		
		if (expected < 5.0 && val < maxValue) {
			// Объединяем с другими малыми значениями
			groupedObserved[-1] += pair.second; // Группа "остальные"
			groupedExpected[-1] += expected;
		} else {
			groupedObserved[val] += pair.second;
			groupedExpected[val] = expected;
		}
	}
	
	// Вычисляем статистику хи-квадрат
	for (const auto& pair : groupedObserved) {
		if (groupedExpected[pair.first] > 0.1) {
			chiSquare += std::pow(pair.second - groupedExpected[pair.first], 2) / groupedExpected[pair.first];
			validValues++;
		}
	}
	
	result.chiSquare = chiSquare;
	result.degreesOfFreedom = validValues - 1 - 1; // -1 за оцененный параметр lambda
	if (result.degreesOfFreedom < 1) result.degreesOfFreedom = 1;
	
	result.pValue = chiSquarePValue(chiSquare, result.degreesOfFreedom);
	double criticalValue = chiSquareCriticalValue(result.degreesOfFreedom, significanceLevel);
	result.hypothesisAccepted = (chiSquare < criticalValue);
	
	return result;
}

// Проверка гипотезы о геометрическом распределении
HypothesisTestResult testGeometricDistribution(const Sample& sample, double significanceLevel) {
	HypothesisTestResult result;
	result.significanceLevel = significanceLevel;
	
	int n = sample.getTotalSize();
	if (n < 10) {
		throw std::runtime_error("Выборка слишком мала для проверки гипотезы");
	}
	
	// Оцениваем параметр p по выборке
	// Для геометрического распределения: E[X] = (1-p)/p, отсюда p = 1/(1+E[X])
	double mean = sampleMean(sample);
	double p = 1.0 / (1.0 + mean);
	if (p <= 0 || p > 1) {
		throw std::runtime_error("Оценка параметра p некорректна");
	}
	
	double chiSquare = 0.0;
	int validValues = 0;
	
	// Находим максимальное значение
	int maxValue = static_cast<int>(sample.data[0].first);
	for (const auto& pair : sample.data) {
		int val = static_cast<int>(pair.first);
		if (val > maxValue) maxValue = val;
	}
	
	// Группируем значения
	std::map<int, int> groupedObserved;
	std::map<int, double> groupedExpected;
	
	for (const auto& pair : sample.data) {
		int val = static_cast<int>(pair.first);
		// Вероятность для геометрического распределения: P(k) = p * (1-p)^k
		double prob = p * std::pow(1.0 - p, val);
		double expected = prob * n;
		
		if (expected < 5.0 && val < maxValue) {
			groupedObserved[-1] += pair.second;
			groupedExpected[-1] += expected;
		} else {
			groupedObserved[val] += pair.second;
			groupedExpected[val] = expected;
		}
	}
	
	for (const auto& pair : groupedObserved) {
		if (groupedExpected[pair.first] > 0.1) {
			chiSquare += std::pow(pair.second - groupedExpected[pair.first], 2) / groupedExpected[pair.first];
			validValues++;
		}
	}
	
	result.chiSquare = chiSquare;
	result.degreesOfFreedom = validValues - 1 - 1; // -1 за оцененный параметр p
	if (result.degreesOfFreedom < 1) result.degreesOfFreedom = 1;
	
	result.pValue = chiSquarePValue(chiSquare, result.degreesOfFreedom);
	double criticalValue = chiSquareCriticalValue(result.degreesOfFreedom, significanceLevel);
	result.hypothesisAccepted = (chiSquare < criticalValue);
	
	return result;
}

// Проверка гипотезы о гипергеометрическом распределении
HypothesisTestResult testHypergeometricDistribution(const Sample& sample, int N, int K, int n, double significanceLevel) {
	HypothesisTestResult result;
	result.significanceLevel = significanceLevel;
	
	if (N <= 0 || K < 0 || n < 0 || K > N || n > N) {
		throw std::invalid_argument("Некорректные параметры гипергеометрического распределения");
	}
	
	int sampleSize = sample.getTotalSize();
	if (sampleSize < 10) {
		throw std::runtime_error("Выборка слишком мала для проверки гипотезы");
	}
	
	double chiSquare = 0.0;
	int validValues = 0;
	
	// Вычисляем теоретические вероятности
	std::map<int, int> observedFreq;
	std::map<int, double> expectedFreq;
	
	for (const auto& pair : sample.data) {
		int val = static_cast<int>(pair.first);
		observedFreq[val] += pair.second;
	}
	
	// Вероятность для гипергеометрического распределения
	// P(k) = C(K,k) * C(N-K, n-k) / C(N, n)
	// Вычисляем комбинации
	auto binom = [](int n, int k) -> double {
		if (k < 0 || k > n) return 0.0;
		if (k == 0 || k == n) return 1.0;
		double result = 1.0;
		for (int i = 1; i <= k; ++i) {
			result = result * (n - k + i) / i;
		}
		return result;
	};
	
	double totalCombinations = binom(N, n);
	
	for (const auto& pair : observedFreq) {
		int k = pair.first;
		if (k < 0 || k > std::min(K, n)) continue;
		
		double prob = (binom(K, k) * binom(N - K, n - k)) / totalCombinations;
		double expected = prob * sampleSize;
		
		if (expected > 0.1) {
			expectedFreq[k] = expected;
			chiSquare += std::pow(pair.second - expected, 2) / expected;
			validValues++;
		}
	}
	
	result.chiSquare = chiSquare;
	result.degreesOfFreedom = validValues - 1; // Параметры заданы, не оцениваются
	if (result.degreesOfFreedom < 1) result.degreesOfFreedom = 1;
	
	result.pValue = chiSquarePValue(chiSquare, result.degreesOfFreedom);
	double criticalValue = chiSquareCriticalValue(result.degreesOfFreedom, significanceLevel);
	result.hypothesisAccepted = (chiSquare < criticalValue);
	
	return result;
}

// Проверка гипотезы о двойном пуассоновском распределении
HypothesisTestResult testDoublePoissonDistribution(const Sample& sample, double lambda1, double lambda2, double p, double significanceLevel) {
	HypothesisTestResult result;
	result.significanceLevel = significanceLevel;
	
	if (lambda1 <= 0 || lambda2 <= 0 || p < 0 || p > 1) {
		throw std::invalid_argument("Некорректные параметры двойного пуассоновского распределения");
	}
	
	int n = sample.getTotalSize();
	if (n < 10) {
		throw std::runtime_error("Выборка слишком мала для проверки гипотезы");
	}
	
	double chiSquare = 0.0;
	int validValues = 0;
	
	// Вычисляем теоретические вероятности
	std::map<int, int> observedFreq;
	std::map<int, double> expectedFreq;
	
	for (const auto& pair : sample.data) {
		int val = static_cast<int>(pair.first);
		observedFreq[val] += pair.second;
	}
	
	// Вероятность для двойного пуассоновского: P(k) = p * P1(k) + (1-p) * P2(k)
	for (const auto& pair : observedFreq) {
		int k = pair.first;
		
		// P1(k) для пуассоновского с lambda1
		double prob1 = std::pow(lambda1, k) * std::exp(-lambda1);
		for (int i = 2; i <= k; ++i) {
			prob1 /= i;
		}
		
		// P2(k) для пуассоновского с lambda2
		double prob2 = std::pow(lambda2, k) * std::exp(-lambda2);
		for (int i = 2; i <= k; ++i) {
			prob2 /= i;
		}
		
		double prob = p * prob1 + (1.0 - p) * prob2;
		double expected = prob * n;
		
		if (expected > 0.1) {
			expectedFreq[k] = expected;
			chiSquare += std::pow(pair.second - expected, 2) / expected;
			validValues++;
		}
	}
	
	result.chiSquare = chiSquare;
	result.degreesOfFreedom = validValues - 1; // Параметры заданы
	if (result.degreesOfFreedom < 1) result.degreesOfFreedom = 1;
	
	result.pValue = chiSquarePValue(chiSquare, result.degreesOfFreedom);
	double criticalValue = chiSquareCriticalValue(result.degreesOfFreedom, significanceLevel);
	result.hypothesisAccepted = (chiSquare < criticalValue);
	
	return result;
}

// Проверка гипотезы о двойном геометрическом распределении
HypothesisTestResult testDoubleGeometricDistribution(const Sample& sample, double p1, double p2, double q, double significanceLevel) {
	HypothesisTestResult result;
	result.significanceLevel = significanceLevel;
	
	if (p1 <= 0 || p1 > 1 || p2 <= 0 || p2 > 1 || q < 0 || q > 1) {
		throw std::invalid_argument("Некорректные параметры двойного геометрического распределения");
	}
	
	int n = sample.getTotalSize();
	if (n < 10) {
		throw std::runtime_error("Выборка слишком мала для проверки гипотезы");
	}
	
	double chiSquare = 0.0;
	int validValues = 0;
	
	std::map<int, int> observedFreq;
	std::map<int, double> expectedFreq;
	
	for (const auto& pair : sample.data) {
		int val = static_cast<int>(pair.first);
		observedFreq[val] += pair.second;
	}
	
	// Вероятность для двойного геометрического: P(k) = q * P1(k) + (1-q) * P2(k)
	for (const auto& pair : observedFreq) {
		int k = pair.first;
		
		// P1(k) для геометрического с p1
		double prob1 = p1 * std::pow(1.0 - p1, k);
		
		// P2(k) для геометрического с p2
		double prob2 = p2 * std::pow(1.0 - p2, k);
		
		double prob = q * prob1 + (1.0 - q) * prob2;
		double expected = prob * n;
		
		if (expected > 0.1) {
			expectedFreq[k] = expected;
			chiSquare += std::pow(pair.second - expected, 2) / expected;
			validValues++;
		}
	}
	
	result.chiSquare = chiSquare;
	result.degreesOfFreedom = validValues - 1; // Параметры заданы
	if (result.degreesOfFreedom < 1) result.degreesOfFreedom = 1;
	
	result.pValue = chiSquarePValue(chiSquare, result.degreesOfFreedom);
	double criticalValue = chiSquareCriticalValue(result.degreesOfFreedom, significanceLevel);
	result.hypothesisAccepted = (chiSquare < criticalValue);
	
	return result;
}