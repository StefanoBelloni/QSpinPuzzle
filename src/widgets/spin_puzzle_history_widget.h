
#ifndef SPIN_PUZZLE_HISDTORY_WIDGET_H
#define SPIN_PUZZLE_HISDTORY_WIDGET_H

#include <QDialog>
#include <QScrollArea>
#include <QWidget>

#include "puzzle/spin_puzzle_game.h"
#include "puzzle/spin_puzzle_record.h"

class SpinPuzzleWidget;
class QStackedWidget;
class QComboBox;
class QPushButton;

/**
 * @brief  Class to display the saved puzzle to select them
 */
class SpinPuzzleHistoryWidget : public QWidget
{
  Q_OBJECT
public:
  SpinPuzzleHistoryWidget(int win_width,
                          int win_heigth,
                          SpinPuzzleWidget* parent,
                          const std::vector<puzzle::SpinPuzzleRecord>& games);

private:
  QWidget* get_puzzle(const puzzle::SpinPuzzleRecord& game);
  void populateStackedWidget();
  void import_game();

  void setup_select_btn(QPushButton* btn);
  void setup_export_btn(QPushButton* btn);
  void setup_delete_btn(QPushButton* btn);

  void setup_replay_btn(QPushButton* btn);

  int m_win_width;
  int m_win_heigth;

  SpinPuzzleWidget* m_parent;
  QStackedWidget* m_stackedWidget;
  QComboBox* m_pageComboBox;
  std::vector<puzzle::SpinPuzzleRecord> m_games;
};

#endif // SPIN_PUZZLE_HISDTORY_WIDGET_H