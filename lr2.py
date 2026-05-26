"""
ЛР2: y' = x + y, y(0) = 1 на [0, 1], h = 0.2.
Методы: Эйлер, расширенный Эйлер, Рунге–Кутта 4-го порядка.
Точное решение: y(x) = 2*exp(x) - x - 1.
"""

from __future__ import annotations

from pathlib import Path
from typing import Callable

import matplotlib.pyplot as plt
import numpy as np

OUTPUT_DIR = Path("output/lr2")

X0, Y0, X_END, H = 0.0, 1.0, 1.0, 0.2


def f(x: float, y: float) -> float:
    return x + y


def exact_solution(x: np.ndarray | float) -> np.ndarray | float:
    x_arr = np.asarray(x)
    return 2 * np.exp(x_arr) - x_arr - 1


def _grid(x0: float, x_end: float, h: float) -> np.ndarray:
    n_steps = int(round((x_end - x0) / h))
    if n_steps < 1:
        raise ValueError("Шаг h слишком велик для заданного интервала.")
    return np.linspace(x0, x_end, n_steps + 1)


def euler(f_callable: Callable[[float, float], float], x0: float, y0: float, x_end: float, h: float) -> tuple[np.ndarray, np.ndarray]:
    x = _grid(x0, x_end, h)
    y = np.zeros_like(x)
    y[0] = y0
    for n in range(len(x) - 1):
        y[n + 1] = y[n] + h * f_callable(x[n], y[n])
    return x, y


def euler_modified(
    f_callable: Callable[[float, float], float], x0: float, y0: float, x_end: float, h: float
) -> tuple[np.ndarray, np.ndarray]:
    x = _grid(x0, x_end, h)
    y = np.zeros_like(x)
    y[0] = y0
    for n in range(len(x) - 1):
        y_tilde = y[n] + h * f_callable(x[n], y[n])
        y[n + 1] = y[n] + 0.5 * h * (f_callable(x[n], y[n]) + f_callable(x[n + 1], y_tilde))
    return x, y


def runge_kutta4(
    f_callable: Callable[[float, float], float], x0: float, y0: float, x_end: float, h: float
) -> tuple[np.ndarray, np.ndarray]:
    x = _grid(x0, x_end, h)
    y = np.zeros_like(x)
    y[0] = y0
    for n in range(len(x) - 1):
        k1 = h * f_callable(x[n], y[n])
        k2 = h * f_callable(x[n] + 0.5 * h, y[n] + 0.5 * k1)
        k3 = h * f_callable(x[n] + 0.5 * h, y[n] + 0.5 * k2)
        k4 = h * f_callable(x[n] + h, y[n] + k3)
        y[n + 1] = y[n] + (k1 + 2 * k2 + 2 * k3 + k4) / 6
    return x, y


def _plot_path(filename: str) -> Path:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    return OUTPUT_DIR / filename


def _print_values_table(x: np.ndarray, y_exact: np.ndarray, y_euler: np.ndarray, y_mod: np.ndarray, y_rk4: np.ndarray) -> None:
    print("\nТаблица значений y(x):")
    header = f"{'x':>6} {'Точное':>14} {'Эйлер':>14} {'Расш. Эйлер':>14} {'Рунге-Кутта':>14}"
    print(header)
    print("-" * len(header))
    for i in range(len(x)):
        print(
            f"{x[i]:6.1f} {y_exact[i]:14.6f} {y_euler[i]:14.6f} {y_mod[i]:14.6f} {y_rk4[i]:14.6f}"
        )


def _print_errors_table(x: np.ndarray, err_euler: np.ndarray, err_mod: np.ndarray, err_rk4: np.ndarray) -> None:
    print("\nТаблица абсолютных погрешностей:")
    header = f"{'x':>6} {'Эйлер':>14} {'Расш. Эйлер':>14} {'Рунге-Кутта':>14}"
    print(header)
    print("-" * len(header))
    for i in range(len(x)):
        print(f"{x[i]:6.1f} {err_euler[i]:14.6e} {err_mod[i]:14.6e} {err_rk4[i]:14.6e}")


