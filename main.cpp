#include "visualizer.h"
#include "random_variable.h"
#include "random_walk_visualizer.h"
#include "laplace_function.h"
#include "sample_generator.h"
#include <iostream>
#include <limits>
#include <windows.h>
#include <commdlg.h>
#include <set>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <vector>
#include <sstream>


std::vector<std::pair<double, double>> readDistributionFromStdin() {
	std::vector<std::pair<double, double>> dist;
	std::cout << "Введите количество исходов n: ";
	int n = 0;
	while (!(std::cin >> n) || n <= 0) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Некорректное n. Повторите: ";
	}

	std::set<double> usedValues;
	dist.reserve(static_cast<size_t>(n));
	for (int i = 0; i < n; ++i) {
		double x = 0.0, p = 0.0;
		std::cout << "Введите пару x p для i=" << (i + 1) << ": ";
		while (!(std::cin >> x >> p) || p < 0.0) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Некорректный ввод. Ожидается: <x: число> <p: неотрицательное>. Повторите: ";
		}
		if (usedValues.count(x)) {
			std::cout << "Значение x=" << x << " уже встречалось. Введите уникальное значение.\n";
			--i;
			continue;
		}
		usedValues.insert(x);
		dist.emplace_back(x, p);
	}

	double sum = 0.0;
	for (const auto &pr : dist) sum += pr.second;
	const double eps = 1e-9;
	if (sum <= eps) {
		throw std::runtime_error("Сумма вероятностей равна нулю.");
	}
	if (std::abs(sum - 1.0) > 1e-6) {
		std::cout << "Сумма вероятностей = " << sum << ", будет выполнена нормализация к 1.\n";
		for (auto &pr : dist) pr.second /= sum;
	}
	return dist;
}

void demonstrateTask1_1() {
	std::cout << "=== Задание 1.1: Демонстрация функционала DiscreteRandomVariable ===\n\n";

	// Выбор источника распределения: CLI или пример по умолчанию
	bool useCli = false;
	std::cout << "Использовать ввод с клавиатуры? (y/n): ";
	char choice;
	std::cin >> choice;
	if (choice == 'y' || choice == 'Y') {
		useCli = true;
	}

	DiscreteRandomVariable rv1;
	if (useCli) {
		try {
			auto dist = readDistributionFromStdin();
			rv1.setDistribution(dist);
		} catch (const std::exception& ex) {
			std::cerr << "Ошибка ввода распределения: " << ex.what() << "\n";
			return;
		}
	} else {
		std::vector<std::pair<double, double>> dist1 = {
			{1, 0.1}, {2, 0.2}, {3, 0.4}, {4, 0.2}, {5, 0.1}
		};
		rv1.setDistribution(dist1);
		std::cout << "Используется распределение по умолчанию: {1:0.1, 2:0.2, 3:0.4, 4:0.2, 5:0.1}\n\n";
	}

	// 1. Отображение в виде закона распределения
	std::cout << "1. Закон распределения:\n";
	std::cout << rv1.toString() << "\n";

	// 2. Отображение в виде полилайна
	std::cout << "2. Отображение в виде полилайна:\n";
	std::cout << rv1.toPolylineString() << "\n";

	// 3. Отображение в виде функции распределения
	std::cout << "3. Отображение в виде функции распределения:\n";
	std::cout << rv1.toCDFString() << "\n";

	// 4. Умножение на скаляр
	std::cout << "4. Умножение ДСВ на скаляр (X * 2.0):\n";
	DiscreteRandomVariable rv2 = rv1 * 2.0;
	std::cout << rv2.toString() << "\n";

	// 5. Сложение ДСВ
	std::cout << "5. Сложение ДСВ (X + X):\n";
	DiscreteRandomVariable rv3 = rv1 + rv1;
	std::cout << rv3.toString() << "\n";

	// 6. Умножение ДСВ
	std::cout << "6. Умножение ДСВ (X * X):\n";
	DiscreteRandomVariable rv4 = rv1 * rv1;
	std::cout << rv4.toString() << "\n";

	// 7. Статистические характеристики
	std::cout << "7. Статистические характеристики исходной ДСВ:\n";
	std::cout << "   Математическое ожидание: " << rv1.expectation() << "\n";
	std::cout << "   Дисперсия: " << rv1.variance() << "\n";
	std::cout << "   Среднеквадратическое отклонение: " << rv1.standardDeviation() << "\n";
	std::cout << "   Коэффициент асимметрии: " << rv1.skewness() << "\n";
	std::cout << "   Коэффициент эксцесса: " << rv1.kurtosis() << "\n\n";

	// 8. Сериализация и десериализация
	std::cout << "8. Сериализация и десериализация:\n";
	const std::string filename = "rv_serialized.bin";
	try {
		// Сериализация
		std::ofstream outFile(filename, std::ios::binary);
		if (!outFile.is_open()) {
			throw std::runtime_error("Не удалось открыть файл для записи");
		}
		rv1.serialize(outFile);
		outFile.close();
		std::cout << "   ДСВ сериализована в файл: " << filename << "\n";

		// Десериализация
		std::ifstream inFile(filename, std::ios::binary);
		if (!inFile.is_open()) {
			throw std::runtime_error("Не удалось открыть файл для чтения");
		}
		DiscreteRandomVariable rv5;
		rv5.deserialize(inFile);
		inFile.close();
		std::cout << "   ДСВ десериализована из файла: " << filename << "\n";
		std::cout << "   Проверка (должно совпадать с исходной):\n";
		std::cout << rv5.toString() << "\n";
	} catch (const std::exception& ex) {
		std::cerr << "   Ошибка при сериализации/десериализации: " << ex.what() << "\n";
	}

	std::cout << "\n=== Задание 1.1 завершено ===\n";
}

void demonstrateTask1_2() {
	std::cout << "=== Задание 1.2: Визуализация ДСВ через OpenGL ===\n\n";

	Visualizer visualizer;

	// Выбор источника распределения: CLI или пример по умолчанию
	bool useCli = false;
	std::cout << "Использовать ввод с клавиатуры? (y/n): ";
	char choice;
	std::cin >> choice;
	if (choice == 'y' || choice == 'Y') {
		useCli = true;
	}

	DiscreteRandomVariable rv1;
	if (useCli) {
		try {
			auto dist = readDistributionFromStdin();
			rv1.setDistribution(dist);
		} catch (const std::exception& ex) {
			std::cerr << "Ошибка ввода распределения: " << ex.what() << "\n";
			return;
		}
	} else {
		std::vector<std::pair<double, double>> dist1 = {
			{1, 0.1}, {2, 0.2}, {3, 0.4}, {4, 0.2}, {5, 0.1}
		};
		rv1.setDistribution(dist1);
		std::cout << "Используется распределение по умолчанию: {1:0.1, 2:0.2, 3:0.4, 4:0.2, 5:0.1}\n\n";
	}

	// Инициализация окна
	if (!visualizer.initialize(800, 600, "Discrete Random Variable Visualizer")) {
		std::cerr << "Ошибка инициализации визуализатора\n";
		return;
	}

	// Устанавливаем указатель на визуализатор для callback'ов
	glfwSetWindowUserPointer(visualizer.getWindow(), &visualizer);

	// Установка для визуализации
	visualizer.setRandomVariable(rv1);

	std::cout << "Controls:\n";
	std::cout << "SPACE - Change view mode\n";
	std::cout << "ESC - Exit\n\n";

	visualizer.run();

	std::cout << "\n=== Задание 1.2 завершено ===\n";
}

