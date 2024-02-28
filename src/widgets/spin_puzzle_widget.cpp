#include "spin_puzzle_widget.h"
#include <stdint.h>

#include <QBrush>
#include <QGridLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QtMath>

#define DEBUG_EVENT 0
#define DEBUG_MARBLES 0
#define DEBUG_CONSISTENCY 0

template <typename T>
inline constexpr int signum(T x, std::false_type is_signed) {
  return T(0) < x;
}

template <typename T>
inline constexpr int signum(T x, std::true_type is_signed) {
  return (T(0) < x) - (x < T(0));
}

template <typename T> inline constexpr int signum(T x) {
  return signum(x, std::is_signed<T>());
}

SpinPuzzleWidget::SpinPuzzleWidget(int win_width, int win_heigth,
                                   QWidget *parent)
    : QWidget(parent) {

  reset_btn = new QPushButton("reset", this);
  connect(reset_btn, &QPushButton::released, this, &SpinPuzzleWidget::reset);

  pb = new QPushButton("START", this);
  connect(pb, &QPushButton::released, this, &SpinPuzzleWidget::shuffle);

  twist_side = new QPushButton("twist", this);
  connect(twist_side, &QPushButton::released, this, [this] {
    this->m_game.swap_side();
    update();
  });

  spin_north = new QPushButton("spin", this);
  connect(spin_north, &QPushButton::released, this, [this] {
    do_spin_north();
    update();
  });

  spin_east = new QPushButton("spin", this);
  connect(spin_east, &QPushButton::released, this, [this] {
    do_spin_east();
    update();
  });

  spin_west = new QPushButton("spin", this);
  connect(spin_west, &QPushButton::released, this, [this] {
    do_spin_west();
    update();
  });

  set_size(win_width, win_heigth);
  reset_leaf_colors();

  this->setFocus();
}

void SpinPuzzleWidget::do_spin_north() {
  int color_side = static_cast<uint8_t>(m_game.get_active_side());
  int color_opposite = (color_side + 1) % 2;
  std::swap(m_colors_leaves[color_side][0], m_colors_leaves[color_opposite][0]);
  std::swap(m_colors_leaves_internal[color_side][0],
            m_colors_leaves_internal[color_opposite][0]);
  this->m_game.spin_leaf(puzzle::LEAF::NORTH);
}

void SpinPuzzleWidget::do_spin_east() {
  int color_side = static_cast<uint8_t>(m_game.get_active_side());
  int color_opposite = (color_side + 1) % 2;
  std::swap(m_colors_leaves[color_side][1], m_colors_leaves[color_opposite][2]);
  std::swap(m_colors_leaves_internal[color_side][1],
            m_colors_leaves_internal[color_opposite][2]);
  this->m_game.spin_leaf(puzzle::LEAF::EAST);
}

void SpinPuzzleWidget::do_spin_west() {
  int color_side = static_cast<uint8_t>(m_game.get_active_side());
  int color_opposite = (color_side + 1) % 2;
  std::swap(m_colors_leaves[color_side][2], m_colors_leaves[color_opposite][1]);
  std::swap(m_colors_leaves_internal[color_side][2],
            m_colors_leaves_internal[color_opposite][1]);
  this->m_game.spin_leaf(puzzle::LEAF::WEST);
}

void SpinPuzzleWidget::reset_leaf_colors() {

  m_colors_leaves[0] = {Qt::red, Qt::blue, Qt::yellow};
  m_colors_leaves_internal[0] = {Qt::darkRed, Qt::darkBlue, Qt::darkYellow};
  m_colors_leaves_body[0] = {Qt::red, Qt::blue, Qt::yellow};

  m_colors_leaves[1] = {Qt::green, Qt::darkBlue, Qt::darkYellow};
  m_colors_leaves_internal[1] = {Qt::darkGreen, Qt::blue, Qt::yellow};
  m_colors_leaves_body[1] = {Qt::green, Qt::darkBlue, Qt::darkYellow};
}

