#ifndef QSPINPUZZLEWINDOW_H
#define QSPINPUZZLEWINDOW_H

#include "spin_puzzle_widget.h"
#include <QMainWindow>

class SpinPuzzleWindow : public QMainWindow {
  Q_OBJECT

public:
  SpinPuzzleWindow(QWidget *parent = nullptr);
  ~SpinPuzzleWindow();

private:
  SpinPuzzleWidget *m_spinPuzzleWidget;
};
#endif // QSPINPUZZLEWINDOW_H