std::string openFileDialog() {
	OPENFILENAMEA ofn;
	char szFile[260] = { 0 };
	
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Binary Files\0*.bin\0All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	
	if (GetOpenFileNameA(&ofn) == TRUE) {
		return std::string(szFile);
	}
	return "";
}

void demonstrateTask2() {
	std::cout << "=== Задание 2: Моделирование случайного блуждания ===\n\n";

	RandomWalkVisualizer visualizer;

	// Инициализация окна
	if (!visualizer.initialize(1000, 600, "Random Walk Simulator")) {
		std::cerr << "Ошибка инициализации визуализатора\n";
		return;
	}

	// Устанавливаем указатель на визуализатор для callback'ов
	glfwSetWindowUserPointer(visualizer.getWindow(), &visualizer);

	// Параметры по умолчанию
	double initialPosition = 0.0;
	int numberOfSteps = 10;
	DiscreteRandomVariable stepDistribution;

	// Выбор источника распределения
	std::cout << "Выберите способ задания закона перемещения:\n";
	std::cout << "1 - Использовать распределение по умолчанию\n";
	std::cout << "2 - Загрузить из файла (десериализация)\n";
	std::cout << "3 - Ввести с клавиатуры\n";
	std::cout << "Ваш выбор: ";

	int choice;
	std::cin >> choice;

	if (choice == 2) {
		// Загрузка из файла
		std::cout << "\nОткрывается диалог выбора файла...\n";
		std::string filename = openFileDialog();
		if (!filename.empty()) {
			visualizer.loadStepDistributionFromFile(filename);
		} else {
			std::cout << "Файл не выбран, используется распределение по умолчанию.\n";
			std::vector<std::pair<double, double>> defaultDist = {
				{-1.0, 0.25}, {0.0, 0.5}, {1.0, 0.25}
			};
			stepDistribution.setDistribution(defaultDist);
			visualizer.setStepDistribution(stepDistribution);
		}
	} else if (choice == 3) {
		// Ввод с клавиатуры
		try {
			auto dist = readDistributionFromStdin();
			stepDistribution.setDistribution(dist);
			visualizer.setStepDistribution(stepDistribution);
		} catch (const std::exception& ex) {
			std::cerr << "Ошибка ввода распределения: " << ex.what() << "\n";
			std::cout << "Используется распределение по умолчанию.\n";
			std::vector<std::pair<double, double>> defaultDist = {
				{-1.0, 0.25}, {0.0, 0.5}, {1.0, 0.25}
			};
			stepDistribution.setDistribution(defaultDist);
			visualizer.setStepDistribution(stepDistribution);
		}
	} else {
		// По умолчанию
		std::vector<std::pair<double, double>> defaultDist = {
			{-1.0, 0.25}, {0.0, 0.5}, {1.0, 0.25}
		};
		stepDistribution.setDistribution(defaultDist);
		visualizer.setStepDistribution(stepDistribution);
		std::cout << "Используется распределение по умолчанию: {-1:0.25, 0:0.5, 1:0.25}\n";
	}

	// Ввод начальной позиции
	std::cout << "\nВведите начальное положение точки (по умолчанию 0.0): ";
	std::string input;
	std::cin.ignore();
	std::getline(std::cin, input);
	if (!input.empty()) {
		try {
			initialPosition = std::stod(input);
		} catch (...) {
			initialPosition = 0.0;
		}
	}
	visualizer.setInitialPosition(initialPosition);

	// Ввод количества шагов
	std::cout << "Введите количество шагов (по умолчанию 10): ";
	std::getline(std::cin, input);
	if (!input.empty()) {
		try {
			numberOfSteps = std::stoi(input);
			if (numberOfSteps <= 0) numberOfSteps = 10;
		} catch (...) {
			numberOfSteps = 10;
		}
	}
	visualizer.setNumberOfSteps(numberOfSteps);

	std::cout << "\n=== Управление ===\n";
	std::cout << "SPACE - Запуск/Пауза\n";
	std::cout << "R - Сброс\n";
	std::cout << "S - Остановка\n";
	std::cout << "D - Показать/скрыть распределение конечных позиций\n";
	std::cout << "ESC - Выход\n\n";

	visualizer.run();

	// Вывод результата после завершения
	std::cout << "\n=== Результат моделирования ===\n";
	std::cout << "Распределение вероятностей попадания точки во все конечные позиции:\n";
	auto& walk = visualizer.getWalk();
	if (walk.isCompleted()) {
		std::cout << walk.getFinalPositionDistributionString() << "\n";
	} else {
		std::cout << "Моделирование не завершено.\n";
	}

	std::cout << "\n=== Задание 2 завершено ===\n";
}

