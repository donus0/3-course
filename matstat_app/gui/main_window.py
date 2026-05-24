"""Главное окно: выбор ЛР, меню ЛР №1 (матстат) и ЛР №2 (ВиСМАД)."""

from __future__ import annotations

from functools import partial
from typing import List, Optional, Sequence

from PyQt6.QtCore import Qt
from PyQt6.QtGui import QFont
from PyQt6.QtWidgets import (
    QHBoxLayout,
    QLabel,
    QMainWindow,
    QMessageBox,
    QPushButton,
    QScrollArea,
    QSpinBox,
    QStackedWidget,
    QTableWidget,
    QTableWidgetItem,
    QVBoxLayout,
    QWidget,
)

from matstat_app.gui.task_pages import TaskPage
from matstat_app.gui.vismad_task_pages import VISmad_TASK_TITLES, VismadTaskPage
from matstat_app.math.compute import AnalysisResult, analyze, build_sample_array, parse_rows
from matstat_app.math.dataset import DEFAULT_DATASET_TEXT


def _rows_to_table_model(rows: Sequence[Sequence[float]]) -> tuple[int, int, List[List[float]]]:
    if not rows:
        return 0, 0, []
    ncol = max(len(r) for r in rows)
    return len(rows), ncol, [list(r) for r in rows]


LR1_TASK_TITLES: dict[int, str] = {
    1: "Выборка: 10 строк с N-й",
    2: "min, max, размах R",
    3: "Число интервалов k",
    4: "Шаг h",
    5: "Границы интервалов αᵢ",
    6: "Точки на числовой прямой",
    7: "Статистическая таблица, гистограмма, ЭФР",
    8: "Среднее и исправленная дисперсия (сгрупп.)",
    9: "Проверка нормальности",
    10: "Гипотеза σ² = 1 (α = 0,05)",
    11: "Гипотеза μ = 0 (α = 0,05)",
}

# Алиас для task_pages
TASK_TITLES = LR1_TASK_TITLES


