"""Страницы заданий ЛР №2 (ВиСМАД)."""

from __future__ import annotations

from typing import Any, Callable, Optional

from PyQt6.QtCore import Qt
from PyQt6.QtWidgets import (
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QPushButton,
    QSpinBox,
    QVBoxLayout,
    QWidget,
)

from matstat_app.math import vismad

VISmad_TASK_TITLES: dict[int, str] = {
    1: "φ(n): определение, факторизация, ДПФ",
    2: "Тест простоты Ферма",
    3: "Символы Лежандра/Якоби, Соловей–Штрассен",
    4: "Тест Миллера–Рабина",
    5: "Линейный конгруэнтный генератор (LCG)",
    6: "Инверсный конгруэнтный генератор (ICG)",
    7: "Генератор Блюма–Блюма–Шуба (BBS)",
}


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


def _spin_int(lo: int, hi: int, val: int) -> QSpinBox:
    s = QSpinBox()
    s.setRange(lo, hi)
    s.setValue(val)
    return s


class VismadTaskPage(QWidget):
    def __init__(self, task_num: int, main_window: Any) -> None:
        super().__init__()
        self._task_num = int(task_num)
        self._main = main_window

        root = QVBoxLayout(self)
        self._head = QLabel()
        self._head.setTextFormat(Qt.TextFormat.RichText)
        self._head.setWordWrap(True)
        root.addWidget(self._head)

        self._inner = QWidget()
        self._inner_layout = QVBoxLayout(self._inner)
        root.addWidget(self._inner, stretch=1)

        btn = QPushButton("В меню ЛР №2")
        btn.clicked.connect(main_window._to_lr2_menu)
        root.addWidget(btn)

        self._set_title()

    def _set_title(self) -> None:
        t = VISmad_TASK_TITLES.get(self._task_num, "")
        self._head.setText(f"<h3>ЛР №2 — задание {self._task_num}</h3><p>{t}</p>")

    def refresh(self) -> None:
        _clear_layout(self._inner_layout)
        builders = {
            1: self._build_task1,
            2: self._build_task2,
            3: self._build_task3,
            4: self._build_task4,
            5: self._build_task5,
            6: self._build_task6,
            7: self._build_task7,
        }
        builders.get(self._task_num, lambda: None)()

    def _add_row(self, text: str, widget: QWidget) -> None:
        row = QHBoxLayout()
        lab = QLabel(text)
        row.addWidget(lab)
        row.addWidget(widget)
        row.addStretch()
        wrap = QWidget()
        wrap.setLayout(row)
        self._inner_layout.addWidget(wrap)

    def _add_compute(
        self,
        layout: QVBoxLayout,
        handler: Callable[[], None],
        out: QLabel,
    ) -> None:
        btn = QPushButton("Вычислить")
        btn.clicked.connect(handler)
        layout.addWidget(btn)
        layout.addWidget(out)

    def _build_task1(self) -> None:
        spin_n = _spin_int(1, 10_000_000, 36)
        self._add_row("n =", spin_n)
        out = _label("")
        self._inner_layout.addWidget(out)

        def run() -> None:
            n = spin_n.value()
            try:
                d, f, t = vismad.euler_phi_all(n)
                fac = vismad.factorize(n)
                fac_s = " · ".join(f"{p}^{e}" if e > 1 else str(p) for p, e in fac) or "1"
                out.setText(
                    f"<p>Разложение: <b>{fac_s}</b></p>"
                    f"<p>φ(n) по определению: <b>{d}</b></p>"
                    f"<p>φ(n) по формуле факторизации: <b>{f}</b></p>"
                    f"<p>φ(n) по ДПФ (Σ gcd·cos): <b>{t:.10g}</b> "
                    f"(округл. <b>{round(t):.0f}</b>)</p>"
                )
            except ValueError as e:
                out.setText(f"<p style='color:red'>{e}</p>")

        btn = QPushButton("Вычислить")
        btn.clicked.connect(run)
        self._inner_layout.addWidget(btn)

    def _build_primality_task(
        self,
        test_fn: Callable[..., tuple],
        extra_fields: Optional[list[tuple[str, QSpinBox]]] = None,
    ) -> None:
        spin_n = _spin_int(2, 10_000_000, 561)
        spin_w = _spin_int(1, 50, 5)
        spin_seed = _spin_int(0, 999_999, 42)
        self._add_row("n =", spin_n)
        self._add_row("число свидетелей =", spin_w)
        self._add_row("seed =", spin_seed)
        if extra_fields:
            for txt, w in extra_fields:
                self._add_row(txt, w)
        out = _label("")
        self._inner_layout.addWidget(out)

        def run() -> None:
            try:
                ok, ws, details = test_fn(spin_n.value(), spin_w.value(), spin_seed.value())
                ws_s = ", ".join(str(a) for a in ws)
                lines = "<br>".join(details)
                verdict = "вероятно простое" if ok else "составное / отвергнуто"
                out.setText(
                    f"<p>Свидетели: <b>[{ws_s}]</b></p>"
                    f"<p>{lines}</p>"
                    f"<p><b>{verdict}</b></p>"
                )
            except ValueError as e:
                out.setText(f"<p style='color:red'>{e}</p>")

        btn = QPushButton("Запустить тест")
        btn.clicked.connect(run)
        self._inner_layout.addWidget(btn)

    def _build_task2(self) -> None:
        self._build_primality_task(vismad.fermat_test)

    def _build_task3(self) -> None:
        spin_a = _spin_int(0, 10_000_000, 2)
        spin_p = _spin_int(3, 10_000_000, 7)
        spin_n = _spin_int(3, 10_000_000, 15)
        self._add_row("a (Лежандр) =", spin_a)
        self._add_row("p (простое) =", spin_p)
        self._add_row("n (Якоби, нечёт.) =", spin_n)
        out_sym = _label("")
        self._inner_layout.addWidget(out_sym)

        def run_sym() -> None:
            try:
                leg = vismad.legendre_symbol(spin_a.value(), spin_p.value())
                jac = vismad.jacobi_symbol(spin_a.value(), spin_n.value())
                out_sym.setText(
                    f"<p>(a/p) Лежандр = <b>{leg}</b></p>"
                    f"<p>(a/n) Якоби = <b>{jac}</b></p>"
                )
            except ValueError as e:
                out_sym.setText(f"<p style='color:red'>{e}</p>")

        btn1 = QPushButton("Символы Лежандра и Якоби")
        btn1.clicked.connect(run_sym)
        self._inner_layout.addWidget(btn1)
        self._inner_layout.addWidget(_label("<p><b>Тест Соловея–Штрассена:</b></p>"))
        self._build_primality_task(vismad.solovay_strassen_test)

    def _build_task4(self) -> None:
        self._build_primality_task(vismad.miller_rabin_test)

    def _build_generator_task(
        self,
        period_fn: Callable[..., int],
        fields: list[tuple[str, str, int]],
        experiment_fn: Callable[..., tuple],
        hint: str,
    ) -> None:
        spins = {}
        for key, lbl, default in fields:
            s = _spin_int(1, 2_147_483_647, default)
            spins[key] = s
            self._add_row(lbl, s)
        spin_x0 = _spin_int(0, 2_147_483_647, 1)
        self._add_row("X₀ =", spin_x0)
        out = _label("")
        self._inner_layout.addWidget(out)

        def run_period() -> None:
            try:
                kwargs = {k: spins[k].value() for k in spins}
                p = period_fn(**kwargs, x0=spin_x0.value())
                out.setText(f"<p>Период генератора: <b>{p}</b></p>")
            except ValueError as e:
                out.setText(f"<p style='color:red'>{e}</p>")

        btn = QPushButton("Оценить период")
        btn.clicked.connect(run_period)
        self._inner_layout.addWidget(btn)
        self._inner_layout.addWidget(_label(f"<p>{hint}</p>"))

        spin_samples = _spin_int(10, 5000, 512)
        spin_seed = _spin_int(0, 999_999, 1)
        self._add_row("число опытов (Z₂¹²):", spin_samples)
        self._add_row("seed опытов =", spin_seed)
        out_exp = _label("")
        self._inner_layout.addWidget(out_exp)

        def run_exp() -> None:
            try:
                periods, var_p = experiment_fn(
                    spin_samples.value(), spin_seed.value()
                )
                if not periods:
                    out_exp.setText("<p>Не удалось собрать выборку периодов.</p>")
                    return
                mn = min(periods)
                mx = max(periods)
                out_exp.setText(
                    f"<p>Опытов: <b>{len(periods)}</b>, "
                    f"min(P)=<b>{mn}</b>, max(P)=<b>{mx}</b></p>"
                    f"<p>Оценка <b>D(P)</b> (выборочная дисперсия периодов): "
                    f"<b>{var_p:.6g}</b></p>"
                )
            except ValueError as e:
                out_exp.setText(f"<p style='color:red'>{e}</p>")

        btn2 = QPushButton("Эксперимент: D(P) для случайных параметров")
        btn2.clicked.connect(run_exp)
        self._inner_layout.addWidget(btn2)

    def _build_task5(self) -> None:
        self._build_generator_task(
            vismad.lcg_period,
            [("a", "a =", 1), ("b", "b =", 1), ("m", "m =", 16)],
            vismad.sample_periods_lcg,
            "X<sub>n+1</sub> = (a·X<sub>n</sub> + b) mod m. "
            "D(P) — по случайным (a,b,m) из ℤ<sub>2¹²</sub>.",
        )

    def _build_task6(self) -> None:
        self._build_generator_task(
            vismad.icg_period,
            [("a", "a =", 3), ("b", "b =", 1), ("mod", "модуль n =", 11)],
            vismad.sample_periods_icg,
            "Если модуль простой: при X=0 → b, иначе a·X⁻¹+b. "
            "Иначе при gcd(a,n)=1: a·X⁻¹+b mod n.",
        )

    def _build_task7(self) -> None:
        spin_p = _spin_int(3, 65535, 11)
        spin_q = _spin_int(3, 65535, 19)
        spin_x0 = _spin_int(1, 10_000_000, 3)
        self._add_row("p ≡ 3 (mod 4) =", spin_p)
        self._add_row("q ≡ 3 (mod 4) =", spin_q)
        self._add_row("X₀ =", spin_x0)
        out = _label("")
        self._inner_layout.addWidget(out)

        def run_period() -> None:
            try:
                period = vismad.bbs_period(spin_p.value(), spin_q.value(), spin_x0.value())
                M = spin_p.value() * spin_q.value()
                out.setText(f"<p>M = p·q = <b>{M}</b>, период: <b>{period}</b></p>")
            except ValueError as e:
                out.setText(f"<p style='color:red'>{e}</p>")

        btn = QPushButton("Оценить период")
        btn.clicked.connect(run_period)
        self._inner_layout.addWidget(btn)
        self._inner_layout.addWidget(
            _label(
                "<p>X<sub>n+1</sub> = X<sub>n</sub>² mod M, p,q — простые ≡ 3 (mod 4).</p>"
            )
        )

        spin_samples = _spin_int(10, 2000, 256)
        spin_seed = _spin_int(0, 999_999, 1)
        self._add_row("число опытов:", spin_samples)
        self._add_row("seed =", spin_seed)
        out_exp = _label("")
        self._inner_layout.addWidget(out_exp)

        def run_exp() -> None:
            periods, var_p = vismad.sample_periods_bbs(
                spin_samples.value(), spin_seed.value()
            )
            if not periods:
                out_exp.setText("<p>Мало успешных пар (p,q); увеличьте число опытов.</p>")
                return
            out_exp.setText(
                f"<p>Успешных опытов: <b>{len(periods)}</b>, "
                f"D(P) ≈ <b>{var_p:.6g}</b></p>"
            )

        btn2 = QPushButton("Эксперимент: D(P)")
        btn2.clicked.connect(run_exp)
        self._inner_layout.addWidget(btn2)