void demonstrateTask3() {
	std::cout << "=== Задание 3: Вычисление интегральной функции Лапласа ===\n\n";
	
	// Ввод параметров
	double x = 0.0;
	int n = 1000; // Количество разбиений по умолчанию
	
	std::cout << "Введите значение аргумента x: ";
	if (!(std::cin >> x)) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cerr << "Некорректный ввод, используется x = 0.0\n";
		x = 0.0;
	}
	
	std::cout << "Введите количество разбиений n (по умолчанию 1000): ";
	std::string input;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::getline(std::cin, input);
	if (!input.empty()) {
		try {
			int inputN = std::stoi(input);
			if (inputN > 0) {
				n = inputN;
			} else {
				std::cerr << "Количество разбиений должно быть положительным, используется n = 1000\n";
			}
		} catch (...) {
			std::cerr << "Некорректный ввод, используется n = 1000\n";
		}
	}
	
	std::cout << "\n=== Результаты вычислений ===\n";
	std::cout << "Параметры:\n";
	std::cout << "  x = " << x << "\n";
	std::cout << "  n = " << n << "\n\n";
	
	// Вычисление методом прямоугольников
	double resultRect = laplaceFunction(x, n, false);
	std::cout << "Метод прямоугольников:\n";
	std::cout << "  Φ(" << x << ") = " << std::fixed << std::setprecision(10) << resultRect << "\n\n";
	
	// Вычисление методом трапеций
	double resultTrap = laplaceFunction(x, n, true);
	std::cout << "Метод трапеций:\n";
	std::cout << "  Φ(" << x << ") = " << std::fixed << std::setprecision(10) << resultTrap << "\n\n";
	
	// Сравнение результатов
	double difference = std::abs(resultRect - resultTrap);
	std::cout << "Разница между методами: " << std::scientific << std::setprecision(6) << difference << "\n\n";
	
	// Демонстрация для нескольких значений
	std::cout << "=== Таблица значений для различных x ===\n";
	std::cout << std::fixed << std::setprecision(6);
	std::cout << std::setw(10) << "x" << std::setw(20) << "Метод прям." << std::setw(20) << "Метод трап." << std::setw(20) << "Разница" << "\n";
	std::cout << std::string(70, '-') << "\n";
	
	std::vector<double> testValues = {-3.0, -2.0, -1.0, 0.0, 1.0, 2.0, 3.0};
	for (double testX : testValues) {
		double rect = laplaceFunction(testX, n, false);
		double trap = laplaceFunction(testX, n, true);
		double diff = std::abs(rect - trap);
		std::cout << std::setw(10) << testX 
		          << std::setw(20) << std::setprecision(10) << rect
		          << std::setw(20) << trap
		          << std::setw(20) << std::scientific << std::setprecision(6) << diff << "\n";
	}
	
	// Исследование сходимости
	std::cout << "\n=== Исследование сходимости (x = " << x << ") ===\n";
	std::cout << std::fixed << std::setprecision(6);
	std::cout << std::setw(10) << "n" << std::setw(20) << "Метод прям." << std::setw(20) << "Метод трап." << std::setw(20) << "Разница" << "\n";
	std::cout << std::string(70, '-') << "\n";
	
	std::vector<int> testN = {10, 50, 100, 500, 1000, 5000, 10000};
	for (int testNVal : testN) {
		double rect = laplaceFunction(x, testNVal, false);
		double trap = laplaceFunction(x, testNVal, true);
		double diff = std::abs(rect - trap);
		std::cout << std::setw(10) << testNVal 
		          << std::setw(20) << std::setprecision(10) << rect
		          << std::setw(20) << trap
		          << std::setw(20) << std::scientific << std::setprecision(6) << diff << "\n";
	}
	
	std::cout << "\n=== Задание 3 завершено ===\n";
}

void demonstrateTask4() {
	std::cout << "=== Задание 4: Поиск аргумента по значению функции Лапласа ===\n\n";
	
	// Ввод параметров
	double targetValue = 0.0;
	double epsilon = 1e-6;
	int n = 1000; // Количество разбиений для численного интегрирования
	
	std::cout << "Введите значение функции Φ(x), которое нужно найти: ";
	if (!(std::cin >> targetValue)) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cerr << "Некорректный ввод, используется значение 0.0\n";
		targetValue = 0.0;
	}
	
	std::cout << "Введите точность epsilon (по умолчанию 1e-6): ";
	std::string input;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::getline(std::cin, input);
	if (!input.empty()) {
		try {
			epsilon = std::stod(input);
			if (epsilon <= 0) {
				std::cerr << "Точность должна быть положительной, используется epsilon = 1e-6\n";
				epsilon = 1e-6;
			}
		} catch (...) {
			std::cerr << "Некорректный ввод, используется epsilon = 1e-6\n";
			epsilon = 1e-6;
		}
	}
	
	std::cout << "Введите количество разбиений n для численного интегрирования (по умолчанию 1000): ";
	std::getline(std::cin, input);
	if (!input.empty()) {
		try {
			int inputN = std::stoi(input);
			if (inputN > 0) {
				n = inputN;
			} else {
				std::cerr << "Количество разбиений должно быть положительным, используется n = 1000\n";
			}
		} catch (...) {
			std::cerr << "Некорректный ввод, используется n = 1000\n";
		}
	}
	
	std::cout << "\n=== Результаты вычислений ===\n";
	std::cout << "Параметры:\n";
	std::cout << "  Целевое значение Φ(x) = " << targetValue << "\n";
	std::cout << "  Точность epsilon = " << std::scientific << std::setprecision(6) << epsilon << "\n";
	std::cout << "  Количество разбиений n = " << std::fixed << n << "\n\n";
	
	// Проверка диапазона значений функции Лапласа
	// Функция Лапласа принимает значения примерно от -0.5 до 0.5
	const double maxValue = 0.5;
	if (std::abs(targetValue) > maxValue) {
		std::cerr << "Предупреждение: значение " << targetValue 
		          << " выходит за типичный диапазон функции Лапласа [-0.5, 0.5]\n";
		std::cerr << "Результаты могут быть неточными.\n\n";
	}
	
	// Метод деления отрезка пополам
	std::cout << "=== Метод деления отрезка пополам ===\n";
	try {
		// Определяем начальный отрезок
		// Для функции Лапласа значения лежат примерно в диапазоне [-4, 4] для значений функции [-0.5, 0.5]
		double a = -5.0;
		double b = 5.0;
		
		// Если значение отрицательное, ищем в отрицательной области
		if (targetValue < 0) {
			a = -5.0;
			b = 0.0;
		} else if (targetValue > 0) {
			a = 0.0;
			b = 5.0;
		} else {
			// Если значение 0, результат очевиден
			std::cout << "  x = 0.0 (значение функции равно 0)\n\n";
		}
		
		if (targetValue != 0.0) {
			double xBisection = laplaceInverseBisection(targetValue, a, b, epsilon, n);
			double computedValue = laplaceFunction(xBisection, n, true);
			double error = std::abs(computedValue - targetValue);
			
			std::cout << std::fixed << std::setprecision(10);
			std::cout << "  Найденный аргумент x = " << xBisection << "\n";
			std::cout << "  Проверка: Φ(" << xBisection << ") = " << computedValue << "\n";
			std::cout << "  Ошибка: |Φ(x) - target| = " << std::scientific << std::setprecision(6) << error << "\n\n";
		}
	} catch (const std::exception& ex) {
		std::cerr << "  Ошибка при использовании метода бисекции: " << ex.what() << "\n\n";
	}
	
	// Метод Ньютона
	std::cout << "=== Метод Ньютона ===\n";
	try {
		// Начальное приближение
		// Для положительных значений начинаем с положительного x, для отрицательных - с отрицательного
		double x0 = (targetValue >= 0) ? 1.0 : -1.0;
		
		double xNewton = laplaceInverseNewton(targetValue, x0, epsilon, n);
		double computedValue = laplaceFunction(xNewton, n, true);
		double error = std::abs(computedValue - targetValue);
		
		std::cout << std::fixed << std::setprecision(10);
		std::cout << "  Начальное приближение x0 = " << x0 << "\n";
		std::cout << "  Найденный аргумент x = " << xNewton << "\n";
		std::cout << "  Проверка: Φ(" << xNewton << ") = " << computedValue << "\n";
		std::cout << "  Ошибка: |Φ(x) - target| = " << std::scientific << std::setprecision(6) << error << "\n\n";
	} catch (const std::exception& ex) {
		std::cerr << "  Ошибка при использовании метода Ньютона: " << ex.what() << "\n\n";
	}
	
	// Сравнение методов
	std::cout << "=== Сравнение методов ===\n";
	try {
		double a = (targetValue < 0) ? -5.0 : 0.0;
		double b = (targetValue > 0) ? 5.0 : 0.0;
		
		if (targetValue != 0.0) {
			double xBisection = laplaceInverseBisection(targetValue, a, b, epsilon, n);
			double x0 = (targetValue >= 0) ? 1.0 : -1.0;
			double xNewton = laplaceInverseNewton(targetValue, x0, epsilon, n);
			
			double diff = std::abs(xBisection - xNewton);
			
			std::cout << std::fixed << std::setprecision(10);
			std::cout << "  Метод бисекции: x = " << xBisection << "\n";
			std::cout << "  Метод Ньютона:  x = " << xNewton << "\n";
			std::cout << "  Разница: |x_bisection - x_newton| = " 
			          << std::scientific << std::setprecision(6) << diff << "\n\n";
		}
	} catch (const std::exception& ex) {
		std::cerr << "  Не удалось сравнить методы: " << ex.what() << "\n\n";
	}
	
	// Демонстрация для нескольких значений
	std::cout << "\n=== Таблица результатов для различных значений ===\n";
	std::cout << std::fixed << std::setprecision(6);
	std::cout << std::setw(15) << "Φ(x)" << std::setw(20) << "Метод бисекции" 
	          << std::setw(20) << "Метод Ньютона" << std::setw(20) << "Разница" << "\n";
	std::cout << std::string(75, '-') << "\n";
	
	std::vector<double> testValues = {-0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3};
	for (double testTarget : testValues) {
		try {
			double a = (testTarget < 0) ? -5.0 : 0.0;
			double b = (testTarget > 0) ? 5.0 : 0.0;
			double x0 = (testTarget >= 0) ? 1.0 : -1.0;
			
			if (testTarget == 0.0) {
				std::cout << std::setw(15) << testTarget 
				          << std::setw(20) << 0.0 
				          << std::setw(20) << 0.0 
				          << std::setw(20) << 0.0 << "\n";
			} else {
				double xBisection = laplaceInverseBisection(testTarget, a, b, epsilon, n);
				double xNewton = laplaceInverseNewton(testTarget, x0, epsilon, n);
				double diff = std::abs(xBisection - xNewton);
				
				std::cout << std::setw(15) << std::setprecision(6) << testTarget
				          << std::setw(20) << std::setprecision(10) << xBisection
				          << std::setw(20) << xNewton
				          << std::setw(20) << std::scientific << std::setprecision(6) << diff << "\n";
			}
		} catch (...) {
			std::cout << std::setw(15) << testTarget 
			          << std::setw(20) << "Ошибка" 
			          << std::setw(20) << "Ошибка" 
			          << std::setw(20) << "N/A" << "\n";
		}
	}
	
	std::cout << "\n=== Задание 4 завершено ===\n";
}

