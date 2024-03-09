#ifndef QSPINPUZZLEWINDOW_H
#define QSPINPUZZLEWINDOW_H

#include "spin_puzzle_widget.h"
#include <QMainWindow>

class QAction;
class QMenu;

class SpinPuzzleWindow : public QMainWindow {
  Q_OBJECT

public:
  SpinPuzzleWindow(QWidget *parent = nullptr);
  ~SpinPuzzleWindow();

  void closeEvent(QCloseEvent *event) override {
    m_spinPuzzleWidget->closeEvent(event);
  }

private:
  void create_actions();
  void create_menus();

  SpinPuzzleWidget *m_spinPuzzleWidget;

  QAction *m_records_action;
  QAction *m_import;
  QAction *m_start_game;
  QAction *m_reset_game;
  QAction *m_save_game;
  QAction *m_load_game;
  QAction *m_about;
  QAction *m_quit;

  QAction *m_reset_application;

  QMenu *m_menu;
  QMenu *m_extra;
};
#endif // QSPINPUZZLEWINDOW_H
