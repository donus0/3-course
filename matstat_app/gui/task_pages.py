"""Страницы заданий 1–11: расчёты из math.compute и графики matplotlib."""

from __future__ import annotations

import math
from typing import Any, Optional

import matplotlib

matplotlib.use("QtAgg")
import numpy as np
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from PyQt6.QtCore import Qt
from PyQt6.QtWidgets import (
    QLabel,
    QPushButton,
    QScrollArea,
    QTableWidget,
    QTableWidgetItem,
    QVBoxLayout,
    QWidget,
)

from matstat_app.math.compute import (
    AnalysisResult,
    chi2_normality_gof,
    mean_test_zero,
    sample_row_numbers,
    shapiro_wilk_normal_hint,
    variance_test_sigma2,
)

def _clear_layout(layout: QVBoxLayout) -> None:
    while layout.count():
        item = layout.takeAt(0)
        w = item.widget()
        if w is not None:
            w.setParent(None)
            w.deleteLater()


def _label(html: str) -> QLabel:
    lab = QLabel(html)
    lab.setTextFormat(Qt.TextFormat.RichText)
    lab.setWordWrap(True)
    return lab


class TaskPage(QWidget):
    """Одна страница пункта ТЗ; при открытии вызывается refresh()."""

    def __init__(self, task_num: int, main_window: Any) -> None:
        super().__init__()
        self._task_num = int(task_num)
        self._main = main_window

        root = QVBoxLayout(self)
        self._head = QLabel()
        self._head.setTextFormat(Qt.TextFormat.RichText)
        self._head.setWordWrap(True)
        root.addWidget(self._head)

        self._scroll = QScrollArea()
        self._scroll.setWidgetResizable(True)
        self._inner = QWidget()
        self._inner_layout = QVBoxLayout(self._inner)
        self._scroll.setWidget(self._inner)
        root.addWidget(self._scroll, stretch=1)

        btn = QPushButton("В главное меню")
        btn.clicked.connect(main_window._to_main_menu)
        root.addWidget(btn)

        self._set_title()

    def _set_title(self) -> None:
        from matstat_app.gui.main_window import TASK_TITLES

        t = TASK_TITLES.get(self._task_num, "")
        self._head.setText(f"<h3>Задание {self._task_num}</h3><p>{t}</p>")

    def refresh(self) -> None:
        _clear_layout(self._inner_layout)
        res: Optional[AnalysisResult] = self._main._compute_analysis()
        if res is None:
            self._inner_layout.addWidget(
                _label(
                    "<p>Нет выборки: откройте «Исходный датасет и параметр N», "
                    "загрузите данные и нажмите «Применить N».</p>"
                )
            )
            return

        if self._task_num == 1:
            self._fill_task1(res)
        elif self._task_num == 2:
            self._fill_task2(res)
        elif self._task_num == 3:
            self._fill_task3(res)
        elif self._task_num == 4:
            self._fill_task4(res)
        elif self._task_num == 5:
            self._fill_task5(res)
        elif self._task_num == 6:
            self._fill_task6(res)
        elif self._task_num == 7:
            self._fill_task7(res)
        elif self._task_num == 8:
            self._fill_task8(res)
        elif self._task_num == 9:
            self._fill_task9(res)
        elif self._task_num == 10:
            self._fill_task10(res)
        elif self._task_num == 11:
            self._fill_task11(res)

    def _fill_task1(self, res: AnalysisResult) -> None:
        rows = self._main._rows
        n = self._main._n_start
        r = len(rows)
        idx = sample_row_numbers(r, n, 10)
        lines = [
            f"<p>Объём выборки (число значений): <b>{res.n}</b>.</p>",
            "<p>Использованы строки таблицы (по кругу): <b>"
            + ", ".join(str(i) for i in idx)
            + "</b>.</p>",
            "<p>Значения выборки (в порядке объединения строк):</p>",
            "<p><code>"
            + " ".join(f"{v:.6g}" for v in np.asarray(res.x).ravel())
            + "</code></p>",
        ]
        self._inner_layout.addWidget(_label("".join(lines)))

    def _fill_task2(self, res: AnalysisResult) -> None:
        self._inner_layout.addWidget(
            _label(
                f"<p>min x = <b>{res.x_min:.6g}</b></p>"
                f"<p>max x = <b>{res.x_max:.6g}</b></p>"
                f"<p>Размах R = max x − min x = <b>{res.r_span:.6g}</b></p>"
            )
        )

    def _fill_task3(self, res: AnalysisResult) -> None:
        lg = math.log10(res.n)
        raw = 1.0 + 3.32 * lg
        k = res.k
        self._inner_layout.addWidget(
            _label(
                f"<p>n = <b>{res.n}</b>, lg n = {lg:.6g}</p>"
                f"<p>1 + 3,32·lg n = <b>{raw:.6g}</b></p>"
                f"<p>Число интервалов k = ⌊1 + 3,32·lg n⌋ = <b>{k}</b></p>"
            )
        )

    def _fill_task4(self, res: AnalysisResult) -> None:
        eps = self._main._eps_decimals
        self._inner_layout.addWidget(
            _label(
                f"<p>Размах R = <b>{res.r_span:.6g}</b>, k = <b>{res.k}</b></p>"
                f"<p>Шаг h = R / k ≈ <b>{res.h:.6g}</b> "
                f"(округление до ε = 10<sup>−{eps}</sup>).</p>"
            )
        )

    def _fill_task5(self, res: AnalysisResult) -> None:
        parts = [f"α<sub>{i}</sub> = {a:.6g}" for i, a in enumerate(res.alpha)]
        chk = float(res.alpha[-1] - res.alpha[0])
        approx = res.k * res.h
        self._inner_layout.addWidget(
            _label(
                "<p>Границы α<sub>0</sub> … α<sub>k</sub>:</p><p><b>"
                + ", ".join(parts)
                + "</b></p>"
                f"<p>Контроль: α<sub>k</sub> − α<sub>0</sub> = <b>{chk:.6g}</b> "
                f"(ожидается близко к k·h ≈ <b>{approx:.6g}</b>).</p>"
            )
        )

    def _fill_task6(self, res: AnalysisResult) -> None:
        self._inner_layout.addWidget(
            _label(
                "<p>Точки выборки на оси x; вертикали — границы α<sub>i</sub>; "
                "граница относится к <b>правому</b> отрезку (как в группировке).</p>"
            )
        )
        fig = Figure(figsize=(8, 2.8), layout="tight")
        ax = fig.add_subplot(1, 1, 1)
        x = np.asarray(res.x, dtype=float).ravel()
        rng = float(res.alpha[-1] - res.alpha[0]) or 1.0
        pad = 0.02 * rng
        ax.set_xlim(float(res.alpha[0]) - pad, float(res.alpha[-1]) + pad)
        y = np.zeros_like(x)
        ax.scatter(x, y, s=22, zorder=3, c="C0", label="точки выборки")
        for a in res.alpha:
            ax.axvline(float(a), color="gray", linewidth=0.9, alpha=0.85)
        ax.set_yticks([])
        ax.set_xlabel("x")
        ax.set_title("Числовая прямая и границы интервалов")
        ax.legend(loc="upper right")
        canvas = FigureCanvas(fig)
        self._inner_layout.addWidget(canvas)

    def _fill_task7(self, res: AnalysisResult) -> None:
        tbl = QTableWidget()
        tbl.setColumnCount(6)
        tbl.setHorizontalHeaderLabels(
            ["№", "Интервал (α_{i-1}; α_i]", "Середина x_i", "n_i", "n_i/n", "N_i"]
        )
        tbl.setRowCount(len(res.bins))
        for r, b in enumerate(res.bins):
            tbl.setItem(r, 0, QTableWidgetItem(str(b.idx)))
            tbl.setItem(r, 1, QTableWidgetItem(f"({b.lo:.6g}; {b.hi:.6g}]"))
            tbl.setItem(r, 2, QTableWidgetItem(f"{b.mid:.6g}"))
            tbl.setItem(r, 3, QTableWidgetItem(str(b.n_abs)))
            tbl.setItem(r, 4, QTableWidgetItem(f"{b.n_rel:.6g}"))
            tbl.setItem(r, 5, QTableWidgetItem(str(b.n_cum)))
        tbl.resizeColumnsToContents()
        tbl.horizontalHeader().setStretchLastSection(True)
        self._inner_layout.addWidget(_label("<p>Статистическая таблица сгруппированных данных:</p>"))
        self._inner_layout.addWidget(tbl)

        self._inner_layout.addWidget(
            _label("<p><b>Гистограмма</b> и <b>эмпирическая функция распределения</b> (по исходной выборке):</p>")
        )
        fig2 = Figure(figsize=(8, 5.5), layout="tight")
        ax1 = fig2.add_subplot(2, 1, 1)
        ax2 = fig2.add_subplot(2, 1, 2)
        x = res.x
        edges = res.alpha
        ax1.hist(x, bins=edges, density=False, edgecolor="black", alpha=0.85)
        ax1.set_title("Гистограмма абсолютных частот")
        ax1.set_xlabel("x")
        ax1.set_ylabel("n_i")
        ax1.grid(True, alpha=0.3)
        xs = np.sort(x)
        nn = xs.size
        y = np.arange(1, nn + 1, dtype=float) / nn
        ax2.step(xs, y, where="post", color="C0", label="F_n(x)")
        ax2.set_ylim(0, 1.05)
        ax2.set_title("Эмпирическая функция распределения")
        ax2.set_xlabel("x")
        ax2.set_ylabel("F_n(x)")
        ax2.grid(True, alpha=0.3)
        ax2.legend(loc="lower right")
        c2 = FigureCanvas(fig2)
        self._inner_layout.addWidget(c2)

    def _fill_task8(self, res: AnalysisResult) -> None:
        self._inner_layout.addWidget(
            _label(
                "<p>По <b>сгруппированным</b> данным (середины интервалов, частоты nᵢ):</p>"
                f"<p>Выборочное среднее x̄* = <b>{res.mean_grouped:.6g}</b></p>"
                f"<p>Исправленная дисперсия s*² = <b>{res.var_grouped:.6g}</b></p>"
                "<p>Для сравнения по <b>исходной</b> выборке:</p>"
                f"<p>x̄ = <b>{res.mean_raw:.6g}</b>, s² = <b>{res.var_raw:.6g}</b></p>"
            )
        )

    def _fill_task9(self, res: AnalysisResult) -> None:
        chi2, df_chi, p_chi, rej = chi2_normality_gof(res.x, res.alpha)
        lines = [
            "<p>Критерий согласия <b>χ² Пирсона</b> с оценкой параметров N(μ̂, σ̂) по выборке "
            "и интервалами группировки; малые ожидаемые частоты объединяются.</p>"
        ]
        if np.isfinite(chi2):
            lines.append(
                f"<p>χ² = <b>{chi2:.6g}</b>, число степеней свободы = <b>{df_chi}</b>, "
                f"p-value = <b>{p_chi:.6g}</b></p>"
                f"<p>При α = 0,05: H₀ о нормальном законе <b>{'отвергаем' if rej else 'не отвергаем'}</b>.</p>"
            )
        else:
            lines.append("<p>Расчёт χ² недоступен (мало степеней свободы или s = 0).</p>")
        w, p_sw, _rej_sw = shapiro_wilk_normal_hint(res.x)
        if np.isfinite(w):
            lines.append(
                f"<p>Дополнительно (Шапиро–Уилк): W = {w:.6g}, p = {p_sw:.6g}.</p>"
            )
        self._inner_layout.addWidget(_label("".join(lines)))

    def _fill_task10(self, res: AnalysisResult) -> None:
        chi2v, lo, hi, rej = variance_test_sigma2(res.x, 1.0, 0.05)
        if np.isfinite(chi2v):
            txt = (
                "<p>H₀: σ² = 1, уровень значимости <b>0,05</b> (двусторонний критерий для "
                "(n−1)s²/σ₀² ~ χ²(n−1)).</p>"
                f"<p>Статистика (n−1)s²/σ₀² = <b>{chi2v:.6g}</b>.</p>"
                f"<p>Границы χ²<sub>0,025</sub>(n−1) = <b>{lo:.6g}</b>, "
                f"χ²<sub>0,975</sub>(n−1) = <b>{hi:.6g}</b>.</p>"
                f"<p>Вывод: H₀ <b>{'отвергаем' if rej else 'не отвергаем'}</b>.</p>"
            )
        else:
            txt = "<p>Недостаточно данных.</p>"
        self._inner_layout.addWidget(_label(txt))

    def _fill_task11(self, res: AnalysisResult) -> None:
        t_stat, p_t, _al, rej = mean_test_zero(res.x, 0.05)
        if np.isfinite(t_stat):
            txt = (
                "<p>H₀: μ = 0, уровень значимости <b>0,05</b> (двусторонний t-критерий).</p>"
                f"<p>t = <b>{t_stat:.6g}</b>, p-value = <b>{p_t:.6g}</b>.</p>"
                f"<p>Вывод: H₀ <b>{'отвергаем' if rej else 'не отвергаем'}</b>.</p>"
            )
        else:
            txt = "<p>Недостаточно данных.</p>"
        self._inner_layout.addWidget(_label(txt))