void demonstrateTask5() {
	std::cout << "=== Задание 5: Генерация выборок для различных распределений ===\n\n";
	
	int sampleSize = 100;
	unsigned int seed = 0;
	
	std::cout << "Введите размер выборки (по умолчанию 100): ";
	std::string input;
	std::getline(std::cin, input);
	if (!input.empty()) {
		try {
			int inputSize = std::stoi(input);
			if (inputSize > 0) {
				sampleSize = inputSize;
			} else {
				std::cerr << "Размер выборки должен быть положительным, используется 100\n";
			}
		} catch (...) {
			std::cerr << "Некорректный ввод, используется размер 100\n";
		}
	}
	
	std::cout << "Введите seed для генератора случайных чисел (0 = случайный, по умолчанию 0): ";
	std::getline(std::cin, input);
	if (!input.empty()) {
		try {
			seed = static_cast<unsigned int>(std::stoul(input));
		} catch (...) {
			std::cerr << "Некорректный ввод, используется случайный seed\n";
			seed = 0;
		}
	}
	
	std::cout << "\n=== Параметры ===\n";
	std::cout << "Размер выборки: " << sampleSize << "\n";
	if (seed == 0) {
		std::cout << "Seed: случайный\n\n";
	} else {
		std::cout << "Seed: " << seed << "\n\n";
	}
	
	// a. Нормальное распределение
	std::cout << "=== a. Нормальное распределение ===\n";
	try {
		double mean = 0.0;
		double stddev = 1.0;
		
		std::cout << "Введите математическое ожидание (по умолчанию 0.0): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				mean = std::stod(input);
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 0.0\n";
			}
		}
		
		std::cout << "Введите среднеквадратическое отклонение (по умолчанию 1.0): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				stddev = std::stod(input);
				if (stddev <= 0) {
					std::cerr << "Среднеквадратическое отклонение должно быть положительным, используется 1.0\n";
					stddev = 1.0;
				}
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 1.0\n";
			}
		}
		
		Sample sample = generateNormalSample(sampleSize, mean, stddev, seed);
		std::cout << "Параметры: μ = " << mean << ", σ = " << stddev << "\n";
		std::cout << sample.toString() << "\n";
	} catch (const std::exception& ex) {
		std::cerr << "Ошибка: " << ex.what() << "\n\n";
	}
	
	// b. Пуассоновское распределение
	std::cout << "=== b. Пуассоновское распределение ===\n";
	try {
		double lambda = 3.0;
		
		std::cout << "Введите параметр lambda (по умолчанию 3.0): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				lambda = std::stod(input);
				if (lambda <= 0) {
					std::cerr << "Lambda должен быть положительным, используется 3.0\n";
					lambda = 3.0;
				}
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 3.0\n";
			}
		}
		
		Sample sample = generatePoissonSample(sampleSize, lambda, seed);
		std::cout << "Параметр: λ = " << lambda << "\n";
		std::cout << sample.toString() << "\n";
	} catch (const std::exception& ex) {
		std::cerr << "Ошибка: " << ex.what() << "\n\n";
	}
	
	// c. Геометрическое распределение
	std::cout << "=== c. Геометрическое распределение ===\n";
	try {
		double p = 0.3;
		
		std::cout << "Введите вероятность успеха p (по умолчанию 0.3): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				p = std::stod(input);
				if (p <= 0 || p > 1) {
					std::cerr << "Вероятность должна быть в диапазоне (0, 1], используется 0.3\n";
					p = 0.3;
				}
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 0.3\n";
			}
		}
		
		Sample sample = generateGeometricSample(sampleSize, p, seed);
		std::cout << "Параметр: p = " << p << "\n";
		std::cout << sample.toString() << "\n";
	} catch (const std::exception& ex) {
		std::cerr << "Ошибка: " << ex.what() << "\n\n";
	}
	
	// d. Гипергеометрическое распределение
	std::cout << "=== d. Гипергеометрическое распределение ===\n";
	try {
		int N = 100;
		int K = 30;
		int n = 20;
		
		std::cout << "Введите размер генеральной совокупности N (по умолчанию 100): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				N = std::stoi(input);
				if (N <= 0) {
					std::cerr << "N должен быть положительным, используется 100\n";
					N = 100;
				}
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 100\n";
			}
		}
		
		std::cout << "Введите количество успешных элементов K (по умолчанию 30): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				K = std::stoi(input);
				if (K < 0 || K > N) {
					std::cerr << "K должен быть в диапазоне [0, N], используется 30\n";
					K = 30;
				}
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 30\n";
			}
		}
		
		std::cout << "Введите размер выборки n (по умолчанию 20): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				n = std::stoi(input);
				if (n < 0 || n > N) {
					std::cerr << "n должен быть в диапазоне [0, N], используется 20\n";
					n = 20;
				}
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 20\n";
			}
		}
		
		Sample sample = generateHypergeometricSample(sampleSize, N, K, n, seed);
		std::cout << "Параметры: N = " << N << ", K = " << K << ", n = " << n << "\n";
		std::cout << sample.toString() << "\n";
	} catch (const std::exception& ex) {
		std::cerr << "Ошибка: " << ex.what() << "\n\n";
	}
	
	// e. Двойное пуассоновское распределение
	std::cout << "=== e. Двойное пуассоновское распределение ===\n";
	try {
		double lambda1 = 2.0;
		double lambda2 = 5.0;
		double p = 0.6;
		
		std::cout << "Введите параметр lambda1 (по умолчанию 2.0): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				lambda1 = std::stod(input);
				if (lambda1 <= 0) {
					std::cerr << "Lambda1 должен быть положительным, используется 2.0\n";
					lambda1 = 2.0;
				}
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 2.0\n";
			}
		}
		
		std::cout << "Введите параметр lambda2 (по умолчанию 5.0): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				lambda2 = std::stod(input);
				if (lambda2 <= 0) {
					std::cerr << "Lambda2 должен быть положительным, используется 5.0\n";
					lambda2 = 5.0;
				}
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 5.0\n";
			}
		}
		
		std::cout << "Введите вероятность выбора первого распределения p (по умолчанию 0.6): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				p = std::stod(input);
				if (p < 0 || p > 1) {
					std::cerr << "Вероятность должна быть в диапазоне [0, 1], используется 0.6\n";
					p = 0.6;
				}
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 0.6\n";
			}
		}
		
		Sample sample = generateDoublePoissonSample(sampleSize, lambda1, lambda2, p, seed);
		std::cout << "Параметры: λ1 = " << lambda1 << ", λ2 = " << lambda2 << ", p = " << p << "\n";
		std::cout << sample.toString() << "\n";
	} catch (const std::exception& ex) {
		std::cerr << "Ошибка: " << ex.what() << "\n\n";
	}
	
	// f. Двойное геометрическое распределение
	std::cout << "=== f. Двойное геометрическое распределение ===\n";
	try {
		double p1 = 0.3;
		double p2 = 0.5;
		double q = 0.4;
		
		std::cout << "Введите вероятность успеха p1 (по умолчанию 0.3): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				p1 = std::stod(input);
				if (p1 <= 0 || p1 > 1) {
					std::cerr << "Вероятность должна быть в диапазоне (0, 1], используется 0.3\n";
					p1 = 0.3;
				}
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 0.3\n";
			}
		}
		
		std::cout << "Введите вероятность успеха p2 (по умолчанию 0.5): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				p2 = std::stod(input);
				if (p2 <= 0 || p2 > 1) {
					std::cerr << "Вероятность должна быть в диапазоне (0, 1], используется 0.5\n";
					p2 = 0.5;
				}
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 0.5\n";
			}
		}
		
		std::cout << "Введите вероятность выбора первого распределения q (по умолчанию 0.4): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				q = std::stod(input);
				if (q < 0 || q > 1) {
					std::cerr << "Вероятность должна быть в диапазоне [0, 1], используется 0.4\n";
					q = 0.4;
				}
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 0.4\n";
			}
		}
		
		Sample sample = generateDoubleGeometricSample(sampleSize, p1, p2, q, seed);
		std::cout << "Параметры: p1 = " << p1 << ", p2 = " << p2 << ", q = " << q << "\n";
		std::cout << sample.toString() << "\n";
	} catch (const std::exception& ex) {
		std::cerr << "Ошибка: " << ex.what() << "\n\n";
	}
	
	std::cout << "\n=== Задание 5 завершено ===\n";
}

