#include "spin_puzzle_window.h"

#include <QGridLayout>
#include <QGuiApplication>
#include <QPushButton>
#include <QScreen>

SpinPuzzleWindow::SpinPuzzleWindow(QWidget *parent) : QMainWindow(parent) {
  QScreen *screen = QGuiApplication::primaryScreen();
  QRect screenGeometry = screen->geometry();
  m_spinPuzzleWidget = new SpinPuzzleWidget(
#ifdef Q_OS_ANDROID
      screenGeometry.width(), screenGeometry.height(),
#else
      screenGeometry.width() / 2, screenGeometry.height() / 2,
#endif
      this);
  this->setCentralWidget(m_spinPuzzleWidget);
  m_spinPuzzleWidget->setFocus();
}

SpinPuzzleWindow::~SpinPuzzleWindow() {}
