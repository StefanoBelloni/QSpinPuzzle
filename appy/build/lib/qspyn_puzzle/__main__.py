import sys
import qspyn_puzzle
from PySide6 import QtWidgets, QtGui
from qspyn_puzzle.spin_puzzle_window import QSpinPuzzleWindow

if __name__ == "__main__":
    print("Running " + qspyn_puzzle.__application_name__ + ": version", qspyn_puzzle.__version__)
    app = QtWidgets.QApplication([])
    screen = QtGui.QGuiApplication.primaryScreen()
    screenGeometry = screen.geometry()
    size = min(screenGeometry.height() / 2, screenGeometry.width() / 2)

    widget = QSpinPuzzleWindow()
    widget.resize(size, size)
    widget.show()

    sys.exit(app.exec())