void demonstrateTask6() {
	std::cout << "=== Задание 6: Загрузка выборки из текстового файла ===\n\n";
	
	std::cout << "Формат файла:\n";
	std::cout << "  - Каждая строка содержит: значение количество\n";
	std::cout << "  - Разделитель: пробел или табуляция\n";
	std::cout << "  - Пустые строки и строки, начинающиеся с #, игнорируются\n";
	std::cout << "  - Пример:\n";
	std::cout << "    1.5  10\n";
	std::cout << "    2.0  5\n";
	std::cout << "    3.5  8\n\n";
	
	std::string filename;
	std::cout << "Введите путь к файлу: ";
	std::string input;
	std::getline(std::cin, input);
	if (input.empty()) {
		std::cerr << "Путь к файлу не может быть пустым\n";
		return;
	}
	filename = input;
	
	// Параметры валидации
	bool requireUniqueValues = true;
	bool requireNonNegativeValues = false;
	bool requireNonNegativeCounts = true;
	
	std::cout << "\nПараметры валидации:\n";
	std::cout << "Требовать уникальность значений xi (y/n, по умолчанию y): ";
	std::getline(std::cin, input);
	if (!input.empty() && (input[0] == 'n' || input[0] == 'N')) {
		requireUniqueValues = false;
	}
	
	std::cout << "Требовать неотрицательность значений xi (y/n, по умолчанию n): ";
	std::getline(std::cin, input);
	if (!input.empty() && (input[0] == 'y' || input[0] == 'Y')) {
		requireNonNegativeValues = true;
	}
	
	std::cout << "Требовать неотрицательность количеств xj (y/n, по умолчанию y): ";
	std::getline(std::cin, input);
	if (!input.empty() && (input[0] == 'n' || input[0] == 'N')) {
		requireNonNegativeCounts = false;
	}
	
	std::cout << "\n=== Загрузка выборки ===\n";
	try {
		Sample sample = loadSampleFromFile(filename, requireUniqueValues, 
		                                  requireNonNegativeValues, requireNonNegativeCounts);
		
		std::cout << "Выборка успешно загружена!\n\n";
		std::cout << sample.toString() << "\n";
		
		// Дополнительная информация
		std::cout << "=== Дополнительная информация ===\n";
		std::cout << "Количество уникальных значений: " << sample.data.size() << "\n";
		std::cout << "Общий размер выборки: " << sample.getTotalSize() << "\n";
		
		// Статистика
		if (!sample.data.empty()) {
			double minValue = sample.data[0].first;
			double maxValue = sample.data[0].first;
			int maxCount = sample.data[0].second;
			int minCount = sample.data[0].second;
			
			for (const auto& pair : sample.data) {
				if (pair.first < minValue) minValue = pair.first;
				if (pair.first > maxValue) maxValue = pair.first;
				if (pair.second > maxCount) maxCount = pair.second;
				if (pair.second < minCount) minCount = pair.second;
			}
			
			std::cout << "Минимальное значение: " << minValue << "\n";
			std::cout << "Максимальное значение: " << maxValue << "\n";
			std::cout << "Минимальная частота: " << minCount << "\n";
			std::cout << "Максимальная частота: " << maxCount << "\n";
		}
		
	} catch (const std::exception& ex) {
		std::cerr << "Ошибка при загрузке выборки: " << ex.what() << "\n";
		std::cerr << "\nПроверьте:\n";
		std::cerr << "  1. Существует ли файл по указанному пути\n";
		std::cerr << "  2. Корректность формата файла\n";
		std::cerr << "  3. Соответствие данных требованиям валидации\n";
	}
	
	std::cout << "\n=== Задание 6 завершено ===\n";
}

