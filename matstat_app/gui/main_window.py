"""Главное окно: главное меню, датасет (N, ε), страницы заданий 1–11."""

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
from matstat_app.math.compute import AnalysisResult, analyze, parse_rows, select_sample
from matstat_app.math.dataset import DEFAULT_DATASET_TEXT


def _rows_to_table_model(rows: Sequence[Sequence[float]]) -> tuple[int, int, List[List[float]]]:
    if not rows:
        return 0, 0, []
    ncol = max(len(r) for r in rows)
    return len(rows), ncol, [list(r) for r in rows]


TASK_TITLES: dict[int, str] = {
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


class MatstatMainWindow(QMainWindow):
    IDX_MENU = 0
    IDX_DATASET = 1
    IDX_TASK_FIRST = 2

    def __init__(self) -> None:
        super().__init__()
        self.setWindowTitle("MATSTAT")
        self.resize(1024, 600)

        self._rows: List[List[float]] = []
        self._n_start: int = 1

        self._stack = QStackedWidget()
        self.setCentralWidget(self._stack)

        self._stack.addWidget(self._build_main_menu())
        self._stack.addWidget(self._build_dataset_page())
        for i in range(1, 12):
            self._stack.addWidget(TaskPage(i, self))

        self._stack.setCurrentIndex(self.IDX_MENU)

    def _eps_decimals(self) -> int:
        return int(self.spin_eps.value())

    def _compute_analysis(self) -> Optional[AnalysisResult]:
        if not self._rows:
            return None
        try:
            x = select_sample(self._rows, self._n_start, 10)
            return analyze(x, eps_decimals=self._eps_decimals())
        except ValueError:
            return None

    def _build_main_menu(self) -> QWidget:
        page = QWidget()
        outer = QVBoxLayout(page)

        title = QLabel("<h2>Главное меню</h2>")
        title.setTextFormat(Qt.TextFormat.RichText)
        outer.addWidget(title)

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        inner = QWidget()
        menu_layout = QVBoxLayout(inner)

        ref_btn = QPushButton("1.", inner)
        ref_btn.hide()
        base_h = ref_btn.sizeHint().height()

        btn_data = QPushButton("Исходный датасет и параметр N")
        btn_data.clicked.connect(self._open_dataset_page)
        f_data = QFont(btn_data.font())
        ps = f_data.pointSize()
        if ps <= 0:
            ps = self.font().pointSize() if self.font().pointSize() > 0 else 9
        f_data.setPointSize(ps * 2)
        btn_data.setFont(f_data)
        btn_data.setMinimumHeight(max(base_h * 2, btn_data.sizeHint().height()))
        menu_layout.addWidget(btn_data)

        for i in range(1, 12):
            t = TASK_TITLES[i]
            btn = QPushButton(f"{i}. {t}")
            btn.clicked.connect(partial(self._go_task, i))
            menu_layout.addWidget(btn)

        menu_layout.addStretch()
        scroll.setWidget(inner)
        outer.addWidget(scroll, stretch=1)

        btn_quit = QPushButton("Выход из приложения")
        btn_quit.clicked.connect(self.close)
        outer.addWidget(btn_quit)

        return page

    def _build_dataset_page(self) -> QWidget:
        page = QWidget()
        layout = QVBoxLayout(page)

        h = QLabel("<h3>Исходный датасет</h3>")
        h.setTextFormat(Qt.TextFormat.RichText)
        layout.addWidget(h)
        t = QLabel(
            "Здесь задаётся <b>N</b> и точность округления шага <b>h</b> (ε). "
            "Если N больше числа строк R, старт по правилу остатка (см. подпись ниже)."
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
        btn_back = QPushButton("В главное меню")
        btn_back.clicked.connect(self._to_main_menu)
        row_back.addWidget(btn_back)
        row_back.addStretch()
        layout.addLayout(row_back)

        return page

    def _task_page_index(self, task_num: int) -> int:
        return self.IDX_TASK_FIRST + (task_num - 1)

    def _go_task(self, task_num: int) -> None:
        idx = self._task_page_index(task_num)
        w = self._stack.widget(idx)
        if isinstance(w, TaskPage):
            w.refresh()
        self._stack.setCurrentIndex(idx)

    def _open_dataset_page(self) -> None:
        self.spin_n.setValue(self._n_start)
        if not self._rows:
            self._load_default_dataset()
        else:
            self._refresh_n_label()
        self._stack.setCurrentIndex(self.IDX_DATASET)

    def _to_main_menu(self) -> None:
        self._stack.setCurrentIndex(self.IDX_MENU)

    def _apply_n_from_dataset_page(self) -> None:
        self._n_start = int(self.spin_n.value())
        self._refresh_n_label()

    def _refresh_n_label(self) -> None:
        from matstat_app.math.compute import effective_start_index0, sample_row_numbers_1based

        r = len(self._rows)
        if r == 0:
            self.lbl_n.setText(
                f"Текущее N: <b>{self._n_start}</b> — в таблице нет строк данных."
            )
            return
        nums = sample_row_numbers_1based(r, self._n_start, 10)
        tail = ", ".join(str(x) for x in nums)
        first = effective_start_index0(r, self._n_start) + 1
        if self._n_start > r:
            note = f"N больше R — первая строка выборки: <b>{first}</b> (остаток по R)"
        else:
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
