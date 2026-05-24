# ЛР №2: Вероятностные и стохастические модели анализа данных

from __future__ import annotations

import math
import random
from typing import List, Optional, Sequence, Tuple

# --- вспомогательное ---


def gcd(a: int, b: int) -> int:
    return math.gcd(int(a), int(b))


def is_prime(n: int) -> bool:
    n = int(n)
    if n < 2:
        return False
    if n < 4:
        return True
    if n % 2 == 0:
        return False
    r = int(math.isqrt(n))
    d = 3
    while d <= r:
        if n % d == 0:
            return False
        d += 2
    return True


def factorize(n: int) -> List[Tuple[int, int]]:
    """Разложение n > 0 на простые множители: [(p, степень), ...]."""
    n = int(n)
    if n < 1:
        raise ValueError("n должно быть ≥ 1")
    if n == 1:
        return []
    factors: List[Tuple[int, int]] = []
    x = n
    p = 2
    while p * p <= x:
        if x % p == 0:
            e = 0
            while x % p == 0:
                x //= p
                e += 1
            factors.append((p, e))
        p = 3 if p == 2 else p + 2
    if x > 1:
        factors.append((x, 1))
    return factors


def mod_inverse(a: int, m: int) -> Optional[int]:
    a = int(a) % int(m)
    if m <= 1 or gcd(a, m) != 1:
        return None
    return pow(a, -1, int(m))


def _distinct_witnesses(n: int, count: int, rng: random.Random) -> List[int]:
    if n < 4:
        return []
    lo, hi = 2, n - 2
    if hi - lo + 1 < count:
        raise ValueError(f"Нужно {count} различных свидетелей из [{lo}, {hi}]")
    pool = list(range(lo, hi + 1))
    rng.shuffle(pool)
    return sorted(pool[:count])


# --- п. 1: φ(n) ---


def euler_phi_definition(n: int) -> int:
    n = int(n)
    if n < 1:
        raise ValueError("n ≥ 1")
    if n == 1:
        return 1
    return sum(1 for k in range(1, n + 1) if gcd(k, n) == 1)


def euler_phi_factorization(n: int) -> int:
    n = int(n)
    if n < 1:
        raise ValueError("n ≥ 1")
    if n == 1:
        return 1
    result = n
    for p, _ in factorize(n):
        result = result // p * (p - 1)
    return result


def euler_phi_dft(n: int) -> float:
    """φ(n) = Σ_{k=1}^{n} gcd(k,n)·cos(2πk/n)."""
    n = int(n)
    if n < 1:
        raise ValueError("n ≥ 1")
    s = 0.0
    for k in range(1, n + 1):
        s += gcd(k, n) * math.cos(2.0 * math.pi * k / n)
    return s


def euler_phi_all(n: int) -> Tuple[int, int, float]:
    d = euler_phi_definition(n)
    f = euler_phi_factorization(n)
    t = euler_phi_dft(n)
    return d, f, t


# --- п. 2: тест Ферма ---


def fermat_test(n: int, witnesses: int, seed: Optional[int] = None) -> Tuple[bool, List[int], List[str]]:
    n = int(n)
    if n < 2:
        return False, [], ["n < 2 — не простое"]
    if n == 2:
        return True, [], ["2 — простое"]
    if n % 2 == 0:
        return False, [], ["чётное — составное"]
    rng = random.Random(seed)
    ws = _distinct_witnesses(n, witnesses, rng)
    details: List[str] = []
    for a in ws:
        if pow(a, n - 1, n) != 1:
            details.append(f"a={a}: a^(n-1) mod n ≠ 1 → составное")
            return False, ws, details
        details.append(f"a={a}: условие Ферма выполнено")
    return True, ws, details + ["вероятно простое (тест не отверг)"]


# --- п. 3: Лежандр, Якоби, Соловей–Штрассен ---