void SpinPuzzleWidget::paint_status() {
  const int L = this->m_length;
  const auto &side = m_game.get_side(m_game.get_active_side());

  QPainter painter_status(this);

  if (side.get_trifoild_status() == puzzle::TREFOIL::BORDER_ROTATION ||
      side.is_border_rotation_possible()) {
    painter_status.setBrush(Qt::green);
  } else if (side.get_trifoild_status() == puzzle::TREFOIL::INVALID) {
    painter_status.setBrush(Qt::red);
  } else {
    painter_status.setBrush(Qt::yellow);
  }
  painter_status.drawRect(0, 0, L / 24, L / 24);

  painter_status.setBrush(
      (side.is_rotation_possible(puzzle::LEAF::NORTH)) ? Qt::green : Qt::red);
  painter_status.drawRect(0 + 0 * L / 24, L / 24, L / 24, L / 24);
  painter_status.setBrush(
      (side.is_rotation_possible(puzzle::LEAF::EAST)) ? Qt::green : Qt::red);
  painter_status.drawRect(0 + 1 * L / 24, L / 24, L / 24, L / 24);
  painter_status.setBrush(
      (side.is_rotation_possible(puzzle::LEAF::WEST)) ? Qt::green : Qt::red);
  painter_status.drawRect(0 + 2 * L / 24, L / 24, L / 24, L / 24);

  QString keyboard_status("KB: ");
  if (m_game.get_keybord_state() == puzzle::LEAF::NORTH) {
    keyboard_status += "NORTH";
  } else if (m_game.get_keybord_state() == puzzle::LEAF::EAST) {
    keyboard_status += "EAST";
  } else if (m_game.get_keybord_state() == puzzle::LEAF::WEST) {
    keyboard_status += "WEST";
  } else if (m_game.get_keybord_state() == puzzle::LEAF::INVALID) {
    keyboard_status += "CENTER";
  }
  painter_status.drawText(QPoint(0, 3 * L / 24), keyboard_status);
}

void SpinPuzzleWidget::set_size(int win_width, int win_height) {
  m_length = std::min(win_width, win_height);
  m_tx = (win_width - m_length) / 2.0;
  m_ty = (win_height - m_length) / 2.0;

  const int r = get_radius_internal() - width;
  radius_marble = r * sin(M_PI / 5) / 2;
  create_polygon(this->m_polygon);
  const int L = this->m_length;

  reset_btn->setGeometry(QRect(0, 3 * L / 24, 3 * L / 24, L / 24));
  pb->setGeometry(QRect(0, 4 * L / 24, 3 * L / 24, L / 24));
  twist_side->setGeometry(QRect(0, 5 * L / 24, 3 * L / 24, L / 24));

  spin_north->setGeometry(
      QRect(L / 2 - L / 24 + m_tx, 0 + m_ty, 3 * L / 24, L / 24));
  spin_east->setGeometry(
      QRect(L - 3 * L / 24 + m_tx, L / 2 + m_ty, 3 * L / 24, L / 24));
  spin_west->setGeometry(QRect(0 + m_tx, L / 2 + m_ty, 3 * L / 24, L / 24));
}

void SpinPuzzleWidget::next_section(QPainter &painter, int angle) const {
  const int L = this->m_length;
  const QPoint center = QPoint(L / 2, (2 * std::sqrt(3) + 3) * L / 12);
  painter.translate(center);
  painter.rotate(angle);
  painter.translate(-center);
}

void SpinPuzzleWidget::paint_puzzle_section(QPainter &painter, QColor color,
                                            QColor color_internal,
                                            QColor color_body) const {
  const int L = this->m_length;
  const double R = L / 4;
  const QPoint centerTop = QPoint(L / 2, L / 4);
  const int internalRadius = get_radius_internal();
  const bool primary = m_game.get_active_side() == puzzle::SIDE::FRONT;

  painter.setBrush(color);
  // The span angle for an ellipse segment to angle , which is in 16ths of a
  // degree 360 * 16 / 2 is half the full ellipse
  painter.drawPie(R, 0, 2 * R, 2 * R, 0, 360 * 8);

  painter.setBrush(color_internal);
  painter.drawPie(R + (R - internalRadius), (R - internalRadius),
                  2 * internalRadius, 2 * internalRadius, 0, 360 * 8);

  painter.setBrush(color);
  painter.drawPie(R + (R - internalRadius + 2 * width),
                  (R - internalRadius + 2 * width),
                  2 * (internalRadius - 2 * width),
                  2 * (internalRadius - 2 * width), 0, 360 * 8);

  painter.setBrush((primary) ? Qt::cyan : Qt::darkCyan);
  painter.drawPolygon(m_polygon);

  painter.setBrush(color_body);
  painter.drawPie(R + (R - internalRadius), (R - internalRadius),
                  2 * internalRadius, 2 * internalRadius, 360 * 8, 360 * 8);

  painter.drawLine(L / 2 - R, L / 4, L / 2 + R, L / 4);
}

