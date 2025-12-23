#include "../include/sample_generator.h"
#include <random>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <vector>
#include <map>

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