def legendre_symbol(a: int, p: int) -> int:
    a = int(a) % int(p)
    p = int(p)
    if p < 2 or not is_prime(p) or p == 2:
        raise ValueError("p — нечётное простое ≥ 3")
    if a == 0:
        return 0
    ls = pow(a, (p - 1) // 2, p)
    if ls == 1:
        return 1
    if ls == p - 1:
        return -1
    raise ValueError("символ Лежандра не определён")


def jacobi_symbol(a: int, n: int) -> int:
    a = int(a) % int(n)
    n = int(n)
    if n < 1 or n % 2 == 0:
        raise ValueError("n — нечётное положительное")
    if gcd(a, n) > 1:
        return 0
    if n == 1:
        return 1
    t = 1
    while a != 0:
        while a % 2 == 0:
            a //= 2
            if n % 8 in (3, 5):
                t = -t
        a, n = n, a
        if a % 4 == 3 and n % 4 == 3:
            t = -t
        a %= n
    return t if n == 1 else 0


def solovay_strassen_test(
    n: int, witnesses: int, seed: Optional[int] = None
) -> Tuple[bool, List[int], List[str]]:
    n = int(n)
    if n < 2:
        return False, [], ["n < 2"]
    if n == 2:
        return True, [], ["2 — простое"]
    if n % 2 == 0:
        return False, [], ["чётное — составное"]
    rng = random.Random(seed)
    ws = _distinct_witnesses(n, witnesses, rng)
    details: List[str] = []
    for a in ws:
        if gcd(a, n) > 1:
            details.append(f"a={a}: gcd(a,n)≠1 → составное")
            return False, ws, details
        jac = jacobi_symbol(a, n)
        rhs = pow(a, (n - 1) // 2, n)
        if rhs == n - 1:
            rhs = -1
        if jac != rhs:
            details.append(f"a={a}: (a/n)={jac} ≠ a^((n-1)/2) mod n → составное")
            return False, ws, details
        details.append(f"a={a}: согласовано")
    return True, ws, details + ["вероятно простое"]


# --- п. 4: Миллер–Рабин ---


def _miller_rabin_witness(a: int, n: int, s: int, d: int) -> bool:
    x = pow(a, d, n)
    if x == 1 or x == n - 1:
        return True
    for _ in range(s - 1):
        x = (x * x) % n
        if x == n - 1:
            return True
    return False


def miller_rabin_test(
    n: int, witnesses: int, seed: Optional[int] = None
) -> Tuple[bool, List[int], List[str]]:
    n = int(n)
    if n < 2:
        return False, [], ["n < 2"]
    if n == 2:
        return True, [], ["2 — простое"]
    if n % 2 == 0:
        return False, [], ["чётное — составное"]
    s, d = 0, n - 1
    while d % 2 == 0:
        s += 1
        d //= 2
    rng = random.Random(seed)
    ws = _distinct_witnesses(n, witnesses, rng)
    details: List[str] = []
    for a in ws:
        if not _miller_rabin_witness(a, n, s, d):
            details.append(f"a={a}: свидетель составности")
            return False, ws, details
        details.append(f"a={a}: не свидетель составности")
    return True, ws, details + ["вероятно простое"]


# --- п. 5–7: периоды генераторов ---


def lcg_period(a: int, b: int, m: int, x0: int = 0) -> int:
    m = int(m)
    if m <= 0:
        return 0
    a, b, x0 = int(a) % m, int(b) % m, int(x0) % m
    seen = {}
    x, step = x0, 0
    while x not in seen:
        seen[x] = step
        step += 1
        x = (a * x + b) % m
        if step > m:
            return step - seen.get(x, 0)
    return step - seen[x]


def icg_next_state(a: int, b: int, mod: int, x: int) -> Optional[int]:
    mod = int(mod)
    if mod <= 1:
        return None
    a, b, x = int(a) % mod, int(b) % mod, int(x) % mod
    if is_prime(mod):
        if x == 0:
            return b % mod
        inv = mod_inverse(x, mod)
        if inv is None:
            return None
        return (a * inv + b) % mod
    if gcd(a, mod) != 1:
        return None
    if x == 0:
        return None
    inv = mod_inverse(x, mod)
    if inv is None:
        return None
    return (a * inv + b) % mod


def icg_period(a: int, b: int, mod: int, x0: int = 1) -> int:
    mod = int(mod)
    if mod <= 1:
        return 0
    x0 = int(x0) % mod
    seen = {}
    x, step = x0, 0
    while x not in seen:
        seen[x] = step
        step += 1
        nx = icg_next_state(a, b, mod, x)
        if nx is None:
            return step
        x = nx
        if step > mod * mod:
            return step - seen.get(x, 0)
    return step - seen[x]


def _is_blum_prime(p: int) -> bool:
    return is_prime(p) and p % 4 == 3


def bbs_period(p: int, q: int, x0: int) -> int:
    p, q = int(p), int(q)
    if not _is_blum_prime(p) or not _is_blum_prime(q):
        raise ValueError("p и q — простые, ≡ 3 (mod 4)")
    M = p * q
    x0 = int(x0) % M
    if gcd(x0, M) != 1:
        raise ValueError("x₀ должно быть взаимно просто с M")
    seen = {}
    x, step = x0, 0
    while x not in seen:
        seen[x] = step
        step += 1
        x = (x * x) % M
        if step > M:
            return step - seen.get(x, 0)
    return step - seen[x]


def sample_periods_lcg(
    samples: int, seed: Optional[int] = None, mod_bits: int = 12
) -> Tuple[List[int], float]:
    """Случайные (a,b,m) из Z_{2^mod_bits}; D(P) — выборочная дисперсия периодов."""
    rng = random.Random(seed)
    cap = 1 << mod_bits
    periods: List[int] = []
    for _ in range(samples):
        m = rng.randint(2, cap - 1) if cap > 2 else 2
        a = rng.randint(0, cap - 1)
        b = rng.randint(0, cap - 1)
        periods.append(lcg_period(a, b, m, 0))
    if len(periods) < 2:
        return periods, 0.0
    mean = sum(periods) / len(periods)
    var = sum((t - mean) ** 2 for t in periods) / (len(periods) - 1)
    return periods, var


def sample_periods_icg(
    samples: int, seed: Optional[int] = None, mod_bits: int = 12
) -> Tuple[List[int], float]:
    rng = random.Random(seed)
    cap = 1 << mod_bits
    periods: List[int] = []
    for _ in range(samples):
        mod = rng.randint(2, cap - 1) if cap > 2 else 2
        a = rng.randint(0, cap - 1)
        b = rng.randint(0, cap - 1)
        periods.append(icg_period(a, b, mod, 1))
    if len(periods) < 2:
        return periods, 0.0
    mean = sum(periods) / len(periods)
    var = sum((t - mean) ** 2 for t in periods) / (len(periods) - 1)
    return periods, var


def sample_periods_bbs(
    samples: int, seed: Optional[int] = None, prime_bits: int = 16
) -> Tuple[List[int], float]:
    rng = random.Random(seed)
    cap = 1 << prime_bits
    periods: List[int] = []
    attempts = 0
    while len(periods) < samples and attempts < samples * 200:
        attempts += 1
        p = rng.randint(3, cap)
        if not _is_blum_prime(p):
            continue
        q = rng.randint(3, cap)
        if not _is_blum_prime(q) or p == q:
            continue
        M = p * q
        x0 = rng.randint(1, M - 1)
        if gcd(x0, M) != 1:
            continue
        try:
            periods.append(bbs_period(p, q, x0))
        except ValueError:
            continue
    if len(periods) < 2:
        return periods, 0.0
    mean = sum(periods) / len(periods)
    var = sum((t - mean) ** 2 for t in periods) / (len(periods) - 1)
    return periods, var