void SpinPuzzleWidget::paintEvent(QPaintEvent *) {

  auto &game_side = m_game.get_side(m_game.get_active_side());
  const int L = this->m_length;
  const QPoint center = QPoint(L / 2, (2 * std::sqrt(3) + 3) * L / 12);

  paint_status();
  QPainter painter(this);
  painter.translate(QPoint(m_tx, m_ty));
  painter.save();
  // ====================================================================== //
  // NORTH
  int color_side = static_cast<uint8_t>(m_game.get_active_side());
  int north = static_cast<uint8_t>(puzzle::LEAF::NORTH);
  int east = static_cast<uint8_t>(puzzle::LEAF::EAST);
  int west = static_cast<uint8_t>(puzzle::LEAF::WEST);
  paint_puzzle_section(painter, m_colors_leaves[color_side][north],
                       m_colors_leaves_internal[color_side][north],
                       m_colors_leaves_body[color_side][north]);
  next_section(painter);
  paint_puzzle_section(painter, m_colors_leaves[color_side][east],
                       m_colors_leaves_internal[color_side][east],
                       m_colors_leaves_body[color_side][east]);
  // WEST
  next_section(painter);
  paint_puzzle_section(painter, m_colors_leaves[color_side][west],
                       m_colors_leaves_internal[color_side][west],
                       m_colors_leaves_body[color_side][west]);
  // ====================================================================== //

  painter.restore();
  painter.save();
  const int internalRadius = get_radius_internal() + width / 2;
  painter.setBrush(Qt::magenta);
  painter.drawEllipse(center, internalRadius, internalRadius);

  painter.restore();
  painter.save();
  // NORTH
  next_section(painter, game_side.get_phase_shift_internal_disk());
  painter.setBrush(Qt::darkBlue);
  paint_internal_circular_guide(painter, Qt::darkBlue);
  // EAST
  next_section(painter);
  paint_internal_circular_guide(painter, Qt::darkBlue);
  // WEST
  next_section(painter);
  paint_internal_circular_guide(painter, Qt::darkBlue);
  // ====================================================================== //
  painter.restore();
  // ====================================================================== //
  paint_marbles(painter);
}

void SpinPuzzleWidget::paint_marbles(QPainter &painter) {

  painter.save();
  auto &game_side = m_game.get_side(m_game.get_active_side());
  if (game_side.get_trifoild_status() != puzzle::TREFOIL::BORDER_ROTATION) {
    do_paint_marbles(painter);
  } else {
    do_paint_marbles_on_border(painter);
  }
}

void SpinPuzzleWidget::do_paint_marbles(QPainter &painter) {

  auto &game_side = m_game.get_side(m_game.get_active_side());
  // ====================================================================== //
  // paint marbles
  paint_marbles_on_leaf(painter, puzzle::LEAF::NORTH);
  next_section(painter);
  paint_marbles_on_leaf(painter, puzzle::LEAF::EAST);
  next_section(painter);
  paint_marbles_on_leaf(painter, puzzle::LEAF::WEST);
  // -------------------------- //
  painter.restore();
  // -------------------------- //
  next_section(painter, game_side.get_phase_shift_internal_disk());
  paint_marbles_on_internal_circle(painter, puzzle::LEAF::NORTH);
  next_section(painter);
  paint_marbles_on_internal_circle(painter, puzzle::LEAF::EAST);
  next_section(painter);
  paint_marbles_on_internal_circle(painter, puzzle::LEAF::WEST);
}

void SpinPuzzleWidget::do_paint_marbles_on_border(QPainter &painter) {

  painter.restore();
  auto &game_side = m_game.get_side(m_game.get_active_side());
  auto it = game_side.begin();
  const int L = this->m_length;
  const int r = get_radius_internal() - width;
  const QPoint center = QPoint(L / 2, L / 4);
  double shift_local = qDegreesToRadians(it.get_angle() * 12);
  double internal_dist_angle = game_side.get_phase_shift_internal_disk();

  // NORTH
  for (size_t n = 0; n < 7; ++n, ++it) {
    paint_marble(shift_local, painter, it, center, n + 2, r);
  }
  next_section(painter, internal_dist_angle);
  for (size_t n = 9; n >= 7; --n, ++it) {
    paint_marble(-shift_local, painter, it, center, n + 2, r);
  }
  // EAST
  next_section(painter, 120 - internal_dist_angle);
  for (size_t n = 0; n < 7; ++n, ++it) {
    paint_marble(shift_local, painter, it, center, n + 2, r);
  }
  next_section(painter, internal_dist_angle);
  for (size_t n = 9; n >= 7; --n, ++it) {
    paint_marble(-shift_local, painter, it, center, n + 2, r);
  }
  // WEST
  next_section(painter, 120 - internal_dist_angle);
  for (size_t n = 0; n < 7; ++n, ++it) {
    paint_marble(shift_local, painter, it, center, n + 2, r);
  }
  next_section(painter, internal_dist_angle);
  for (size_t n = 9; n >= 7; --n, ++it) {
    paint_marble(-shift_local, painter, it, center, n + 2, r);
  }
  // next_section(painter, 120 - internal_dist_angle);
}

