#include "spin_puzzle_window.h"

#include <QAction>
#include <QGridLayout>
#include <QGuiApplication>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QScreen>

#define CREATE_SPIN_PUZZLE_ACTION(__name__, __text__)                          \
  auto* m_##__name__ = new QAction(tr((__text__)), this);                      \
  connect(m_##__name__,                                                        \
          &QAction::triggered,                                                 \
          m_spinPuzzleWidget,                                                  \
          &SpinPuzzleWidget::__name__);                                        \
  m_menu->addAction(m_##__name__);

#define CREATE_SPIN_PUZZLE_ACTION_THIS(__name__, __text__)                     \
  auto* m_##__name__ = new QAction(tr((__text__)), this);                      \
  connect(                                                                     \
    m_##__name__, &QAction::triggered, this, &SpinPuzzleWindow::__name__);     \
  m_menu->addAction(m_##__name__);

#define CREATE_SPIN_PUZZLE_SEPARATOR() m_menu->addSeparator();

SpinPuzzleWindow::SpinPuzzleWindow(QWidget* parent)
  : QMainWindow(parent)
{
  setup_central_widget();
  create_menu();
  m_spinPuzzleWidget->setFocus();
}

SpinPuzzleWindow::~SpinPuzzleWindow() {}

void
SpinPuzzleWindow::setup_central_widget()
{
  QScreen* screen = QGuiApplication::primaryScreen();
  QRect screenGeometry = screen->geometry();
  m_spinPuzzleWidget = new SpinPuzzleWidget(
#ifdef Q_OS_ANDROID
    screenGeometry.width(),
    screenGeometry.height(),
#else
    screenGeometry.width() / 2,
    screenGeometry.height() / 2,
#endif
    SpinPuzzleWidget::TypePuzzle::GAME,
    this);
  this->setCentralWidget(m_spinPuzzleWidget);
}

void
SpinPuzzleWindow::create_menu()
{
  m_menu = menuBar()->addMenu(tr("Menu"));

  CREATE_SPIN_PUZZLE_ACTION(start_game, "Start Game (▶)");
  CREATE_SPIN_PUZZLE_ACTION(reset_game, "Reset Game (⟲)");
  CREATE_SPIN_PUZZLE_ACTION(save_progress, "Save Progress (⇓)");
  CREATE_SPIN_PUZZLE_ACTION(load_latest_game, "Load Latest Game (⟪)");
  CREATE_SPIN_PUZZLE_SEPARATOR();
  CREATE_SPIN_PUZZLE_ACTION(exec_puzzle_records_dialog, "Records");
  CREATE_SPIN_PUZZLE_ACTION(import_game, "Import Game");
  CREATE_SPIN_PUZZLE_SEPARATOR();
  CREATE_SPIN_PUZZLE_ACTION(exec_puzzle_config_dialog, "Configuration");
  CREATE_SPIN_PUZZLE_ACTION(reset_file_app, "Reset App");
  CREATE_SPIN_PUZZLE_SEPARATOR();
  CREATE_SPIN_PUZZLE_ACTION_THIS(about, "About");
  CREATE_SPIN_PUZZLE_SEPARATOR();
  CREATE_SPIN_PUZZLE_ACTION_THIS(quit, "Quit");
}

void
SpinPuzzleWindow::closeEvent(QCloseEvent* event)
{
  m_spinPuzzleWidget->closeEvent(event);
}

void
SpinPuzzleWindow::about()
{
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
}

void
SpinPuzzleWindow::quit()
{
  if (m_spinPuzzleWidget->quit()) {
    close();
  }
}
