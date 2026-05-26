"""
ЛР1: численное решение задачи Коши y' = f(t, y) методом Эйлера.
Модель: y' = 0.25*y, y(0) = 80, 0 <= t <= 8.
"""

from __future__ import annotations

from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

OUTPUT_DIR = Path("output/lr1")


def _h_suffix(h: float) -> str:
    return str(h).replace(".", "_")


def _plot_path(filename: str) -> Path:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    return OUTPUT_DIR / filename


def f(t: float, y: float) -> float:
    """Правая часть ОДУ: y' = 0.25 * y."""
    return 0.25 * y


def euler(
    f_callable,
    t0: float,
    y0: float,
    T: float,
    h: float,
) -> tuple[np.ndarray, np.ndarray]:
    """
    Явный метод Эйлера для y' = f(t, y), y(t0) = y0 на [t0, T].

    Возвращает массивы узлов t и приближений y.
    """
    n_steps = int(np.round((T - t0) / h))
    if n_steps < 1:
        raise ValueError("Шаг слишком велик: нужно хотя бы один шаг до T.")

    t = np.zeros(n_steps + 1)
    y = np.zeros(n_steps + 1)
    t[0] = t0
    y[0] = y0

    for k in range(n_steps):
        t[k + 1] = t[k] + h
        y[k + 1] = y[k] + h * f_callable(t[k], y[k])

    return t, y


def exact_solution(t: np.ndarray | float, y0: float = 80.0, lam: float = 0.25) -> np.ndarray | float:
    """Точное решение: y(t) = y0 * exp(lam * t)."""
    return y0 * np.exp(lam * np.asarray(t))


def print_table_edges(t: np.ndarray, y_num: np.ndarray, y_ex: np.ndarray, n_show: int = 5) -> None:
    """Первые и последние n_show строк таблицы (t, y_числ, y_точн, |ошибка|)."""
    err = np.abs(y_num - y_ex)
    print(f"  Первые {n_show} значений (t, y_числ, y_точн, |ошибка|):")
    for i in range(min(n_show, len(t))):
        print(f"    t={t[i]:.6g}, y_num={y_num[i]:.10g}, y_ex={y_ex[i]:.10g}, err={err[i]:.4e}")
    print(f"  Последние {n_show} значений:")
    start = max(0, len(t) - n_show)
    for i in range(start, len(t)):
        print(f"    t={t[i]:.6g}, y_num={y_num[i]:.10g}, y_ex={y_ex[i]:.10g}, err={err[i]:.4e}")


def plot_solution_vs_exact(
    t_num: np.ndarray,
    y_num: np.ndarray,
    T: float,
    y0: float,
    lam: float,
    title: str,
    filepath: Path,
) -> None:
    t_fine = np.linspace(0.0, T, 500)
    y_fine = exact_solution(t_fine, y0, lam)

    plt.figure(figsize=(8, 5))
    plt.plot(t_fine, y_fine, "b-", label="Точное решение", linewidth=2)
    plt.plot(t_num, y_num, "ko", markersize=4, label="Узлы Эйлера")
    plt.plot(t_num, y_num, "k--", linewidth=1, alpha=0.8, label="Ломаная Эйлера")
    plt.xlabel("t")
    plt.ylabel("y")
    plt.title(title)
    plt.legend()
    plt.grid(True, alpha=0.35)
    plt.tight_layout()
    plt.savefig(filepath, dpi=150)
    plt.close()


def plot_error_vs_time(t: np.ndarray, delta: np.ndarray, title: str, filepath: Path) -> None:
    plt.figure(figsize=(8, 5))
    plt.semilogy(t, delta, "r.-", markersize=5)
    plt.xlabel(r"$t_n$")
    plt.ylabel(r"$\Delta_n = |y_n - y(t_n)|$")
    plt.title(title)
    plt.grid(True, which="both", alpha=0.35)
    plt.tight_layout()
    plt.savefig(filepath, dpi=150)
    plt.close()


def _part1() -> None:
    t0, T, y0, lam = 0.0, 8.0, 80.0, 0.25
    y_exact_at_T = float(exact_solution(T, y0, lam))
    steps = [1.0, 0.5, 0.1]

    print("=" * 70)
    print("ЗАДАЧА 1. Метод Эйлера и сравнение с точным решением")
    print("=" * 70)

    for h in steps:
        t, y_num = euler(f, t0, y0, T, h)
        n_steps = len(t) - 1
        y_T_num = float(y_num[-1])
        abs_err_end = abs(y_T_num - y_exact_at_T)
        y_ex_nodes = exact_solution(t, y0, lam)

        print(f"\n--- Шаг h = {h} ---")
        print(f"  Количество шагов N: {n_steps}")
        print(f"  Численное y(T) = {y_T_num:.10g}")
        print(f"  Точное y(8)   = {y_exact_at_T:.10g}")
        print(f"  Абсолютная ошибка в T: {abs_err_end:.4e}")
        print_table_edges(t, y_num, y_ex_nodes)

        plot_path = _plot_path(f"lr1_task1_solution_h_{_h_suffix(h)}.png")
        plot_solution_vs_exact(
            t,
            y_num,
            T,
            y0,
            lam,
            title=f"ЛР1, задача 1: точное и численное решение (h = {h})",
            filepath=plot_path,
        )
        print(f"  График: {plot_path}")


