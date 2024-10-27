#ifndef QSPINPUZZLEWINDOW_H
#define QSPINPUZZLEWINDOW_H

#include <QMainWindow>

#include "spin_puzzle_widget.h"

class QAction;
class QMenu;

class SpinPuzzleWindow : public QMainWindow
{
  Q_OBJECT

public:
  SpinPuzzleWindow(QWidget* parent = nullptr);
  ~SpinPuzzleWindow();
  void closeEvent(QCloseEvent* event) override;

private:
  void create_menu();
  void setup_central_widget();
  void about();
  void quit();

  SpinPuzzleWidget* m_spinPuzzleWidget;
  QMenu* m_menu;
};
#endif // QSPINPUZZLEWINDOW_H
