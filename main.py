"""Точка входа: выбор лабораторной работы (1–3)."""

from __future__ import annotations

import argparse
import sys

import matplotlib.pyplot as plt

from lr1 import run as run_lr1
from lr2 import run as run_lr2
from lr3 import run as run_lr3


def _print_menu() -> None:
    print("Доступные задания:")
    print("  1 — ЛР1: метод Эйлера (экспоненциальный рост)")
    print("  2 — ЛР2: Эйлер, расширенный Эйлер, Рунге–Кутта 4")
    print("  3 — ЛР3: аппроксимация данных")
    print("  0 — выход")


def _run_task(choice: str) -> bool:
    """Выполнить задание. Возвращает False, если нужно завершить программу."""
    if choice in ("0", "q", "quit", "exit"):
        return False
    if choice == "1":
        run_lr1()
        return True
    if choice == "2":
        run_lr2()
        return True
    if choice == "3":
        run_lr3()
        return True
    print(f"Неизвестный номер: {choice!r}. Введите 1, 2, 3 или 0.")
    return True


def main(argv: list[str] | None = None) -> None:
    plt.rcParams["font.family"] = "DejaVu Sans"

    parser = argparse.ArgumentParser(description="Выбор лабораторной работы")
    parser.add_argument(
        "task",
        nargs="?",
        choices=("1", "2", "3"),
        help="номер ЛР (1–3); без аргумента — интерактивный режим",
    )
    args = parser.parse_args(argv)

    if args.task is not None:
        if args.task == "1":
            run_lr1()
        elif args.task == "2":
            run_lr2()
        else:
            run_lr3()
        return

    while True:
        _print_menu()
        try:
            choice = input("Номер задания: ").strip()
        except (EOFError, KeyboardInterrupt):
            print()
            break
        if not _run_task(choice):
            break


if __name__ == "__main__":
    main()
