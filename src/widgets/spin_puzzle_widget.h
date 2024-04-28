#ifndef SPIN_PUZZLE_WIDGET_H
#define SPIN_PUZZLE_WIDGET_H

#include <QPushButton>
#include <QWidget>

#include "puzzle/spin_configuration.h"
#include "puzzle/spin_puzzle_definitions.h"
#include "puzzle/spin_puzzle_game.h"
#include "puzzle/spin_puzzle_record.h"
#include "spin_puzzle_history_widget.h"

#define SAVE_LOAD_DATA 1

class SpinPuzzleHistoryWidget;
class SpinPuzzleConfigurationWidget;

/**
 * @brief  Class to display the SpinPuzzle Game
 *
 * @note  It's main use is to process the input from the user and call the
 *        corresponding function of \ref puzzle::SpinPuzzleSide
 *
 * A SpinPuzzleGame is composed by two sides, one is the active one, that is
 * display, while the opposite might change if a leaf is spin.
 *
 * It is possible to execute the allowed moves with mouse or keybord:
 * In particular:
 *    * move the marbles:
 *        - mouse: press the mouse button (right) on a marble and drag it in
 *          the desired direction
 *        - keyboard: select first the \ref puzzle::LEAF (N, E; W) or the
 * internal circle (I) to use move the marble right or left with the arrows
 *    * spin the leaf:
 *         - press the button on the leaf with the mouse
 *         - select the desired \ref puzzle::LEAF and press PAGE UP or PAGE DOWN
 *    * twist the side
 *         - press the button (twist) on the side with the mouse
 *         - press the key P
 *
 */
class SpinPuzzleWidget : public QWidget
{
  friend class SpinPuzzleHistoryWidget;
  friend class SpinPuzzleConfigurationWidget;
  Q_OBJECT
public:
  SpinPuzzleWidget(int win_width,
                   int win_heigth,
                   bool m_allow_play = true,
                   QWidget* parent = nullptr);

  void paintEvent(QPaintEvent* ev) override;
  void mouseMoveEvent(QMouseEvent* ev) override;
  void mousePressEvent(QMouseEvent* e) override;
  void keyPressEvent(QKeyEvent* e) override;
  void resizeEvent(QResizeEvent* e) override;
  void closeEvent(QCloseEvent* event) override;

  void exec_puzzle_records_dialog();
  void exec_puzzle_config_dialog();
  void reset_file_app();
  bool import_game();

  void start_game();
  void reset();
  void load_latest_game();
  bool save_progress();
  bool quit();

private:
  void set_size(int win_width, int win_height);
  double get_radius_internal() const;
  void create_polygon(QPolygon& polygon);
  void paint_puzzle_section(QPainter& painter,
                            QColor color,
                            QColor color_internal,
                            QColor color_body) const;

  void next_section(QPainter& painter, int angle = 120) const;

  void paint_background();
  void paint_status();
  void paint_marbles();
  void paint_timer();
  void paint_game();

  void paint_internal_circular_guide(QPainter& painter, QColor color) const;
  void do_paint_marbles(QPainter& painter);
  void do_paint_marbles_on_border(QPainter& painter);

  void paint_marbles_on_leaf(QPainter& painter, puzzle::LEAF leaf);
  void paint_marbles_on_internal_circle(QPainter& painter, puzzle::LEAF leaf);
  void paint_marble(double shift_local,
                    QPainter& painter,
                    puzzle::SpinPuzzleSide<>::iterator it,
                    const QPoint center,
                    size_t n,
                    const int r);
  void paint_congratulation(QPainter& painter);

  bool mouse_event_inside_internal_circle(QPoint position);
  bool mouse_event_inside_leaf(QPoint position,
                               QPoint center,
                               puzzle::LEAF leaf);
  void do_spin_north();
  void do_spin_east();
  void do_spin_west();
  void do_start_game(int shuffle_level);

  void update_configuration(const puzzle::Configuration& config);
  bool load_configuration();

  void connect_play_buttons();
  void create_play_buttons();

  bool processKey(int key, double fraction_angle);

  void delete_history_popup();
  void delete_config_popup();
  void stop_spinning_winning();
  void set_game(int time, const puzzle::SpinPuzzleGame& game);
  void set_game(const puzzle::SpinPuzzleGame& game);
  void start_timer();
  void set_elapsed_time(int t);
  puzzle::SpinPuzzleGame& get_game();

  void start_with_game(const puzzle::SpinPuzzleGame& game);
  void reset_leaf_colors();
  void load(int index);
  void load(int index, puzzle::SpinPuzzleGame& game);
  void store_puzzle_begin();
  bool store_puzzle_record() const;
  bool store_puzzle_record(const puzzle::SpinPuzzleRecord& record) const;
  bool store_puzzles_record(std::vector<puzzle::SpinPuzzleRecord> games) const;
  // return max time
  int load_records(std::vector<puzzle::SpinPuzzleRecord>& games) const;

  bool can_rotate_internal() const;
  bool is_mause_on_leaf_marbles(QPoint pos, QPoint center) const;
  double get_scaled_angle(double a1, double a2) const;
  QColor toQtColor(puzzle::Color value);
  QColor toMarbleColor(puzzle::Color value);

  double get_height_button_bottom() const;
  double get_width_button_bottom() const;
  double get_length_status_square() const;

  std::string get_puzzle_file() const;
  std::string get_records_puzzle_file() const;
  std::string get_current_puzzle_file() const;
  std::string get_config_puzzle_file() const;

  // plot puzzle body
  uint32_t m_length;
  const int width = 15;
  const int max_shift_status = 5;
  int radius_marble = width;
  QPolygon m_polygon;
  // to calculate the update
  QPoint m_lastPositionMause;
  // Game
  puzzle::SpinPuzzleGame m_game;

  QPushButton* m_reset_btn = nullptr;
  QPushButton* m_start_btn = nullptr;
  QPushButton* m_twist_side = nullptr;
  QPushButton* m_spin_north = nullptr;
  QPushButton* m_spin_east = nullptr;
  QPushButton* m_spin_west = nullptr;

  QPushButton* m_load_btn = nullptr;
  QPushButton* m_save_btn = nullptr;
  // QPushButton *m_load_records_btn = nullptr;

  // maybe use a shared_pointer ...
  SpinPuzzleHistoryWidget* m_history_widget = nullptr;
  SpinPuzzleConfigurationWidget* m_config_widget = nullptr;

  QTimer* m_timer;
  QTimer* m_congratulation_timer;
  int m_elapsed_time = 0;

  bool m_solved = false;
  bool m_paint_congratulations = false;

  int m_tx = 0;
  int m_ty = 0;
  int m_win_width = 0;
  int m_win_height = 0;
  bool m_allow_play = true;
  const int m_max_saved_games = 10;
  int m_rotation_congratulation = 0;

  using ColorsSide = std::array<QColor, 3>;

  std::array<ColorsSide, 2> m_colors_leaves;
  std::array<ColorsSide, 2> m_colors_leaves_internal;
  std::array<ColorsSide, 2> m_colors_leaves_body;

  puzzle::Configuration m_config;
};

#endif // SPIN_PUZZLE_WIDGET_H