QColor SpinPuzzleWidget::toQtColor(puzzle::Color value) {
  switch (value) {
  case puzzle::white:
    return Qt::white;
  case puzzle::black:
    return Qt::black;
  case puzzle::red:
    return Qt::red;
  case puzzle::darkRed:
    return Qt::darkRed;
  case puzzle::green:
    return Qt::green;
  case puzzle::darkGreen:
    return Qt::darkGreen;
  case puzzle::blue:
    return Qt::blue;
  case puzzle::darkBlue:
    return Qt::darkBlue;
  case puzzle::cyan:
    return Qt::cyan;
  case puzzle::darkCyan:
    return Qt::darkCyan;
  case puzzle::magenta:
    return Qt::magenta;
  case puzzle::darkMagenta:
    return Qt::darkMagenta;
  case puzzle::yellow:
    return Qt::yellow;
  case puzzle::darkYellow:
    return Qt::darkYellow;
  case puzzle::gray:
    return Qt::gray;
  case puzzle::darkGray:
    return Qt::darkGray;
  case puzzle::lightGray:
    return Qt::lightGray;
  default:
    return Qt::black;
    break;
  }
  return Qt::black;
}

void SpinPuzzleWidget::paint_marble(double shift_local, QPainter &painter,
                                    puzzle::SpinPuzzleSide<>::iterator it,
                                    const QPoint center, size_t n,
                                    const int r) {
  painter.setBrush(toQtColor(it->color()));
  const auto position =
      center + QPoint(r * cos(shift_local + M_PI_2 + n * M_PI / 5),
                      r * sin(shift_local + M_PI_2 + n * M_PI / 5));
  painter.drawEllipse(position, radius_marble, radius_marble);
#if DEBUG_MARBLES == 1
  painter.drawText(position, QString::number(it->id()));
#endif
}

void SpinPuzzleWidget::paint_marbles_on_leaf(QPainter &painter,
                                             puzzle::LEAF leaf) {
  auto &game_side = m_game.get_side(m_game.get_active_side());
  const int L = this->m_length;
  const int r = get_radius_internal() - width;
  const QPoint center = QPoint(L / 2, L / 4);
  auto it = game_side.begin(leaf);
  double shift_local = qDegreesToRadians(it.get_angle());
  if (game_side.get_trifoild_status() == puzzle::TREFOIL::BORDER_ROTATION) {
    shift_local = qDegreesToRadians(it.get_angle() * 12.0);
  }

  for (size_t n = 0; n < 7; ++n, ++it) {
    paint_marble(shift_local, painter, it, center, n + 2, r);
  }
}

void SpinPuzzleWidget::paint_marbles_on_internal_circle(QPainter &painter,
                                                        puzzle::LEAF leaf) {
  auto &game_side = m_game.get_side(m_game.get_active_side());
  const int L = this->m_length;
  const int r = get_radius_internal() - width;
  const QPoint center = QPoint(L / 2, L / 4);
  auto it = game_side.begin(leaf) + 7;
  double shift_local = qDegreesToRadians(it.get_angle());

  int start = 0;
  int end = 3;
  int increment = 1;

  for (size_t n = start; n < end; n += increment, ++it) {
    paint_marble(shift_local, painter, it, center, n + 9, r);
  }
}