void demonstrateTask7() {
	std::cout << "=== Задание 7: Вычисление статистических оценок для выборок ===\n\n";
	
	std::cout << "Выберите источник выборки:\n";
	std::cout << "1 - Сгенерировать новую выборку (задание 5)\n";
	std::cout << "2 - Загрузить из файла (задание 6)\n";
	std::cout << "Ваш выбор: ";
	
	int choice;
	std::string input;
	std::getline(std::cin, input);
	if (input.empty() || !(std::istringstream(input) >> choice)) {
		std::cerr << "Некорректный ввод\n";
		return;
	}
	
	Sample sample;
	
	if (choice == 1) {
		// Генерация выборки
		std::cout << "\n=== Генерация выборки ===\n";
		std::cout << "Выберите тип распределения:\n";
		std::cout << "1 - Нормальное\n";
		std::cout << "2 - Пуассоновское\n";
		std::cout << "3 - Геометрическое\n";
		std::cout << "4 - Гипергеометрическое\n";
		std::cout << "5 - Двойное пуассоновское\n";
		std::cout << "6 - Двойное геометрическое\n";
		std::cout << "Ваш выбор: ";
		
		std::getline(std::cin, input);
		int distChoice;
		if (input.empty() || !(std::istringstream(input) >> distChoice)) {
			std::cerr << "Некорректный ввод\n";
			return;
		}
		
		int size = 100;
		unsigned int seed = 0;
		
		std::cout << "Введите размер выборки (по умолчанию 100): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				size = std::stoi(input);
				if (size <= 0) {
					std::cerr << "Размер должен быть положительным, используется 100\n";
					size = 100;
				}
			} catch (...) {
				std::cerr << "Некорректный ввод, используется 100\n";
			}
		}
		
		try {
			switch (distChoice) {
				case 1: {
					double mean = 0.0, stddev = 1.0;
					std::cout << "Введите математическое ожидание (по умолчанию 0.0): ";
					std::getline(std::cin, input);
					if (!input.empty()) mean = std::stod(input);
					std::cout << "Введите среднеквадратическое отклонение (по умолчанию 1.0): ";
					std::getline(std::cin, input);
					if (!input.empty()) {
						stddev = std::stod(input);
						if (stddev <= 0) stddev = 1.0;
					}
					sample = generateNormalSample(size, mean, stddev, seed);
					std::cout << "Сгенерирована выборка нормального распределения (μ=" << mean << ", σ=" << stddev << ")\n";
					break;
				}
				case 2: {
					double lambda = 3.0;
					std::cout << "Введите параметр lambda (по умолчанию 3.0): ";
					std::getline(std::cin, input);
					if (!input.empty()) {
						lambda = std::stod(input);
						if (lambda <= 0) lambda = 3.0;
					}
					sample = generatePoissonSample(size, lambda, seed);
					std::cout << "Сгенерирована выборка пуассоновского распределения (λ=" << lambda << ")\n";
					break;
				}
				case 3: {
					double p = 0.3;
					std::cout << "Введите вероятность успеха p (по умолчанию 0.3): ";
					std::getline(std::cin, input);
					if (!input.empty()) {
						p = std::stod(input);
						if (p <= 0 || p > 1) p = 0.3;
					}
					sample = generateGeometricSample(size, p, seed);
					std::cout << "Сгенерирована выборка геометрического распределения (p=" << p << ")\n";
					break;
				}
				case 4: {
					int N = 100, K = 30, n = 20;
					std::cout << "Введите N (по умолчанию 100): ";
					std::getline(std::cin, input);
					if (!input.empty()) N = std::stoi(input);
					std::cout << "Введите K (по умолчанию 30): ";
					std::getline(std::cin, input);
					if (!input.empty()) K = std::stoi(input);
					std::cout << "Введите n (по умолчанию 20): ";
					std::getline(std::cin, input);
					if (!input.empty()) n = std::stoi(input);
					sample = generateHypergeometricSample(size, N, K, n, seed);
					std::cout << "Сгенерирована выборка гипергеометрического распределения (N=" << N << ", K=" << K << ", n=" << n << ")\n";
					break;
				}
				case 5: {
					double lambda1 = 2.0, lambda2 = 5.0, p = 0.6;
					std::cout << "Введите lambda1 (по умолчанию 2.0): ";
					std::getline(std::cin, input);
					if (!input.empty()) lambda1 = std::stod(input);
					std::cout << "Введите lambda2 (по умолчанию 5.0): ";
					std::getline(std::cin, input);
					if (!input.empty()) lambda2 = std::stod(input);
					std::cout << "Введите p (по умолчанию 0.6): ";
					std::getline(std::cin, input);
					if (!input.empty()) p = std::stod(input);
					sample = generateDoublePoissonSample(size, lambda1, lambda2, p, seed);
					std::cout << "Сгенерирована выборка двойного пуассоновского распределения\n";
					break;
				}
				case 6: {
					double p1 = 0.3, p2 = 0.5, q = 0.4;
					std::cout << "Введите p1 (по умолчанию 0.3): ";
					std::getline(std::cin, input);
					if (!input.empty()) p1 = std::stod(input);
					std::cout << "Введите p2 (по умолчанию 0.5): ";
					std::getline(std::cin, input);
					if (!input.empty()) p2 = std::stod(input);
					std::cout << "Введите q (по умолчанию 0.4): ";
					std::getline(std::cin, input);
					if (!input.empty()) q = std::stod(input);
					sample = generateDoubleGeometricSample(size, p1, p2, q, seed);
					std::cout << "Сгенерирована выборка двойного геометрического распределения\n";
					break;
				}
				default:
					std::cerr << "Неизвестный тип распределения\n";
					return;
			}
		} catch (const std::exception& ex) {
			std::cerr << "Ошибка при генерации выборки: " << ex.what() << "\n";
			return;
		}
	} else if (choice == 2) {
		// Загрузка из файла
		std::cout << "\n=== Загрузка выборки из файла ===\n";
		std::cout << "Введите путь к файлу: ";
		std::getline(std::cin, input);
		if (input.empty()) {
			std::cerr << "Путь к файлу не может быть пустым\n";
			return;
		}
		
		try {
			sample = loadSampleFromFile(input, true, false, true);
			std::cout << "Выборка успешно загружена из файла: " << input << "\n";
		} catch (const std::exception& ex) {
			std::cerr << "Ошибка при загрузке выборки: " << ex.what() << "\n";
			return;
		}
	} else {
		std::cerr << "Неизвестный выбор\n";
		return;
	}
	
	// Вычисление статистических оценок
	std::cout << "\n=== Статистические оценки ===\n";
	std::cout << "Размер выборки: " << sample.getTotalSize() << "\n";
	std::cout << "Количество уникальных значений: " << sample.data.size() << "\n\n";
	
	try {
		// Несмещённые оценки
		std::cout << "=== Несмещённые оценки ===\n";
		double mean = sampleMean(sample);
		std::cout << std::fixed << std::setprecision(10);
		std::cout << "Выборочное среднее (x̄): " << mean << "\n";
		
		double correctedVar = correctedVariance(sample);
		std::cout << "Исправленная дисперсия (s²): " << correctedVar << "\n";
		std::cout << "Исправленное СКО (s): " << correctedStandardDeviation(sample) << "\n\n";
		
		// Смещённые оценки
		std::cout << "=== Смещённые оценки ===\n";
		double biasedVar = sampleVariance(sample);
		std::cout << "Выборочная дисперсия (D): " << biasedVar << "\n";
		std::cout << "Выборочное СКО (σ): " << sampleStandardDeviation(sample) << "\n\n";
		
		// Сравнение
		std::cout << "=== Сравнение оценок ===\n";
		std::cout << "Разница между дисперсиями: " << std::scientific << std::setprecision(6) 
		          << std::abs(correctedVar - biasedVar) << "\n";
		std::cout << "Отношение исправленной к выборочной: " << std::fixed << std::setprecision(6)
		          << (sample.getTotalSize() > 1 ? (double)sample.getTotalSize() / (sample.getTotalSize() - 1) : 1.0) << "\n";
		
		// Теоретическое соотношение: s² = (n/(n-1)) * D
		int n = sample.getTotalSize();
		if (n > 1) {
			double theoreticalRatio = (double)n / (n - 1);
			double actualRatio = correctedVar / biasedVar;
			std::cout << "Теоретическое соотношение n/(n-1): " << theoreticalRatio << "\n";
			std::cout << "Фактическое соотношение s²/D: " << actualRatio << "\n";
			std::cout << "Разница: " << std::scientific << std::setprecision(6) 
			          << std::abs(theoreticalRatio - actualRatio) << "\n";
		}
		
	} catch (const std::exception& ex) {
		std::cerr << "Ошибка при вычислении оценок: " << ex.what() << "\n";
	}
	
	std::cout << "\n=== Задание 7 завершено ===\n";
}

