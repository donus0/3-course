from __future__ import annotations

import math
from dataclasses import dataclass
from typing import List, Sequence, Tuple

import numpy as np
from scipy import stats


def parse_rows(text: str) -> List[List[float]]:
    rows: List[List[float]] = []
    for line in text.splitlines():
        line = line.strip()
        if not line:
            continue
        parts = line.replace(",", ".").split()
        vals = [float(p) for p in parts]
        if vals:
            rows.append(vals)
    return rows


def resolve_start_row_index(rows_count: int, start_row_1based: int) -> int:
    """Индекс (с 0) первой строки выборки; при N > R — циклическое правило остатка."""
    if start_row_1based < 1:
        raise ValueError("Номер строки N должен быть ≥ 1")
    if rows_count <= 0:
        raise ValueError("Нет строк данных")
    if start_row_1based <= rows_count:
        return start_row_1based - 1
    m = start_row_1based % rows_count
    return rows_count - 1 if m == 0 else m


def sample_row_numbers(
    rows_count: int, start_row_1based: int, num_rows: int = 10
) -> List[int]:
    """Номера строк выборки (с 1), подряд по кругу, всего num_rows."""
    i0 = resolve_start_row_index(rows_count, start_row_1based)
    return [(i0 + j) % rows_count + 1 for j in range(num_rows)]


def build_sample_array(rows: Sequence[Sequence[float]], start_row_1based: int, num_rows: int = 10) -> np.ndarray:
    """Склеивает значения num_rows строк таблицы (с N) в один вектор выборки."""
    r = len(rows)
    if r == 0:
        raise ValueError("Нет строк данных")
    i0 = resolve_start_row_index(r, start_row_1based)
    chunk = [rows[(i0 + j) % r] for j in range(num_rows)]
    return np.concatenate([np.asarray(row, dtype=float) for row in chunk], axis=0)


@dataclass
class IntervalBin:
    idx: int
    lo: float
    hi: float
    mid: float
    n_abs: int
    n_rel: float
    n_cum: int


@dataclass
class AnalysisResult:
    x: np.ndarray
    n: int
    x_min: float
    x_max: float
    r_span: float
    k: int
    h: float
    alpha: np.ndarray
    bins: List[IntervalBin]
    mean_grouped: float
    var_grouped: float
    mean_raw: float
    var_raw: float


def _round_h(h: float, eps_decimals: int = 2) -> float:
    return round(h, eps_decimals)


def build_intervals(x: np.ndarray, eps_decimals: int = 2) -> Tuple[int, float, np.ndarray]:
    n = int(x.size)
    if n < 2:
        raise ValueError("Объём выборки должен быть не меньше 2")
    x_min = float(np.min(x))
    x_max = float(np.max(x))
    r_span = x_max - x_min
    if r_span <= 0:
        raise ValueError("Размах выборки равен 0 — группировка невозможна")
    k = int(math.floor(1.0 + 3.32 * math.log10(n)))
    k = max(k, 1)
    h = _round_h(r_span / k, eps_decimals)
    if h <= 0:
        h = 10 ** (-eps_decimals)
    alpha = np.array([x_min + i * h for i in range(k + 1)], dtype=float)
    if alpha[-1] < x_max:
        alpha = alpha.copy()
        alpha[-1] = x_max
    return k, h, alpha


def assign_bins(x: np.ndarray, alpha: np.ndarray) -> Tuple[int, ...]:
    k = alpha.size - 1
    counts = [0] * k
    for v in np.asarray(x, dtype=float).ravel():
        placed = False
        for i in range(k):
            lo, hi = float(alpha[i]), float(alpha[i + 1])
            if i == 0:
                if lo <= v <= hi:
                    counts[i] += 1
                    placed = True
                    break
            elif i == k - 1:
                if lo < v <= hi:
                    counts[i] += 1
                    placed = True
                    break
            else:
                if lo < v <= hi:
                    counts[i] += 1
                    placed = True
                    break
        if not placed:
            if v <= alpha[0]:
                counts[0] += 1
            elif v > alpha[-1]:
                counts[-1] += 1
    return tuple(counts)