def _part2() -> None:
    t0, T, y0, lam = 0.0, 8.0, 80.0, 0.25
    steps = [1.0, 0.5, 0.1]

    print("\n" + "=" * 70)
    print("ЗАДАЧА 2. Погрешность во времени")
    print("=" * 70)

    for h in steps:
        t, y_num = euler(f, t0, y0, T, h)
        y_ex_nodes = exact_solution(t, y0, lam)
        delta = np.abs(y_num - y_ex_nodes)

        plot_path = _plot_path(f"lr1_task2_error_h_{_h_suffix(h)}.png")
        plot_error_vs_time(
            t,
            delta,
            title=f"ЛР1, задача 2: абсолютная ошибка по времени (h = {h})",
            filepath=plot_path,
        )
        print(f"  h = {h}: график {plot_path}")

    print("\n" + "=" * 70)
    print("ЗАДАЧА 2. Выводы по графикам ошибки")
    print("=" * 70)
    print(
        "1) Ошибка |y_n - y(t_n)| растёт со временем: для линейного ОДУ с положительным\n"
        "   коэффициентом роста решение экспоненциально возрастает, а глобальная\n"
        "   погрешность метода Эйлера накапливается от шага к шагу.\n"
        "2) При большем шаге (h = 1) ошибка нарастает быстрее, чем при h = 0.5 и h = 0.1:\n"
        "   меньше узлов — больше локальная ошибка за шаг и сильнее накопление.\n"
        "3) На интервале [0, 8] метод стабилен в смысле устойчивости к малым возмущениям\n"
        "   начальных данных (для данной скалярной линейной задачи явный Эйлер устойчив\n"
        "   при h*λ < 2, здесь λ = 0.25, то есть h < 8; наши h это удовлетворяют).\n"
        "   Решение не «взрывается» из-за самого метода — растёт вместе с точным решением."
    )


def _part3() -> None:
    t0, T, y0, lam = 0.0, 8.0, 80.0, 0.25
    y_exact_at_T = float(exact_solution(T, y0, lam))
    hs = [1.0, 0.5, 0.25, 0.1, 0.05]

    print("\n" + "=" * 70)
    print("ЗАДАЧА 3. Ошибка в конечной точке и зависимость от шага")
    print("=" * 70)

    N_list: list[int] = []
    err_list: list[float] = []

    print(f"\n{'h':>8} {'N':>8} {'|y_N - y(8)|':>18}")
    print("-" * 40)
    for h in hs:
        t, y_num = euler(f, t0, y0, T, h)
        N = len(t) - 1
        err_end = abs(float(y_num[-1]) - y_exact_at_T)
        N_list.append(N)
        err_list.append(err_end)
        print(f"{h:8.3f} {N:8d} {err_end:18.4e}")

    plot_path = _plot_path("lr1_task3_error_vs_h.png")
    plt.figure(figsize=(8, 5))
    plt.loglog(hs, err_list, "bo-", markersize=8, linewidth=1.5)
    plt.xlabel("Шаг h")
    plt.ylabel("Ошибка в точке T = 8")
    plt.title("ЛР1, задача 3: ошибка в конечной точке vs шаг")
    plt.grid(True, which="both", alpha=0.35)
    plt.gca().invert_xaxis()
    plt.tight_layout()
    plt.savefig(plot_path, dpi=150)
    plt.close()
    print(f"\n  График: {plot_path}")

    print("\n" + "=" * 70)
    print("ЗАДАЧА 3. Анализ")
    print("=" * 70)
    print(
        "1) При уменьшении шага h ошибка в конечной точке убывает (для метода Эйлера\n"
        "   ожидается порядок O(h): при уменьшении h в k раз ошибка примерно в k раз меньше).\n"
        "2) Число шагов N ≈ (T - t0)/h, поэтому при уменьшении h в k раз N примерно в k раз больше\n"
        "   (см. таблицу: например, h: 1 → 0.5 даёт N: 8 → 16).\n"
        "3) Разумный компромисс — например, h = 0.1 или 0.25: ошибка уже мала, а число шагов\n"
        "   не чрезмерно велико для учебной постановки; h = 1 даёт заметную погрешность.\n"
        "4) Очень малый шаг увеличивает число операций и время счёта, усиливает влияние\n"
        "   ошибок округления (накопление машинной арифметики), а выигрыш в точности\n"
        "   на практике ограничен; для жёстких задач малый явный шаг может давать неустойчивость."
    )

    if len(N_list) >= 2:
        ratios = [N_list[i] / N_list[i - 1] for i in range(1, len(N_list))]
        print(f"\nОтношения N_i / N_(i-1) при последовательном уменьшении h: {[round(r, 4) for r in ratios]}")


def run() -> None:
    """Запуск ЛР1: задачи 1–3 из «Задачи 1.pdf»."""
    _part1()
    _part2()
    _part3()
    print(f"\nГрафики сохранены в {OUTPUT_DIR.resolve()} (PNG).")