void SpinPuzzleWidget::create_polygon(QPolygon &polygon) {
  const int L = this->m_length;
  const double R = L / 4;
  const QPoint center = QPoint(L / 2, (2 * std::sqrt(3) + 3) * L / 12);
  const QPoint centerTop = QPoint(L / 2, L / 4);
  const QPoint topCorner(0, R);

  const int arc_steps = 100;
  const int angle = 30;
  polygon = QPolygon();
  polygon << centerTop;
  for (int i = 0; i <= arc_steps; ++i) {
    polygon << QPoint(
        topCorner.x() + R * cos(qDegreesToRadians(i * angle / arc_steps)),
        topCorner.y() + R * sin(qDegreesToRadians(i * angle / arc_steps)));
  }
  polygon << center;

  QPoint rightCorner(L, L / 4);
  for (int i = arc_steps; i >= 0; --i) {
    polygon << QPoint(
        rightCorner.x() - R * cos(qDegreesToRadians(i * angle / arc_steps)),
        rightCorner.y() - R * sin(qDegreesToRadians(-i * angle / arc_steps)));
  }
}

/*
   Solve with wolfwamalpha:
    4*R^2sin(pi/5) = (a - c + (cos(11pi/30) - cos(13pi/10))^2 + (a - c +
  (sin(11pi/30) - sin(13pi/10))^2 with origin (0,0) bottom left a = 1/4, b =
  1/4;    -> center left c = 1/2, d = sqrt(3)/2 + 1/4 -> center
*/
int SpinPuzzleWidget::get_radius_internal() const {
  const int L = this->m_length;
  // return 0.19945 * L;
  return 0.185 * L;
}

inline void
SpinPuzzleWidget::paint_internal_circular_guide(QPainter &painter,
                                                QColor color) const {
  const int L = this->m_length;
  const int r = get_radius_internal() - width / 2;
  const QPoint center = QPoint(L / 2, (2 * std::sqrt(3) + 3) * L / 12);

  QPen pen;
  pen.setBrush(color);
  if (width) {
    pen.setWidth(width);
  }
  painter.setPen(pen);
  double angle1 = 90 + 36 * 4;
  double angle2 = 36 * 2;
  painter.drawLine(center - QPoint(L / 24, 0), center + QPoint(L / 24, 0));
  painter.drawArc(L / 2 - r, L / 4 - r, 2 * r, 2 * r, angle1 * 16, angle2 * 16);
}

void SpinPuzzleWidget::mousePressEvent(QMouseEvent *e) {
#if DEBUG_EVENT == 1
  qDebug() << "[DEBUG][mousePessEvent] position: " << e->pos();
#endif
  m_lastPositionMause = e->pos() - QPoint(m_tx, m_ty);
}

void SpinPuzzleWidget::mouseMoveEvent(QMouseEvent *ev) {
#if DEBUG_EVENT == 1
  qDebug() << "[DEBUG][mouseMoveEvent] position: " << ev->pos();
#endif
  const int L = this->m_length;
  const QPoint center_north = QPoint(L / 2, L / 4);
  const QPoint center_west = QPoint(L / 4, (sqrt(3) + 1) / 4 * L);
  const QPoint center_east = QPoint(3 * L / 4, (sqrt(3) + 1) / 4 * L);

  auto pos = ev->pos() - QPoint(m_tx, m_ty);
  if (mouse_event_inside_internal_circle(pos)) {
    update();
  } else if (mouse_event_inside_leaf(pos, center_north, puzzle::LEAF::NORTH) ||
             mouse_event_inside_leaf(pos, center_east, puzzle::LEAF::EAST) ||
             mouse_event_inside_leaf(pos, center_west, puzzle::LEAF::WEST)) {
    update();
  }
#ifdef DEBUG_CONSISTENCY
  if (!m_game.check_consistency()) {
    qDebug() << "[DEBUG][mouseMoveEvent] marbles are in an invalid state mouse "
                "at position "
             << ev->pos();
    qDebug() << "[DEBUG][mouseMoveEvent] GAME:";
    qDebug() << m_game.to_string().c_str();
    assert(false);
  }
#endif
}

bool SpinPuzzleWidget::can_rotate_internal() { return true; }