void demonstrateTask8() {
	std::cout << "=== Задание 8: Проверка гипотез о распределении ===\n\n";
	
	std::cout << "Выберите источник выборки:\n";
	std::cout << "1 - Сгенерировать новую выборку (задание 5)\n";
	std::cout << "2 - Загрузить из файла (задание 6)\n";
	std::cout << "Ваш выбор: ";
	
	int choice;
	std::string input;
	std::getline(std::cin, input);
	if (input.empty() || !(std::istringstream(input) >> choice)) {
		std::cerr << "Некорректный ввод\n";
		return;
	}
	
	Sample sample;
	
	if (choice == 1) {
		// Генерация выборки (упрощенная версия)
		std::cout << "\n=== Генерация выборки ===\n";
		std::cout << "Выберите тип распределения:\n";
		std::cout << "1 - Пуассоновское\n";
		std::cout << "2 - Геометрическое\n";
		std::cout << "Ваш выбор: ";
		
		std::getline(std::cin, input);
		int distChoice;
		if (input.empty() || !(std::istringstream(input) >> distChoice)) {
			std::cerr << "Некорректный ввод\n";
			return;
		}
		
		int size = 100;
		unsigned int seed = 0;
		
		std::cout << "Введите размер выборки (по умолчанию 100): ";
		std::getline(std::cin, input);
		if (!input.empty()) {
			try {
				size = std::stoi(input);
				if (size <= 0) size = 100;
			} catch (...) {
				size = 100;
			}
		}
		
		try {
			if (distChoice == 1) {
				double lambda = 3.0;
				std::cout << "Введите lambda (по умолчанию 3.0): ";
				std::getline(std::cin, input);
				if (!input.empty()) lambda = std::stod(input);
				sample = generatePoissonSample(size, lambda, seed);
			} else if (distChoice == 2) {
				double p = 0.3;
				std::cout << "Введите p (по умолчанию 0.3): ";
				std::getline(std::cin, input);
				if (!input.empty()) p = std::stod(input);
				sample = generateGeometricSample(size, p, seed);
			} else {
				std::cerr << "Неизвестный тип распределения\n";
				return;
			}
		} catch (const std::exception& ex) {
			std::cerr << "Ошибка при генерации: " << ex.what() << "\n";
			return;
		}
	} else if (choice == 2) {
		std::cout << "\n=== Загрузка выборки из файла ===\n";
		std::cout << "Введите путь к файлу: ";
		std::getline(std::cin, input);
		if (input.empty()) {
			std::cerr << "Путь к файлу не может быть пустым\n";
			return;
		}
		
		try {
			sample = loadSampleFromFile(input, true, false, true);
		} catch (const std::exception& ex) {
			std::cerr << "Ошибка при загрузке: " << ex.what() << "\n";
			return;
		}
	} else {
		std::cerr << "Неизвестный выбор\n";
		return;
	}
	
	std::cout << "\nВыборка загружена. Размер: " << sample.getTotalSize() << "\n";
	
	// Выбор типа распределения для проверки
	std::cout << "\n=== Выбор распределения для проверки гипотезы ===\n";
	std::cout << "1 - Нормальное\n";
	std::cout << "2 - Пуассоновское\n";
	std::cout << "3 - Геометрическое\n";
	std::cout << "4 - Гипергеометрическое\n";
	std::cout << "5 - Двойное пуассоновское\n";
	std::cout << "6 - Двойное геометрическое\n";
	std::cout << "Ваш выбор: ";
	
	std::getline(std::cin, input);
	int testChoice;
	if (input.empty() || !(std::istringstream(input) >> testChoice)) {
		std::cerr << "Некорректный ввод\n";
		return;
	}
	
	double significanceLevel = 0.05;
	std::cout << "Введите уровень значимости (по умолчанию 0.05): ";
	std::getline(std::cin, input);
	if (!input.empty()) {
		try {
			significanceLevel = std::stod(input);
			if (significanceLevel <= 0 || significanceLevel >= 1) {
				significanceLevel = 0.05;
			}
		} catch (...) {
			significanceLevel = 0.05;
		}
	}
	
	try {
		HypothesisTestResult result;
		
		switch (testChoice) {
			case 1: {
				result = testNormalDistribution(sample, significanceLevel);
				std::cout << "\n=== Проверка гипотезы о нормальном распределении ===\n";
				break;
			}
			case 2: {
				result = testPoissonDistribution(sample, significanceLevel);
				std::cout << "\n=== Проверка гипотезы о пуассоновском распределении ===\n";
				break;
			}
			case 3: {
				result = testGeometricDistribution(sample, significanceLevel);
				std::cout << "\n=== Проверка гипотезы о геометрическом распределении ===\n";
				break;
			}
			case 4: {
				int N = 100, K = 30, n = 20;
				std::cout << "Введите N (по умолчанию 100): ";
				std::getline(std::cin, input);
				if (!input.empty()) N = std::stoi(input);
				std::cout << "Введите K (по умолчанию 30): ";
				std::getline(std::cin, input);
				if (!input.empty()) K = std::stoi(input);
				std::cout << "Введите n (по умолчанию 20): ";
				std::getline(std::cin, input);
				if (!input.empty()) n = std::stoi(input);
				result = testHypergeometricDistribution(sample, N, K, n, significanceLevel);
				std::cout << "\n=== Проверка гипотезы о гипергеометрическом распределении ===\n";
				std::cout << "Параметры: N=" << N << ", K=" << K << ", n=" << n << "\n";
				break;
			}
			case 5: {
				double lambda1 = 2.0, lambda2 = 5.0, p = 0.6;
				std::cout << "Введите lambda1 (по умолчанию 2.0): ";
				std::getline(std::cin, input);
				if (!input.empty()) lambda1 = std::stod(input);
				std::cout << "Введите lambda2 (по умолчанию 5.0): ";
				std::getline(std::cin, input);
				if (!input.empty()) lambda2 = std::stod(input);
				std::cout << "Введите p (по умолчанию 0.6): ";
				std::getline(std::cin, input);
				if (!input.empty()) p = std::stod(input);
				result = testDoublePoissonDistribution(sample, lambda1, lambda2, p, significanceLevel);
				std::cout << "\n=== Проверка гипотезы о двойном пуассоновском распределении ===\n";
				std::cout << "Параметры: λ1=" << lambda1 << ", λ2=" << lambda2 << ", p=" << p << "\n";
				break;
			}
			case 6: {
				double p1 = 0.3, p2 = 0.5, q = 0.4;
				std::cout << "Введите p1 (по умолчанию 0.3): ";
				std::getline(std::cin, input);
				if (!input.empty()) p1 = std::stod(input);
				std::cout << "Введите p2 (по умолчанию 0.5): ";
				std::getline(std::cin, input);
				if (!input.empty()) p2 = std::stod(input);
				std::cout << "Введите q (по умолчанию 0.4): ";
				std::getline(std::cin, input);
				if (!input.empty()) q = std::stod(input);
				result = testDoubleGeometricDistribution(sample, p1, p2, q, significanceLevel);
				std::cout << "\n=== Проверка гипотезы о двойном геометрическом распределении ===\n";
				std::cout << "Параметры: p1=" << p1 << ", p2=" << p2 << ", q=" << q << "\n";
				break;
			}
			default:
				std::cerr << "Неизвестный тип распределения\n";
				return;
		}
		
		std::cout << result.toString() << "\n";
		
		// Дополнительная информация
		std::cout << "=== Интерпретация результата ===\n";
		if (result.hypothesisAccepted) {
			std::cout << "Гипотеза ПРИНЯТА: данные согласуются с предполагаемым распределением.\n";
		} else {
			std::cout << "Гипотеза ОТКЛОНЕНА: данные НЕ согласуются с предполагаемым распределением.\n";
		}
		std::cout << "p-value = " << std::scientific << std::setprecision(6) << result.pValue;
		if (result.pValue < significanceLevel) {
			std::cout << " < " << significanceLevel << " (уровень значимости) - гипотеза отклонена\n";
		} else {
			std::cout << " >= " << significanceLevel << " (уровень значимости) - гипотеза принята\n";
		}
		
	} catch (const std::exception& ex) {
		std::cerr << "Ошибка при проверке гипотезы: " << ex.what() << "\n";
	}
	
	std::cout << "\n=== Задание 8 завершено ===\n";
}