def analyze(x: np.ndarray, eps_decimals: int = 2) -> AnalysisResult:
    x = np.asarray(x, dtype=float).ravel()
    n = int(x.size)
    x_min = float(np.min(x))
    x_max = float(np.max(x))
    r_span = x_max - x_min
    k, h, alpha = build_intervals(x, eps_decimals=eps_decimals)
    counts = assign_bins(x, alpha)
    bins: List[IntervalBin] = []
    cum = 0
    for i, c in enumerate(counts, start=1):
        lo, hi = float(alpha[i - 1]), float(alpha[i])
        mid = 0.5 * (lo + hi)
        cum += c
        bins.append(
            IntervalBin(
                idx=i,
                lo=lo,
                hi=hi,
                mid=mid,
                n_abs=int(c),
                n_rel=c / n,
                n_cum=cum,
            )
        )
    mids = np.array([b.mid for b in bins], dtype=float)
    cnt = np.array([b.n_abs for b in bins], dtype=float)
    mean_g = float(np.sum(cnt * mids) / n)
    var_g = float(np.sum(cnt * (mids - mean_g) ** 2) / (n - 1)) if n > 1 else 0.0
    mean_raw = float(np.mean(x))
    var_raw = float(np.var(x, ddof=1)) if n > 1 else 0.0
    return AnalysisResult(
        x=x,
        n=n,
        x_min=x_min,
        x_max=x_max,
        r_span=r_span,
        k=k,
        h=h,
        alpha=alpha,
        bins=bins,
        mean_grouped=mean_g,
        var_grouped=var_g,
        mean_raw=mean_raw,
        var_raw=var_raw,
    )


def chi2_normality_gof(x: np.ndarray, alpha: np.ndarray) -> Tuple[float, int, float, bool]:
    n = x.size
    mu = float(np.mean(x))
    sigma = float(np.std(x, ddof=1))
    if sigma <= 0:
        return float("nan"), 0, float("nan"), False

    k0 = alpha.size - 1
    obs = np.array(assign_bins(x, alpha), dtype=float)
    probs = []
    dist = stats.norm(loc=mu, scale=sigma)
    for i in range(k0):
        p = float(dist.cdf(alpha[i + 1]) - dist.cdf(alpha[i]))
        probs.append(max(p, 1e-12))
    probs = np.array(probs, dtype=float)
    probs = probs / probs.sum()

    obs_m, exp_m = _merge_bins(obs, probs * n)
    df = int(obs_m.size - 1 - 2)
    if df < 1:
        return float("nan"), df, float("nan"), False
    chi2 = float(np.sum((obs_m - exp_m) ** 2 / exp_m))
    p_value = float(1 - stats.chi2.cdf(chi2, df))
    reject = p_value < 0.05
    return chi2, df, p_value, reject


def _merge_bins(obs: np.ndarray, exp: np.ndarray) -> Tuple[np.ndarray, np.ndarray]:
    o = obs.tolist()
    e = exp.tolist()
    while len(o) > 1 and e[0] < 5.0:
        o[1] += o[0]
        e[1] += e[0]
        o.pop(0)
        e.pop(0)
    while len(o) > 1 and e[-1] < 5.0:
        o[-2] += o[-1]
        e[-2] += e[-1]
        o.pop()
        e.pop()
    i = 1
    while i < len(o) - 1:
        if e[i] < 5.0:
            if e[i - 1] <= e[i + 1]:
                o[i - 1] += o[i]
                e[i - 1] += e[i]
                o.pop(i)
                e.pop(i)
            else:
                o[i] += o[i + 1]
                e[i] += e[i + 1]
                o.pop(i + 1)
                e.pop(i + 1)
        else:
            i += 1
    return np.asarray(o, dtype=float), np.asarray(e, dtype=float)


def variance_test_sigma2(x: np.ndarray, sigma0_sq: float = 1.0, alpha: float = 0.05) -> Tuple[float, float, float, bool]:
    n = x.size
    if n < 2:
        return float("nan"), float("nan"), float("nan"), False
    s2 = float(np.var(x, ddof=1))
    chi2 = (n - 1) * s2 / sigma0_sq
    chi_lo = stats.chi2.ppf(alpha / 2, n - 1)
    chi_hi = stats.chi2.ppf(1 - alpha / 2, n - 1)
    reject = not (chi_lo <= chi2 <= chi_hi)
    return chi2, chi_lo, chi_hi, reject


def mean_test_zero(x: np.ndarray, alpha: float = 0.05) -> Tuple[float, float, float, bool]:
    n = x.size
    if n < 2:
        return float("nan"), float("nan"), float("nan"), False
    t_stat, p_value = stats.ttest_1samp(x, 0.0)
    reject = p_value < alpha
    return float(t_stat), float(p_value), float(alpha), reject


def shapiro_wilk_normal_hint(x: np.ndarray) -> Tuple[float, float, bool]:
    if x.size < 3 or x.size > 5000:
        return float("nan"), float("nan"), False
    w, p = stats.shapiro(x)
    return float(w), float(p), p < 0.05
