"""
ЛР3: аппроксимация экспериментальных данных различными функциями.
"""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Callable

import matplotlib.pyplot as plt
import numpy as np

OUTPUT_DIR = Path("output/lr3")

X_DATA = np.array([0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5])
Y_DATA = np.array([1.10, 1.48, 2.20, 3.05, 4.10, 5.75, 7.90, 10.60])


@dataclass
class ModelFit:
    name: str
    formula: str
    coeffs_text: str
    predict: Callable[[np.ndarray], np.ndarray]
    y_hat: np.ndarray
    sse: float
    rmse: float


def _plot_path(filename: str) -> Path:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    return OUTPUT_DIR / filename


def _lstsq(design: np.ndarray, target: np.ndarray) -> np.ndarray:
    coeffs, _, _, _ = np.linalg.lstsq(design, target, rcond=None)
    return coeffs


def _metrics(y: np.ndarray, y_hat: np.ndarray) -> tuple[float, float]:
    residuals = y - y_hat
    sse = float(np.sum(residuals**2))
    rmse = float(np.sqrt(sse / len(y)))
    return sse, rmse


def _fit_linear(x: np.ndarray, y: np.ndarray) -> ModelFit:
    a, b = np.polyfit(x, y, 1)
    predict = lambda xs, a=a, b=b: a * xs + b
    y_hat = predict(x)
    sse, rmse = _metrics(y, y_hat)
    return ModelFit(
        name="Линейная функция",
        formula="y = ax + b",
        coeffs_text=f"a = {a:.6f}, b = {b:.6f}",
        predict=predict,
        y_hat=y_hat,
        sse=sse,
        rmse=rmse,
    )


def _fit_quadratic(x: np.ndarray, y: np.ndarray) -> ModelFit:
    a, b, c = np.polyfit(x, y, 2)
    predict = lambda xs, a=a, b=b, c=c: a * xs**2 + b * xs + c
    y_hat = predict(x)
    sse, rmse = _metrics(y, y_hat)
    return ModelFit(
        name="Квадратичная функция",
        formula="y = ax^2 + bx + c",
        coeffs_text=f"a = {a:.6f}, b = {b:.6f}, c = {c:.6f}",
        predict=predict,
        y_hat=y_hat,
        sse=sse,
        rmse=rmse,
    )


def _fit_logarithmic(x: np.ndarray, y: np.ndarray) -> ModelFit:
    t = np.log(x + 1.0)
    a, b = _lstsq(np.column_stack([t, np.ones_like(x)]), y)
    predict = lambda xs, a=a, b=b: a * np.log(xs + 1.0) + b
    y_hat = predict(x)
    sse, rmse = _metrics(y, y_hat)
    return ModelFit(
        name="Логарифмическая функция",
        formula="y = a ln(x + 1) + b",
        coeffs_text=f"a = {a:.6f}, b = {b:.6f}",
        predict=predict,
        y_hat=y_hat,
        sse=sse,
        rmse=rmse,
    )


def _fit_power(x: np.ndarray, y: np.ndarray) -> ModelFit:
    t = np.log(x + 1.0)
    ln_y = np.log(y)
    b, ln_a = _lstsq(np.column_stack([t, np.ones_like(x)]), ln_y)
    a = float(np.exp(ln_a))
    predict = lambda xs, a=a, b=b: a * (xs + 1.0) ** b
    y_hat = predict(x)
    sse, rmse = _metrics(y, y_hat)
    return ModelFit(
        name="Степенная функция",
        formula="y = a (x + 1)^b",
        coeffs_text=f"a = {a:.6f}, b = {b:.6f}",
        predict=predict,
        y_hat=y_hat,
        sse=sse,
        rmse=rmse,
    )


def _fit_exponential(x: np.ndarray, y: np.ndarray) -> ModelFit:
    ln_y = np.log(y)
    b, ln_a = _lstsq(np.column_stack([x, np.ones_like(x)]), ln_y)
    a = float(np.exp(ln_a))
    predict = lambda xs, a=a, b=b: a * np.exp(b * xs)
    y_hat = predict(x)
    sse, rmse = _metrics(y, y_hat)
    return ModelFit(
        name="Экспоненциальная функция",
        formula="y = a e^(bx)",
        coeffs_text=f"a = {a:.6f}, b = {b:.6f}",
        predict=predict,
        y_hat=y_hat,
        sse=sse,
        rmse=rmse,
    )