int main(int argc, char** argv) {
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	// Парсинг аргументов командной строки
	if (argc < 2) {
		std::cerr << "Использование: " << argv[0] << " <номер_задания> [опции]\n";
		std::cerr << "Примеры:\n";
		std::cerr << "  " << argv[0] << " 1.1  - Демонстрация функционала DiscreteRandomVariable\n";
		std::cerr << "  " << argv[0] << " 1.2  - Визуализация ДСВ через OpenGL\n";
		std::cerr << "  " << argv[0] << " 2    - Моделирование случайного блуждания\n";
		std::cerr << "  " << argv[0] << " 3    - Вычисление интегральной функции Лапласа\n";
		std::cerr << "  " << argv[0] << " 4    - Поиск аргумента по значению функции Лапласа\n";
		std::cerr << "  " << argv[0] << " 5    - Генерация выборок для различных распределений\n";
		std::cerr << "  " << argv[0] << " 6    - Загрузка выборки из текстового файла\n";
		std::cerr << "  " << argv[0] << " 7    - Вычисление статистических оценок для выборок\n";
		std::cerr << "  " << argv[0] << " 8    - Проверка гипотез о распределении\n";
		return -1;
	}

	std::string taskNumber = argv[1];

	if (taskNumber == "1.1") {
		demonstrateTask1_1();
	} else if (taskNumber == "1.2") {
		demonstrateTask1_2();
	} else if (taskNumber == "2") {
		demonstrateTask2();
	} else if (taskNumber == "3") {
		demonstrateTask3();
	} else if (taskNumber == "4") {
		demonstrateTask4();
	} else if (taskNumber == "5") {
		demonstrateTask5();
	} else if (taskNumber == "6") {
		demonstrateTask6();
	} else if (taskNumber == "7") {
		demonstrateTask7();
	} else if (taskNumber == "8") {
		demonstrateTask8();
	} else {
		std::cerr << "Неизвестный номер задания: " << taskNumber << "\n";
		std::cerr << "Доступные задания: 1.1, 1.2, 2, 3, 4, 5, 6, 7, 8\n";
		return -1;
	}

	return 0;
}  