def _plot_comparison(x: np.ndarray, y_exact: np.ndarray, y_euler: np.ndarray, y_mod: np.ndarray, y_rk4: np.ndarray) -> Path:
    x_fine = np.linspace(X0, X_END, 200)
    y_fine = exact_solution(x_fine)

    plt.figure(figsize=(9, 6))
    plt.plot(x_fine, y_fine, "b-", linewidth=2, label="Точное решение")
    plt.plot(x, y_euler, "o--", color="tab:orange", markersize=7, label="Метод Эйлера")
    plt.plot(x, y_mod, "s--", color="tab:green", markersize=7, label="Расширенный метод Эйлера")
    plt.plot(x, y_rk4, "^--", color="tab:red", markersize=7, label="Метод Рунге–Кутты 4")
    plt.xlabel("x")
    plt.ylabel("y")
    plt.title("ЛР2: сравнение численных методов (h = 0.2)")
    plt.legend()
    plt.grid(True, alpha=0.35)
    plt.tight_layout()

    path = _plot_path("lr2_methods_comparison.png")
    plt.savefig(path, dpi=150)
    plt.close()
    return path


def _print_conclusions(err_euler: np.ndarray, err_mod: np.ndarray, err_rk4: np.ndarray) -> None:
    methods = {
        "Метод Эйлера": err_euler,
        "Расширенный метод Эйлера": err_mod,
        "Метод Рунге–Кутты 4": err_rk4,
    }
    max_errors = {name: float(np.max(err)) for name, err in methods.items()}
    best = min(max_errors, key=max_errors.get)
    worst = max(max_errors, key=max_errors.get)

    print("\n" + "=" * 70)
    print("ВЫВОДЫ")
    print("=" * 70)
    print(f"1) Самый точный метод: {best} (макс. погрешность {max_errors[best]:.4e}).")
    print(f"2) Наибольшая погрешность: {worst} (макс. погрешность {max_errors[worst]:.4e}).")
    print(
        "3) Метод Рунге–Кутты 4-го порядка точнее метода Эйлера, потому что на каждом шаге\n"
        "   использует несколько оценок наклона (k1–k4) и комбинирует их с весами,\n"
        "   что соответствует аппроксимации ряда Тейлора до O(h^5) на шаге (глобально O(h^4)),\n"
        "   тогда как обычный Эйлер — лишь O(h^2) на шаге (глобально O(h))."
    )
    print(
        "4) При уменьшении шага h погрешность уменьшается: для Эйлера примерно пропорционально h,\n"
        "   для расширенного Эйлера — примерно h^2, для РК4 — примерно h^4 (на конечном интервале)."
    )

    # Демонстрация влияния шага на примере x = 1
    h_small = 0.1
    _, y_e_h = euler(f, X0, Y0, X_END, h_small)
    _, y_m_h = euler_modified(f, X0, Y0, X_END, h_small)
    _, y_r_h = runge_kutta4(f, X0, Y0, X_END, h_small)
    y_ex_end = float(exact_solution(X_END))
    print("\n   Ошибка в точке x = 1 при h = 0.1 (для сравнения с h = 0.2):")
    print(f"   Эйлер:           {abs(y_e_h[-1] - y_ex_end):.4e}  (было {err_euler[-1]:.4e})")
    print(f"   Расш. Эйлер:     {abs(y_m_h[-1] - y_ex_end):.4e}  (было {err_mod[-1]:.4e})")
    print(f"   Рунге–Кутта 4:   {abs(y_r_h[-1] - y_ex_end):.4e}  (было {err_rk4[-1]:.4e})")


def run() -> None:
    """Запуск ЛР2 по заданию из meta/ЛР 2.pdf."""
    print("=" * 70)
    print("ЛР2. y' = x + y, y(0) = 1, 0 <= x <= 1, h = 0.2")
    print("=" * 70)

    x, y_euler = euler(f, X0, Y0, X_END, H)
    _, y_mod = euler_modified(f, X0, Y0, X_END, H)
    _, y_rk4 = runge_kutta4(f, X0, Y0, X_END, H)
    y_exact = exact_solution(x)

    _print_values_table(x, y_exact, y_euler, y_mod, y_rk4)

    err_euler = np.abs(y_exact - y_euler)
    err_mod = np.abs(y_exact - y_mod)
    err_rk4 = np.abs(y_exact - y_rk4)
    _print_errors_table(x, err_euler, err_mod, err_rk4)

    plot_path = _plot_comparison(x, y_exact, y_euler, y_mod, y_rk4)
    print(f"\nГрафик: {plot_path}")

    _print_conclusions(err_euler, err_mod, err_rk4)
    print(f"\nГрафики сохранены в {OUTPUT_DIR.resolve()} (PNG).")