bool SpinPuzzleWidget::mouse_event_inside_internal_circle(QPoint pos) {
  if (!can_rotate_internal()) {
#if DEBUG_EVENT == 1
    qDebug()
        << "[DEBUG][mouse_event_inside_internal_circle] cannot rotate internal";
#endif
    return false;
  }

  const int L = this->m_length;
  const QPoint center = QPoint(L / 2, (2 * std::sqrt(3) + 3) * L / 12);
  const int internalRadius = get_radius_internal() + width / 2;

  auto pos_center = pos - center;
  if (pos_center.dotProduct(pos_center, pos_center) >
      internalRadius * internalRadius) {
    return false;
  }

  auto c_pos = pos;
  auto last = m_lastPositionMause;
  c_pos -= center;
  last -= center;
  auto angle_new = atan2(c_pos.y(), c_pos.x());
  auto angle_old = atan2(last.y(), last.x());

  if ((angle_new * angle_old < 0) && abs(angle_new - angle_old) > M_PI) {
    angle_new = fmod(angle_new + M_PI, 2 * M_PI);
    angle_old = fmod(angle_old + M_PI, 2 * M_PI);
  }

  // shift += (angle_new - angle_old) * internalRadius / 2;
  double delta_alpha = signum(angle_new - angle_old) * 360.0 * get_speed();
  m_lastPositionMause = pos;

#if DEBUG_EVENT == 1
  qDebug()
      << "[DEBUG][mouse_event_inside_internal_circle] rotate internal disk by "
      << delta_alpha;
#endif
  m_game.rotate_internal_disk(delta_alpha);
  // check if we move from rotating marbles in outer circles to moving internal
  // circle update the marbles sequence:
  return true;
}

bool SpinPuzzleWidget::mouse_event_inside_leaf(QPoint pos, QPoint center,
                                               puzzle::LEAF leaf) {
  if (!is_mause_on_leaf_marbles(pos, center)) {
    return false;
  }

  auto c_pos = pos;
  auto last = m_lastPositionMause;
  c_pos -= center;
  last -= center;

  auto angle_new = atan2(c_pos.y(), c_pos.x());
  auto angle_old = atan2(last.y(), last.x());

  if ((angle_new * angle_old < 0) && abs(angle_new - angle_old) > M_PI) {
    angle_new = fmod(angle_new + M_PI, 2 * M_PI);
    angle_old = fmod(angle_old + M_PI, 2 * M_PI);
  }

  double delta_angle = signum(angle_new - angle_old) * 360.0 * get_speed();

  auto &side = m_game.get_side(m_game.get_active_side());
  side.rotate_marbles(leaf, delta_angle);
  m_lastPositionMause = pos;

  return true;
}

double SpinPuzzleWidget::get_speed() { return 1.5 / this->m_length; }

bool SpinPuzzleWidget::is_mause_on_leaf_marbles(QPoint pos, QPoint center) {
  const double internalRadius = get_radius_internal();

  pos = pos - center;
  auto r = sqrt(pos.dotProduct(pos, pos));
  if (r < internalRadius && r > internalRadius - 2 * radius_marble) {
    return true;
  }
  return false;
}

void SpinPuzzleWidget::keyPressEvent(QKeyEvent *e) {
  if (processKey(e->key(), 1.0 / 4.0)) {
#ifdef DEBUG_CONSISTENCY
    if (!m_game.check_consistency()) {
      qDebug() << "[DEBUG][keyPressEvent] marbles are in an invalid state";
      qDebug() << "[DEBUG][KeyPressEvent] GAME:";
      qDebug() << m_game.to_string().c_str();
      assert(false);
    }
#endif
    update();
    this->setFocus();
  }
}

void SpinPuzzleWidget::resizeEvent(QResizeEvent *e) {
  this->set_size(e->size().width(), e->size().height());
  update();
}

bool SpinPuzzleWidget::processKey(int key, double fraction_angle) {
  // To sync. spin of leaf and colors: not optimal.
  // shuffle also is not in sync.
  // probably better directly in SpinPuzzleGame
  if (key == puzzle::Key_PageUp || key == puzzle::Key_PageDown) {
    // make scure only leaves are used
      if (m_game.get_keybord_state() == puzzle::LEAF::NORTH) {
        do_spin_north();
      } else if (m_game.get_keybord_state() == puzzle::LEAF::EAST) {
        do_spin_east();
      } else if (m_game.get_keybord_state() == puzzle::LEAF::WEST) {
        do_spin_west();
      }
      return true;
  }
  return m_game.process_key(key, fraction_angle);
}

void SpinPuzzleWidget::reset() {
  m_game = puzzle::SpinPuzzleGame();
  reset_leaf_colors();
  update();
}

void SpinPuzzleWidget::shuffle() {
  m_game.shuffle();
#ifdef DEBUG_CONSISTENCY
  if (!m_game.check_consistency()) {
    qDebug() << "[DEBUG][keyPressEvent] marbles are in an invalid state";
    qDebug() << "[DEBUG][KeyPressEvent] GAME:";
    qDebug() << m_game.to_string().c_str();
    assert(false);
  }
#endif
  update();
  this->setFocus();
}