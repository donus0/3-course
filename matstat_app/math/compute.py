# Расчёты по лабораторной «Математическая статистика» (п. 1–11 ТЗ)

import math
from dataclasses import dataclass
from typing import List, Sequence, Tuple

import numpy as np
from scipy import stats


#  загрузка таблицы из текста 

def parse_rows(text: str) -> List[List[float]]:
    rows = []
    for line in text.splitlines():
        line = line.strip()
        if not line:
            continue
        parts = line.replace(",", ".").split()
        row = [float(p) for p in parts]
        if row:
            rows.append(row)
    return rows


#  п. 1: 10 строк с N-й 

def resolve_start_row_index(rows_count: int, start_row_1based: int) -> int:
    """Номер N → индекс первой строки (с 0). Если N > R, берём остаток от деления."""
    if start_row_1based < 1:
        raise ValueError("N должно быть не меньше 1")
    if rows_count <= 0:
        raise ValueError("Нет строк в таблице")
    if start_row_1based <= rows_count:
        return start_row_1based - 1
    m = start_row_1based % rows_count
    if m == 0:
        return rows_count - 1
    return m


def sample_row_numbers(rows_count: int, start_row_1based: int, num_rows: int = 10) -> List[int]:
    """Номера строк (с 1), которые входят в выборку."""
    i0 = resolve_start_row_index(rows_count, start_row_1based)
    nums = []
    for j in range(num_rows):
        nums.append((i0 + j) % rows_count + 1)
    return nums


def build_sample_array(
    rows: Sequence[Sequence[float]], start_row_1based: int, num_rows: int = 10
) -> np.ndarray:
    """Склеить 10 строк в один список значений выборки."""
    R = len(rows)
    if R == 0:
        raise ValueError("Нет строк данных")
    i0 = resolve_start_row_index(R, start_row_1based)
    sample = []
    for j in range(num_rows):
        for val in rows[(i0 + j) % R]:
            sample.append(float(val))
    return np.array(sample, dtype=float)


#  результаты для GUI 

@dataclass
class IntervalBin:
    """Одна строка таблицы из п. 7."""
    idx: int
    lo: float
    hi: float
    mid: float
    n_abs: int
    n_rel: float
    n_cum: int


@dataclass
class AnalysisResult:
    """Всё, что считается по выборке для заданий 2–8."""
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


#  п. 2: min, max, размах 

def min_max_range(x) -> Tuple[float, float, float]:
    data = [float(v) for v in np.asarray(x).flat]
    x_min = min(data)
    x_max = max(data)
    R = x_max - x_min
    return x_min, x_max, R


#  п. 3: число интервалов k 

def count_intervals(n: int) -> int:
    """k = [1 + 3,32 · lg n] — целая часть."""
    if n < 1:
        return 1
    return int(math.floor(1.0 + 3.32 * math.log10(n)))


#  п. 4: шаг h 

def step_h(R: float, k: int, eps_decimals: int = 2) -> float:
    h = R / k
    h = round(h, eps_decimals)
    if h <= 0:
        h = 10 ** (-eps_decimals)
    return h


#  п. 5: границы α0 … αk 

def interval_bounds(x_min: float, k: int, h: float, x_max: float) -> List[float]:
    alpha = [x_min + i * h for i in range(k + 1)]
    if alpha[-1] < x_max:
        alpha[-1] = x_max
    return alpha


#  п. 6–7: в какой интервал попала точка (граница → вправо) 

def which_interval(value: float, alpha: List[float]) -> int:
    k = len(alpha) - 1
    for i in range(k):
        lo = alpha[i]
        hi = alpha[i + 1]
        if i == 0:
            if lo <= value <= hi:
                return i
        else:
            if lo < value <= hi:
                return i
    # на всякий случай
    if value <= alpha[0]:
        return 0
    return k - 1


def frequencies(x, alpha: List[float]) -> List[int]:
    k = len(alpha) - 1
    counts = [0] * k
    for v in np.asarray(x).flat:
        i = which_interval(float(v), alpha)
        counts[i] += 1
    return counts


def build_intervals(x, eps_decimals: int = 2):
    """П. 3–5 одним вызовом: k, h, массив границ."""
    data = [float(v) for v in np.asarray(x).flat]
    n = len(data)
    if n < 2:
        raise ValueError("Нужно хотя бы 2 значения в выборке")
    x_min, x_max, R = min_max_range(data)
    if R <= 0:
        raise ValueError("Размах 0 — группировать нельзя")
    k = count_intervals(n)
    if k < 1:
        k = 1
    h = step_h(R, k, eps_decimals)
    alpha = interval_bounds(x_min, k, h, x_max)
    return k, h, np.array(alpha, dtype=float)


def assign_bins(x, alpha) -> Tuple[int, ...]:
    alpha_list = [float(a) for a in np.asarray(alpha).flat]
    return tuple(frequencies(x, alpha_list))


#  п. 7–8: таблица, среднее и дисперсия по сгруппированным данным 

def grouped_mean(x_mid_list, counts) -> float:
    n = sum(counts)
    s = 0.0
    for mid, ni in zip(x_mid_list, counts):
        s += mid * ni
    return s / n


def grouped_variance(x_mid_list, counts, mean_g: float) -> float:
    n = sum(counts)
    if n < 2:
        return 0.0
    s = 0.0
    for mid, ni in zip(x_mid_list, counts):
        s += ni * (mid - mean_g) ** 2
    return s / (n - 1)


def sample_mean(x) -> float:
    data = [float(v) for v in np.asarray(x).flat]
    return sum(data) / len(data)


