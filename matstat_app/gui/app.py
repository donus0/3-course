"""Запуск приложения."""

from __future__ import annotations

import sys

from PyQt6.QtWidgets import QApplication

from matstat_app.gui.main_window import MatstatMainWindow


def main() -> int:
    app = QApplication(sys.argv)
    w = MatstatMainWindow()
    w.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
