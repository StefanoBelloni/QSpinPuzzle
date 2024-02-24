#include "spin_puzzle_window.h"

#include <QGridLayout>
#include <QGuiApplication>
#include <QPushButton>
#include <QScreen>

SpinPuzzleWindow::SpinPuzzleWindow(QWidget *parent) : QMainWindow(parent) {
  QScreen *screen = QGuiApplication::primaryScreen();
  QRect screenGeometry = screen->geometry();
#ifdef Q_OS_ANDROID
  int min_size = std::min(screenGeometry.height(), screenGeometry.width());
#else
  int min_size =
      std::min(screenGeometry.height() / 2, screenGeometry.width() / 2);
#endif
  m_spinPuzzleWidget = new SpinPuzzleWidget(min_size, this);
  this->setCentralWidget(m_spinPuzzleWidget);
  m_spinPuzzleWidget->setFocus();
}

SpinPuzzleWindow::~SpinPuzzleWindow() {}
