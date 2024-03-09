#include "spin_puzzle_window.h"

#include <QAction>
#include <QGridLayout>
#include <QGuiApplication>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
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
      true, this);
  this->setCentralWidget(m_spinPuzzleWidget);

  create_actions();
  create_menus();
  m_spinPuzzleWidget->setFocus();
}

SpinPuzzleWindow::~SpinPuzzleWindow() {}

void SpinPuzzleWindow::create_actions() {
  m_records_action = new QAction(tr("Records"), this);
  m_import = new QAction(tr("Import Game"), this);

  m_start_game = new QAction(tr("Start Game"), this);
  m_reset_game = new QAction(tr("Reset Game"), this);
  m_save_game = new QAction(tr("Save Progress"), this);
  m_load_game = new QAction(tr("Load Latest Game"), this);

  m_about = new QAction(tr("About"), this);
  m_quit = new QAction(tr("Quit"), this);

  m_reset_application = new QAction(tr("Reset App"), this);

  connect(m_records_action, &QAction::triggered, m_spinPuzzleWidget,
          &SpinPuzzleWidget::exec_puzzle_records_dialog);
  connect(m_import, &QAction::triggered, m_spinPuzzleWidget,
          &SpinPuzzleWidget::import_game);

  connect(m_start_game, &QAction::triggered, m_spinPuzzleWidget,
          &SpinPuzzleWidget::start_game);
  connect(m_reset_game, &QAction::triggered, m_spinPuzzleWidget,
          &SpinPuzzleWidget::reset);
  connect(m_save_game, &QAction::triggered, m_spinPuzzleWidget,
          &SpinPuzzleWidget::save_progress);
  connect(m_load_game, &QAction::triggered, m_spinPuzzleWidget,
          &SpinPuzzleWidget::load_latest_game);

  connect(m_about, &QAction::triggered, this, [this] {
    QMessageBox::about(this, "QSpinPuzzle", R"(
QSpinPuzzle is a strategic solitaire game 
played on a unique board where the objective 
is to group a set of marbles according to 
their colors..

Press START to shuffle the marbles
GROUP them by the COLOR 
in the 6 leaves, 3 for each side:

- You can move marbles using the mouse 
  by dragging them or by using the keyboard:
- Select the component to operate on by pressing:
  * N: North leaf
  * E East leaf
  * W: West leaf
  * I: Internal circle
- Rotate the marbles using the arrow keys.
- Check the selected component in the status 
  on the left corner
- Press the "twist" button (top left) or the P key 
  to change the active side.
- Spin a leaf by pressing the corresponding button 
  on the leaf or selecting the leaf with the 
  keyboard and pressing PageUp or PageDown.

                HAVE FUN !!! 
)");
  });
  connect(m_quit, &QAction::triggered, this, [this] {
    if (m_spinPuzzleWidget->quit()) {
      close();
    }
  });

  connect(m_reset_application, &QAction::triggered, m_spinPuzzleWidget,
          &SpinPuzzleWidget::reset_file_app);
}

void SpinPuzzleWindow::create_menus() {
  m_menu = menuBar()->addMenu(tr("Menu"));
  m_menu->addAction(m_start_game);
  m_menu->addAction(m_reset_game);
  m_menu->addAction(m_save_game);
  m_menu->addAction(m_load_game);
  m_menu->addSeparator();
  m_menu->addAction(m_records_action);
  m_menu->addAction(m_import);
  m_menu->addSeparator();
  m_menu->addAction(m_about);
  m_menu->addSeparator();
  m_menu->addAction(m_quit);
  // m_menu->addAction(m_reset_application);
}
