
#ifndef SPIN_PUZZLE_REPLAY_WIDGET_H
#define SPIN_PUZZLE_REPLAY_WIDGET_H

#include <QDialog>
#include <QWidget>

#include "puzzle/spin_game_recorder.h"
#include "puzzle/spin_puzzle_game.h"

class SpinPuzzleWidget;
class QPushButton;
class QTimer;

/**
 * @brief  Class to replay teh solution of a puzzle
 */
class SpinPuzzleReplayWidget : public QDialog
{
  Q_OBJECT
public:
  SpinPuzzleReplayWidget(int win_width,
                         int win_heigth,
                         SpinPuzzleWidget* parent,
                         const puzzle::Recorder& recorder);

private:
  void setup_ui();
  void setup_layout();
  void setup_buttons();
  void create_buttons();

  int m_win_width;
  int m_win_heigth;

  SpinPuzzleWidget* m_parent;
  QPushButton* m_play;
  QPushButton* m_rewind;
  QPushButton* m_next;
  QPushButton* m_faster;
  QPushButton* m_quit;

  puzzle::Recorder m_recorder;
  puzzle::SpinPuzzleGame m_game;

  SpinPuzzleWidget* m_spinPuzzleWidget;
  QTimer* m_timer;
  size_t m_speed{ 10 };
  size_t m_speed_max{ 20 };
  size_t m_timer_step = 40;
};

#endif // SPIN_PUZZLE_HISDTORY_WIDGET_H