class MatstatMainWindow(QMainWindow):
    IDX_ROOT = 0
    IDX_LR1_MENU = 1
    IDX_DATASET = 2
    IDX_LR1_TASK_FIRST = 3
    NUM_LR1_TASKS = 11
    IDX_LR2_MENU = 3 + NUM_LR1_TASKS  # 14
    IDX_LR2_TASK_FIRST = IDX_LR2_MENU + 1  # 15
    NUM_LR2_TASKS = 7

    def __init__(self) -> None:
        super().__init__()
        self.setWindowTitle("ВиСМАД — лабораторные работы")
        self.resize(1024, 600)

        self._rows: List[List[float]] = []
        self._n_start: int = 1

        self._stack = QStackedWidget()
        self.setCentralWidget(self._stack)

        self._stack.addWidget(self._build_root_menu())
        self._stack.addWidget(self._build_lr1_menu())
        self._stack.addWidget(self._build_dataset_page())
        for i in range(1, self.NUM_LR1_TASKS + 1):
            self._stack.addWidget(TaskPage(i, self))
        self._stack.addWidget(self._build_lr2_menu())
        for i in range(1, self.NUM_LR2_TASKS + 1):
            self._stack.addWidget(VismadTaskPage(i, self))

        self._stack.setCurrentIndex(self.IDX_ROOT)

    def _eps_decimals(self) -> int:
        return int(self.spin_eps.value())

    def _compute_analysis(self) -> Optional[AnalysisResult]:
        if not self._rows:
            return None
        try:
            x = build_sample_array(self._rows, self._n_start, 10)
            return analyze(x, eps_decimals=self._eps_decimals())
        except ValueError:
            return None

    def _menu_button(self, text: str, slot, layout: QVBoxLayout, large: bool = False) -> QPushButton:
        btn = QPushButton(text)
        btn.clicked.connect(slot)
        if large:
            f = QFont(btn.font())
            ps = f.pointSize()
            if ps <= 0:
                ps = self.font().pointSize() if self.font().pointSize() > 0 else 9
            f.setPointSize(ps * 2)
            btn.setFont(f)
            btn.setMinimumHeight(btn.sizeHint().height() * 2)
        layout.addWidget(btn)
        return btn

    def _build_scrolled_menu(self, title_html: str, buttons: list[tuple[str, object, bool]]) -> QWidget:
        page = QWidget()
        outer = QVBoxLayout(page)
        title = QLabel(title_html)
        title.setTextFormat(Qt.TextFormat.RichText)
        outer.addWidget(title)

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        inner = QWidget()
        menu_layout = QVBoxLayout(inner)
        for text, slot, large in buttons:
            self._menu_button(text, slot, menu_layout, large=large)
        menu_layout.addStretch()
        scroll.setWidget(inner)
        outer.addWidget(scroll, stretch=1)
        return page

    def _build_root_menu(self) -> QWidget:
        page = QWidget()
        outer = QVBoxLayout(page)
        title = QLabel(
            "<h2>Вероятностные и стохастические модели анализа данных</h2>"
            "<p>Выберите лабораторную работу:</p>"
        )
        title.setTextFormat(Qt.TextFormat.RichText)
        title.setWordWrap(True)
        outer.addWidget(title)

        self._menu_button(
            "Лабораторная работа №1\n(математическая статистика)",
            self._to_lr1_menu,
            outer,
            large=True,
        )
        self._menu_button(
            "Лабораторная работа №2\n(ВиСМАД — Р1)",
            self._to_lr2_menu,
            outer,
            large=True,
        )
        outer.addStretch()
        btn_quit = QPushButton("Выход из приложения")
        btn_quit.clicked.connect(self.close)
        outer.addWidget(btn_quit)
        return page

    def _build_lr1_menu(self) -> QWidget:
        buttons: list[tuple[str, object, bool]] = [
            ("Исходный датасет и параметр N", self._open_dataset_page, True),
        ]
        for i in range(1, self.NUM_LR1_TASKS + 1):
            t = LR1_TASK_TITLES[i]
            buttons.append((f"{i}. {t}", partial(self._go_lr1_task, i), False))
        page = self._build_scrolled_menu("<h2>ЛР №1 — математическая статистика</h2>", buttons)
        wrap = QWidget()
        lay = QVBoxLayout(wrap)
        lay.addWidget(page)
        row = QHBoxLayout()
        btn_back = QPushButton("← К выбору лабораторной")
        btn_back.clicked.connect(self._to_root_menu)
        row.addWidget(btn_back)
        row.addStretch()
        lay.addLayout(row)
        return wrap

    def _build_lr2_menu(self) -> QWidget:
        buttons = [
            (f"{i}. {VISmad_TASK_TITLES[i]}", partial(self._go_lr2_task, i), False)
            for i in range(1, self.NUM_LR2_TASKS + 1)
        ]
        page = self._build_scrolled_menu(
            "<h2>ЛР №2 — ВиСМАД (задания по Р1)</h2>", buttons
        )
        wrap = QWidget()
        lay = QVBoxLayout(wrap)
        lay.addWidget(page)
        row = QHBoxLayout()
        btn_back = QPushButton("← К выбору лабораторной")
        btn_back.clicked.connect(self._to_root_menu)
        row.addWidget(btn_back)
        row.addStretch()
        lay.addLayout(row)
        return wrap

    def _build_dataset_page(self) -> QWidget:
        page = QWidget()
        layout = QVBoxLayout(page)

        h = QLabel("<h3>ЛР №1 — исходный датасет</h3>")
        h.setTextFormat(Qt.TextFormat.RichText)
        layout.addWidget(h)
        t = QLabel(
            "Здесь задаётся <b>N</b> и точность округления шага <b>h</b> (ε). "
            "Если N больше числа строк R, старт по правилу остатка."
        )
        t.setTextFormat(Qt.TextFormat.RichText)
        t.setWordWrap(True)
        layout.addWidget(t)

        self.lbl_n = QLabel()
        self.lbl_n.setTextFormat(Qt.TextFormat.RichText)
        self.lbl_n.setWordWrap(True)
        layout.addWidget(self.lbl_n)

        row_n = QHBoxLayout()
        row_n.addWidget(QLabel("N ="))
        self.spin_n = QSpinBox()
        self.spin_n.setMinimum(1)
        self.spin_n.setMaximum(9999)
        self.spin_n.setValue(self._n_start)
        row_n.addWidget(self.spin_n)
        row_n.addWidget(QLabel("Знаков после запятой для h (ε):"))
        self.spin_eps = QSpinBox()
        self.spin_eps.setMinimum(1)
        self.spin_eps.setMaximum(8)
        self.spin_eps.setValue(2)
        row_n.addWidget(self.spin_eps)
        self.btn_apply_n = QPushButton("Применить")
        self.btn_apply_n.clicked.connect(self._apply_n_from_dataset_page)
        row_n.addWidget(self.btn_apply_n)
        row_n.addStretch()
        layout.addLayout(row_n)

        self.table = QTableWidget()
        self.table.setAlternatingRowColors(True)
        layout.addWidget(self.table, stretch=1)

        row_back = QHBoxLayout()
        btn_back = QPushButton("В меню ЛР №1")
        btn_back.clicked.connect(self._to_lr1_menu)
        row_back.addWidget(btn_back)
        row_back.addStretch()
        layout.addLayout(row_back)

        return page

    def _lr1_task_index(self, task_num: int) -> int:
        return self.IDX_LR1_TASK_FIRST + (task_num - 1)

    def _lr2_task_index(self, task_num: int) -> int:
        return self.IDX_LR2_TASK_FIRST + (task_num - 1)

    def _go_lr1_task(self, task_num: int) -> None:
        idx = self._lr1_task_index(task_num)
        w = self._stack.widget(idx)
        if isinstance(w, TaskPage):
            w.refresh()
        self._stack.setCurrentIndex(idx)

    def _go_lr2_task(self, task_num: int) -> None:
        idx = self._lr2_task_index(task_num)
        w = self._stack.widget(idx)
        if isinstance(w, VismadTaskPage):
            w.refresh()
        self._stack.setCurrentIndex(idx)

    def _open_dataset_page(self) -> None:
        self.spin_n.setValue(self._n_start)
        if not self._rows:
            self._load_default_dataset()
        else:
            self._refresh_n_label()
        self._stack.setCurrentIndex(self.IDX_DATASET)

    def _to_root_menu(self) -> None:
        self._stack.setCurrentIndex(self.IDX_ROOT)

    def _to_lr1_menu(self) -> None:
        self._stack.setCurrentIndex(self.IDX_LR1_MENU)

    def _to_lr2_menu(self) -> None:
        self._stack.setCurrentIndex(self.IDX_LR2_MENU)

    def _to_main_menu(self) -> None:
        """Совместимость: старый вызов → меню ЛР №1."""
        self._to_lr1_menu()

    def _apply_n_from_dataset_page(self) -> None:
        self._n_start = int(self.spin_n.value())
        self._refresh_n_label()

    def _refresh_n_label(self) -> None:
        from matstat_app.math.compute import resolve_start_row_index, sample_row_numbers

        r = len(self._rows)
        if r == 0:
            self.lbl_n.setText(
                f"Текущее N: <b>{self._n_start}</b> — в таблице нет строк данных."
            )
            return
        nums = sample_row_numbers(r, self._n_start, 10)
        tail = ", ".join(str(x) for x in nums)
        first = resolve_start_row_index(r, self._n_start) + 1
        note = f"первая строка выборки: <b>{first}</b>"
        self.lbl_n.setText(
            f"Текущее N: <b>{self._n_start}</b>, R = <b>{r}</b> ({note}). "
            f"Порядок 10 строк: <b>{tail}</b>. "
            f"ε для h: <b>{self._eps_decimals()}</b> знаков."
        )

    def _load_default_dataset(self) -> None:
        try:
            self._rows = parse_rows(DEFAULT_DATASET_TEXT)
            self._apply_rows_to_table(self._rows)
            self._refresh_n_label()
        except ValueError as e:
            QMessageBox.warning(self, "Данные", str(e))

    def _apply_rows_to_table(self, rows: List[List[float]]) -> None:
        n_r, n_c, data = _rows_to_table_model(rows)
        self.table.clear()
        self.table.setRowCount(n_r)
        self.table.setColumnCount(n_c)
        headers = [str(j + 1) for j in range(n_c)]
        self.table.setHorizontalHeaderLabels(headers)
        for i, row_vals in enumerate(data):
            self.table.setVerticalHeaderItem(i, QTableWidgetItem(str(i + 1)))
            for j in range(n_c):
                if j < len(row_vals):
                    item = QTableWidgetItem(f"{row_vals[j]:g}")
                else:
                    item = QTableWidgetItem("")
                item.setFlags(item.flags() & ~Qt.ItemFlag.ItemIsEditable)
                self.table.setItem(i, j, item)
        self.table.resizeColumnsToContents()
