#ifndef SPIN_PUZZLE_WIDGET_H
#define SPIN_PUZZLE_WIDGET_H

#include <QPushButton>
#include <QWidget>

#include "puzzle/spin_puzzle_definitions.h"
#include "puzzle/spin_puzzle_game.h"

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
class SpinPuzzleWidget : public QWidget {
  Q_OBJECT
public:
  SpinPuzzleWidget(uint32_t length, QWidget *parent = nullptr);

  void paintEvent(QPaintEvent *ev) override;
  void mouseMoveEvent(QMouseEvent *ev) override;
  void mousePressEvent(QMouseEvent *e) override;
  void keyPressEvent(QKeyEvent *e) override;
  void resizeEvent(QResizeEvent *e) override;

private:
  void set_size(int length);
  int get_radius_internal() const;
  void create_polygon(QPolygon &polygon);
  void paint_puzzle_section(QPainter &painter, QColor color) const;

  void next_section(QPainter &painter, int angle = 120) const;

  void paint_internal_circular_guide(QPainter &painter, QColor color) const;
  void paint_marbles(QPainter &painter);
  void do_paint_marbles(QPainter &painter);
  void do_paint_marbles_on_border(QPainter &painter);

  void paint_status();
  void paint_marbles_on_leaf(QPainter &painter, puzzle::LEAF leaf);
  void paint_marbles_on_internal_circle(QPainter &painter, puzzle::LEAF leaf);
  void paint_marble(double shift_local, QPainter &painter,
                    puzzle::SpinPuzzleSide<>::iterator it, const QPoint center,
                    size_t n, const int r);

  bool mouse_event_inside_internal_circle(QMouseEvent *ev);
  bool mouse_event_inside_leaf(QMouseEvent *ev, QPoint center,
                               puzzle::LEAF leaf);

  bool processKey(int key, double fraction_angle);
  void shuffle();
  void reset();

  bool can_rotate_internal();
  bool is_mause_on_leaf_marbles(QMouseEvent *ev, QPoint center);
  double get_speed();
  QColor toQtColor(puzzle::Color value);

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

  const double m_speed = 350.0;

  QPushButton *reset_btn = nullptr;
  QPushButton *pb = nullptr;
  QPushButton *twist_side = nullptr;
  QPushButton *spin_north = nullptr;
  QPushButton *spin_east = nullptr;
  QPushButton *spin_west = nullptr;
};

#endif // SPIN_PUZZLE_WIDGET_H