def _fit_trigonometric(x: np.ndarray, y: np.ndarray) -> ModelFit:
    a, b, c = _lstsq(
        np.column_stack([np.sin(x), np.cos(x), np.ones_like(x)]),
        y,
    )
    predict = lambda xs, a=a, b=b, c=c: a * np.sin(xs) + b * np.cos(xs) + c
    y_hat = predict(x)
    sse, rmse = _metrics(y, y_hat)
    return ModelFit(
        name="Тригонометрическая функция",
        formula="y = a sin(x) + b cos(x) + c",
        coeffs_text=f"a = {a:.6f}, b = {b:.6f}, c = {c:.6f}",
        predict=predict,
        y_hat=y_hat,
        sse=sse,
        rmse=rmse,
    )


def _fit_all_models(x: np.ndarray, y: np.ndarray) -> list[ModelFit]:
    return [
        _fit_linear(x, y),
        _fit_quadratic(x, y),
        _fit_logarithmic(x, y),
        _fit_power(x, y),
        _fit_exponential(x, y),
        _fit_trigonometric(x, y),
    ]


def _print_results_table(models: list[ModelFit]) -> None:
    print("\nТаблица коэффициентов и ошибок:")
    header = f"{'Модель':<28} {'Вид функции':<22} {'Коэффициенты':<40} {'SSE':>12} {'RMSE':>10}"
    print(header)
    print("-" * len(header))
    for m in models:
        print(
            f"{m.name:<28} {m.formula:<22} {m.coeffs_text:<40} "
            f"{m.sse:12.6f} {m.rmse:10.6f}"
        )

    print("\nПриближенные значения y на исходных узлах:")
    col_w = 12
    header = f"{'x':>6}" + "".join(f"{m.name[:col_w]:>{col_w}}" for m in models) + f"{'y (исх.)':>{col_w}}"
    print(header)
    print("-" * len(header))
    for i, xi in enumerate(X_DATA):
        row = f"{xi:6.1f}"
        for m in models:
            row += f"{m.y_hat[i]:{col_w}.4f}"
        row += f"{Y_DATA[i]:{col_w}.4f}"
        print(row)


def _plot_all_models(x: np.ndarray, y: np.ndarray, models: list[ModelFit]) -> Path:
    x_fine = np.linspace(float(x.min()), float(x.max()), 400)
    colors = ["tab:blue", "tab:orange", "tab:green", "tab:red", "tab:purple", "tab:brown"]

    plt.figure(figsize=(11, 7))
    plt.scatter(x, y, s=80, c="black", zorder=5, label="Исходные данные")

    for model, color in zip(models, colors):
        plt.plot(x_fine, model.predict(x_fine), color=color, linewidth=1.8, label=model.name)

    plt.xlabel("x")
    plt.ylabel("y")
    plt.title("ЛР3: аппроксимация экспериментальных данных")
    plt.legend(loc="upper left", fontsize=9)
    plt.grid(True, alpha=0.35)
    plt.tight_layout()

    path = _plot_path("lr3_approximation_comparison.png")
    plt.savefig(path, dpi=150)
    plt.close()
    return path


def _print_conclusion(models: list[ModelFit]) -> None:
    best = min(models, key=lambda m: m.sse)
    worst = max(models, key=lambda m: m.sse)

    print("\n" + "=" * 70)
    print("ВЫВОД")
    print("=" * 70)
    print(
        f"Лучшая модель по SSE и RMSE: {best.name} "
        f"(SSE = {best.sse:.6f}, RMSE = {best.rmse:.6f})."
    )
    print(
        f"Наихудшая из рассмотренных: {worst.name} "
        f"(SSE = {worst.sse:.6f}, RMSE = {worst.rmse:.6f})."
    )
    ranked = sorted(models, key=lambda m: m.sse)
    print("Чем меньше SSE и RMSE, тем точнее модель описывает экспериментальные точки.")
    print("Рейтинг моделей (от лучшей к худшей):")
    for i, m in enumerate(ranked, 1):
        print(f"  {i}. {m.name}: SSE = {m.sse:.6f}, RMSE = {m.rmse:.6f}")


def run() -> None:
    """Запуск ЛР3 по заданию из meta/ЛР 3.pdf."""
    x, y = X_DATA, Y_DATA

    print("=" * 70)
    print("ЛР3. Аппроксимация экспериментальных данных")
    print("=" * 70)
    print("Исходная таблица:")
    print(f"  x: {', '.join(f'{v:.1f}' for v in x)}")
    print(f"  y: {', '.join(f'{v:.2f}' for v in y)}")

    models = _fit_all_models(x, y)
    _print_results_table(models)

    plot_path = _plot_all_models(x, y, models)
    print(f"\nГрафик: {plot_path}")

    _print_conclusion(models)
    print(f"\nГрафики сохранены в {OUTPUT_DIR.resolve()} (PNG).")
