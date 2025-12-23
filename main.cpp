#include "visualizer.h"
#include "random_variable.h"
#include "random_walk_visualizer.h"
#include "laplace_function.h"
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
	} else {
		std::cerr << "Неизвестный номер задания: " << taskNumber << "\n";
		std::cerr << "Доступные задания: 1.1, 1.2, 2, 3, 4\n";
		return -1;
	}

	return 0;
}