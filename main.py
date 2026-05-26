"""Точка входа: выбор задания (1–3)."""

from __future__ import annotations

import argparse
import sys

import matplotlib.pyplot as plt

from lb1 import run as run_lb1


def _print_menu() -> None:
    print("Доступные задания:")
    print("  1 — ЛБ1: метод Эйлера (экспоненциальный рост)")
    print("  2 — (пока не реализовано)")
    print("  3 — (пока не реализовано)")
    print("  0 — выход")


def _run_task(choice: str) -> bool:
    """Выполнить задание. Возвращает False, если нужно завершить программу."""
    if choice in ("0", "q", "quit", "exit"):
        return False
    if choice == "1":
        run_lb1()
        return True
    if choice == "2":
        print("Задание 2 ещё не реализовано.")
        return True
    if choice == "3":
        print("Задание 3 ещё не реализовано.")
        return True
    print(f"Неизвестный номер: {choice!r}. Введите 1, 2, 3 или 0.")
    return True


def main(argv: list[str] | None = None) -> None:
    plt.rcParams["font.family"] = "DejaVu Sans"

    parser = argparse.ArgumentParser(description="Выбор задания курса")
    parser.add_argument(
        "task",
        nargs="?",
        choices=("1", "2", "3"),
        help="номер задания (1–3); без аргумента — интерактивный режим",
    )
    args = parser.parse_args(argv)

    if args.task is not None:
        if args.task == "1":
            run_lb1()
        elif args.task == "2":
            print("Задание 2 ещё не реализовано.", file=sys.stderr)
            sys.exit(1)
        else:
            print("Задание 3 ещё не реализовано.", file=sys.stderr)
            sys.exit(1)
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
