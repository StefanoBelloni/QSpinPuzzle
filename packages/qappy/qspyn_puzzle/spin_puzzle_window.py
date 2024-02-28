from PySide6 import QtCore, QtWidgets, QtGui
from qspyn_puzzle.spin_puzzle_widget import QSpinPuzzleWidget

class QSpinPuzzleWindow(QtWidgets.QMainWindow):
    def __init__(self) -> None:
        super().__init__()
        screen = QtGui.QGuiApplication.primaryScreen()
        screenGeometry = screen.geometry()
        self.spin_puzzle = QSpinPuzzleWidget(
            screenGeometry.width() / 2,
            screenGeometry.height() / 2, 
            self)
        self.setCentralWidget(self.spin_puzzle)