def sample_variance(x) -> float:
    data = [float(v) for v in np.asarray(x).flat]
    n = len(data)
    if n < 2:
        return 0.0
    m = sum(data) / n
    s = 0.0
    for v in data:
        s += (v - m) ** 2
    return s / (n - 1)


def analyze(x, eps_decimals: int = 2) -> AnalysisResult:
    x = np.array([float(v) for v in np.asarray(x).flat], dtype=float)
    n = len(x)

    x_min, x_max, R = min_max_range(x)
    k, h, alpha = build_intervals(x, eps_decimals=eps_decimals)
    alpha_list = [float(a) for a in alpha]
    counts = frequencies(x, alpha_list)

    bins = []
    cum = 0
    mids = []
    for i in range(k):
        lo = alpha_list[i]
        hi = alpha_list[i + 1]
        mid = (lo + hi) / 2.0
        mids.append(mid)
        ni = counts[i]
        cum += ni
        bins.append(
            IntervalBin(
                idx=i + 1,
                lo=lo,
                hi=hi,
                mid=mid,
                n_abs=ni,
                n_rel=ni / n,
                n_cum=cum,
            )
        )

    mean_g = grouped_mean(mids, counts)
    var_g = grouped_variance(mids, counts, mean_g)
    mean_raw = sample_mean(x)
    var_raw = sample_variance(x)

    return AnalysisResult(
        x=x,
        n=n,
        x_min=x_min,
        x_max=x_max,
        r_span=R,
        k=k,
        h=h,
        alpha=alpha,
        bins=bins,
        mean_grouped=mean_g,
        var_grouped=var_g,
        mean_raw=mean_raw,
        var_raw=var_raw,
    )


#  п. 9: критерий χ² (нормальность) 

def _merge_small_expected(obs, exp):
    """Если ожидаемая частота < 5, объединяем с соседним интервалом."""
    o = list(obs)
    e = list(exp)
    changed = True
    while changed and len(o) > 1:
        changed = False
        if e[0] < 5.0:
            o[1] += o[0]
            e[1] += e[0]
            del o[0]
            del e[0]
            changed = True
        elif e[-1] < 5.0:
            o[-2] += o[-1]
            e[-2] += e[-1]
            del o[-1]
            del e[-1]
            changed = True
        else:
            i = 1
            while i < len(o) - 1:
                if e[i] < 5.0:
                    if e[i - 1] <= e[i + 1]:
                        o[i - 1] += o[i]
                        e[i - 1] += e[i]
                        del o[i]
                        del e[i]
                    else:
                        o[i] += o[i + 1]
                        e[i] += e[i + 1]
                        del o[i + 1]
                        del e[i + 1]
                    changed = True
                    break
                i += 1
    return np.array(o), np.array(e)


def chi2_normality_gof(x, alpha) -> Tuple[float, int, float, bool]:
    """H0: X ~ N(μ, σ), параметры оцениваем по выборке."""
    data = [float(v) for v in np.asarray(x).flat]
    n = len(data)
    mu = sample_mean(data)
    sigma = math.sqrt(sample_variance(data))
    if sigma <= 0:
        return float("nan"), 0, float("nan"), False

    alpha_list = [float(a) for a in np.asarray(alpha).flat]
    obs = frequencies(data, alpha_list)
    obs = [float(c) for c in obs]

    dist = stats.norm(loc=mu, scale=sigma)
    probs = []
    for i in range(len(obs)):
        p = dist.cdf(alpha_list[i + 1]) - dist.cdf(alpha_list[i])
        if p < 1e-12:
            p = 1e-12
        probs.append(p)
    s_prob = sum(probs)
    probs = [p / s_prob for p in probs]
    exp = [p * n for p in probs]

    obs_m, exp_m = _merge_small_expected(obs, exp)
    df = len(obs_m) - 1 - 2
    if df < 1:
        return float("nan"), df, float("nan"), False

    chi2 = 0.0
    for o, e in zip(obs_m, exp_m):
        chi2 += (o - e) ** 2 / e
    p_value = 1.0 - stats.chi2.cdf(chi2, df)
    reject = p_value < 0.05
    return chi2, df, p_value, reject


#  п. 10: H0: σ² = 1 

def variance_test_sigma2(x, sigma0_sq: float = 1.0, alpha_level: float = 0.05):
    data = [float(v) for v in np.asarray(x).flat]
    n = len(data)
    if n < 2:
        return float("nan"), float("nan"), float("nan"), False

    s2 = sample_variance(data)
    chi2_stat = (n - 1) * s2 / sigma0_sq
    df = n - 1
    chi_lo = stats.chi2.ppf(alpha_level / 2, df)
    chi_hi = stats.chi2.ppf(1 - alpha_level / 2, df)
    reject = not (chi_lo <= chi2_stat <= chi_hi)
    return chi2_stat, chi_lo, chi_hi, reject


#  п. 11: H0: μ = 0 

def mean_test_zero(x, alpha_level: float = 0.05):
    data = [float(v) for v in np.asarray(x).flat]
    n = len(data)
    if n < 2:
        return float("nan"), float("nan"), float("nan"), False

    t_stat, p_value = stats.ttest_1samp(data, 0.0)
    reject = p_value < alpha_level
    return float(t_stat), float(p_value), alpha_level, reject


# дополнительно для задания 9 (не в ТЗ, но есть в программе)

def shapiro_wilk_normal_hint(x) -> Tuple[float, float, bool]:
    data = [float(v) for v in np.asarray(x).flat]
    n = len(data)
    if n < 3 or n > 5000:
        return float("nan"), float("nan"), False
    w, p = stats.shapiro(data)
    return float(w), float(p), p < 0.05
