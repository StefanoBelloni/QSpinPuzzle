#include "spin_puzzle_widget.h"
#include <stdint.h>

#include <QBrush>
#include <QDir>
#include <QGridLayout>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QRadialGradient>
#include <QStandardPaths>
#include <QStyleOption>
#include <QTimer>
#include <QtMath>

#include <fstream>
#include <sstream>

#define DEBUG_MARBLES 0
#define HIDDEN_BUTTON 1

SpinPuzzleWidget::SpinPuzzleWidget(int win_width, int win_heigth,
                                   bool save_files, QWidget *parent)
    : QWidget(parent), m_allow_play(save_files) {

  m_twist_side = new QPushButton("twist", this);
  connect(m_twist_side, &QPushButton::released, this, [this] {
    this->m_game.swap_side();
    update();
  });

  if (m_allow_play) {
    create_play_buttons();
    connect_play_buttons();
  }

  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, [this] {
    m_elapsed_time += 1;
    update();
  });

  m_congratulation_timer = new QTimer(this);
  connect(m_congratulation_timer, &QTimer::timeout, this, [this] {
    m_rotation_congratulation += 5;
    update();
  });

  set_size(win_width, win_heigth);
  reset_leaf_colors();

  this->setFocus();
}

void SpinPuzzleWidget::create_play_buttons() {
  m_spin_north = new QPushButton("SPIN", this);
  m_spin_east = new QPushButton("SPIN", this);
  m_spin_west = new QPushButton("SPIN", this);
  m_reset_btn = new QPushButton("RESET", this);
  m_start_btn = new QPushButton("START", this);
  m_load_btn = new QPushButton("LOAD", this);
  m_save_btn = new QPushButton("SAVE", this);
  m_load_records_btn = new QPushButton("PUZZLE RECORDS", this);
}

void SpinPuzzleWidget::connect_play_buttons() {
  connect(m_spin_north, &QPushButton::released, this, [this] {
    do_spin_north();
    update();
  });

  connect(m_spin_east, &QPushButton::released, this, [this] {
    do_spin_east();
    update();
  });

  connect(m_spin_west, &QPushButton::released, this, [this] {
    do_spin_west();
    update();
  });

  connect(m_reset_btn, &QPushButton::released, this, &SpinPuzzleWidget::reset);

  connect(m_start_btn, &QPushButton::released, this,
          &SpinPuzzleWidget::start_game);

  connect(m_load_btn, &QPushButton::released, this, [this] {
    m_paint_congratulations = false;
    stop_spinning_winning();
    load();
    update();
  });

  connect(m_save_btn, &QPushButton::released, this, [this] {
    if (m_game.is_game_solved() ||
        (!(m_timer->isActive() || m_elapsed_time > 0))) {
      return;
    }
    if (save_progress()) {
      QMessageBox(QMessageBox::Information, "progress", "progress saved.")
          .exec();
      update();
    }
  });

  connect(m_load_records_btn, &QPushButton::released, this, [this] {
    std::vector<std::pair<int, puzzle::SpinPuzzleGame>> games;
    load_records(games);
    /*
    if (games.size() == 0) {
      QMessageBox(QMessageBox::Warning, "records",
                  "No record could be loaded.")
          .exec();
      return;
    }
    */
    m_history_widget =
        new SpinPuzzleHistoryWidget(m_length, m_length, this, games);
    m_history_widget->setFixedSize(m_length, m_length);
    m_history_widget->show();
    m_history_widget->setVisible(true);
  });
}

double SpinPuzzleWidget::get_height_button_bottom() const {
  return 17.0 / 10.0 * m_length / 24.0;
}

double SpinPuzzleWidget::get_width_button_bottom() const {
  return 5.0 * m_length / 24.0;
}

double SpinPuzzleWidget::get_length_status_square() const {
  return m_length / 24.0;
}

void SpinPuzzleWidget::reset_file_app() {
  auto msg = QMessageBox(QMessageBox::Warning, "reset",
                         "Do you want to delete all saved puzzles",
                         QMessageBox::Ok | QMessageBox::Cancel);
  msg.setInformativeText("This operation cannot be reversed!");
  msg.setDefaultButton(QMessageBox::Cancel);
  int delete_progress = msg.exec();
  if (delete_progress == QMessageBox::Cancel) {
    return;
  }
  std::vector<std::string> files;
  files.emplace_back((get_current_puzzle_file().c_str()));
  files.emplace_back((get_puzzle_file().c_str()));
  files.emplace_back((get_records_puzzle_file().c_str()));
  for (auto &d : files) {
    QFile file(d.c_str());
    file.remove();
  }
  // QFile file(get_records_puzzle_file().c_str());
  // file.remove();
  reset();
  delete_history_popup();
  update();
}

bool SpinPuzzleWidget::save_progress() {
  auto file_name = get_current_puzzle_file();
  std::ofstream f(get_current_puzzle_file(), std::ios::trunc);
  if (f.is_open()) {
    f << m_elapsed_time << " ";
    m_game.serialize(f);
    f.close();
    return true;
  }
  return false;
}

void SpinPuzzleWidget::store_puzzle_begin() {
  std::ofstream f(get_puzzle_file(), std::ios::trunc);
  if (f.is_open()) {
    f << m_elapsed_time << " ";
    m_game.serialize(f);
    f.close();
  }
}

int SpinPuzzleWidget::load_records(
    std::vector<std::pair<int, puzzle::SpinPuzzleGame>> &games) {
  games.clear();
  int max_time = 0;
  std::ifstream f1(get_records_puzzle_file());
  int t = 0;
  if (f1.is_open()) {
    std::string s;
    f1 >> s;
    if (s == "records:") {
      while (!f1.eof()) {
        f1 >> t;
        if (f1.eof())
          break;
        max_time = std::max(t, max_time);
        puzzle::SpinPuzzleGame g;
        g.load(f1);
        games.emplace_back(t, g);
      }
    }
  }
  return max_time;
}

bool SpinPuzzleWidget::store_puzzle_record() {
  puzzle::SpinPuzzleGame game;
  std::ifstream f0(get_puzzle_file());
  // load beginning
  if (f0.is_open()) {
    int tmp;
    f0 >> tmp;
    game.load(f0);
    f0.close();
  }
  bool ok = store_puzzle_record(m_elapsed_time, game);
  game.reset();
  return ok;
}

bool SpinPuzzleWidget::store_puzzles_record(
    std::vector<std::pair<int, puzzle::SpinPuzzleGame>> games) {
  std::sort(games.begin(), games.end(),
            [](const std::pair<int, puzzle::SpinPuzzleGame> &first,
               const std::pair<int, puzzle::SpinPuzzleGame> &second) {
              return first.first < second.first;
            });

  std::ofstream f(get_records_puzzle_file());
  if (!f.is_open()) {
    return false;
  }
  f << "records:"
    << " ";
  int m = games.size();
  for (int n = 0; n < std::min(m, m_max_saved_games); ++n) {
    f << games[n].first << " ";
    games[n].second.serialize(f);
  }
  f.close();

  return true;
}

bool SpinPuzzleWidget::store_puzzle_record(int elapsed_time,
                                           puzzle::SpinPuzzleGame game) {
  // store in records - max 5
  std::vector<std::pair<int, puzzle::SpinPuzzleGame>> games;
  // check if current is faster then the others
  int max_time = load_records(games);
  int size = games.size();
  if (size > m_max_saved_games && elapsed_time > max_time) {
    return false;
  }

  auto current = game.current_time_step();
  // check for duplicates
  bool found_duplicate = false;
  for (auto &p : games) {
    if (p.second.current_time_step() == current) {
      if (p.first > elapsed_time) {
        p.first = elapsed_time;
      }
      found_duplicate = true;
      break;
    }
  }
  // add to games
  if (!found_duplicate) {
    games.emplace_back(elapsed_time, game);
  } else {
    return false;
  }

  return store_puzzles_record(games);
}

void SpinPuzzleWidget::start_timer() { m_timer->start(1000); }

// TODO:
// do not overwrite while iterating
void SpinPuzzleWidget::load(int index, puzzle::SpinPuzzleGame &game) {

  m_solved = false;
  m_elapsed_time = 0;
  std::string path;
  path = get_records_puzzle_file();

  auto msg =
      QMessageBox(QMessageBox::Warning, "load", "Unable to load puzzle.");

  std::ifstream f(path);
  int current = -1;
  if (!f.is_open()) {
    msg.setInformativeText("File not found.");
    msg.exec();
    return;
  }
  std::string s;
  f >> s;
  if (s != "records:") {
    msg.setInformativeText("Invalid file.");
    msg.exec();
    f.close();
    return;
  }

  do {
    f >> m_elapsed_time;
    if (f.eof()) {
      msg.setInformativeText("Puzzle not found.");
      f.close();
      return;
    }
    game.load(f);
    current++;
  } while (current != index);
  f.close();
}

void SpinPuzzleWidget::load(int index) { load(index, m_game); }

void SpinPuzzleWidget::load() {

  m_solved = false;
  int do_load = QMessageBox::Cancel;
  QMessageBox msgBox(QMessageBox::Question, "load",
                     "Are you sure you what to load the puzzle?");
  QAbstractButton *pButtonLatest =
      msgBox.addButton(tr("latest"), QMessageBox::YesRole);
  QAbstractButton *pButtonBegin =
      msgBox.addButton(tr("start"), QMessageBox::YesRole);
  msgBox.setInformativeText("You will lose yuor progress");
  msgBox.setStandardButtons(QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Cancel);
  do_load = msgBox.exec();

  if (do_load == QMessageBox::Cancel) {
    return;
  }
  m_elapsed_time = 0;
  std::string path;
  if (msgBox.clickedButton() == pButtonLatest) {
    path = get_current_puzzle_file();
  } else if (msgBox.clickedButton() == pButtonBegin) {
    path = get_puzzle_file();
  } else {
    return;
  }

  std::ifstream f(path);
  if (f.is_open()) {
    f >> m_elapsed_time;
    m_game.load(f);
    f.close();
  }
  m_timer->start(1000);
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

  m_colors_leaves[0] = {QColor(0xee6c6c), QColor(0x756ef3), QColor(0xf3dd6e)};
  m_colors_leaves_internal[0] = {QColor(0x7e413c), QColor(0x93a1d7),
                                 QColor(0xe2c67f)};
  m_colors_leaves_body[0] = {QColor(0xee6c6d), QColor(0x756ef4),
                             QColor(0xf3dd60)};

  m_colors_leaves[1] = {QColor(0x84bf72), QColor(0x1000ab), QColor(0xefc111)};
  m_colors_leaves_internal[1] = {QColor(0x619e02), QColor(0x029e98),
                                 QColor(0xf2ea41)};
  m_colors_leaves_body[1] = {QColor(0x84bf72), QColor(0x1000ab),
                             QColor(0xefc111)};
}

void SpinPuzzleWidget::paint_status() {
  if (!m_allow_play) {
    return;
  }
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

  auto s = get_length_status_square();
  painter_status.translate(0, s);
  painter_status.drawRect(0, s, s - 1, s - 1);

  painter_status.setBrush(
      (side.is_rotation_possible(puzzle::LEAF::NORTH)) ? Qt::green : Qt::red);
  painter_status.drawRect(0 + 1 * s, s, s, s);
  painter_status.setBrush(
      (side.is_rotation_possible(puzzle::LEAF::EAST)) ? Qt::green : Qt::red);
  painter_status.drawRect(0 + 2 * s, s, s, s);
  painter_status.setBrush(
      (side.is_rotation_possible(puzzle::LEAF::WEST)) ? Qt::green : Qt::red);
  painter_status.drawRect(0 + 3 * s, s, s, s);

  QString keyboard_status("***");
  if (m_game.get_keybord_state() == puzzle::LEAF::NORTH) {
    keyboard_status += "NORTH";
  } else if (m_game.get_keybord_state() == puzzle::LEAF::EAST) {
    keyboard_status += "EAST";
  } else if (m_game.get_keybord_state() == puzzle::LEAF::WEST) {
    keyboard_status += "WEST";
  } else if (m_game.get_keybord_state() == puzzle::LEAF::INVALID) {
    keyboard_status += "CENTER";
  } else {
    keyboard_status += "...";
  }
  keyboard_status += " ***";
  painter_status.drawText(QPoint(0, 3 * s), keyboard_status);
}

void SpinPuzzleWidget::paint_timer() {
  // if (!m_allow_play) { return; }
  const int L = this->m_length;
  QPainter painter_status(this);
  if (m_game.is_game_solved()) {
    painter_status.setBrush(Qt::green);
    if (m_timer->isActive() && m_elapsed_time > 0 && !m_solved) {
      store_puzzle_record();
      m_solved = true;
      m_paint_congratulations = true;
      m_congratulation_timer->start(10);
    }
    m_timer->stop();
  } else {
    painter_status.setBrush(Qt::red);
    if (m_timer->isActive()) {
      painter_status.setPen(Qt::darkRed);
    }
  }
  if (m_paint_congratulations) {
    paint_congratulation(painter_status);
  }
  int win_width = (m_allow_play) ? m_win_width : 8 * L / 24;
  int win_heigth = (m_allow_play) ? 0 : (L - L / 24);
  if (m_allow_play) {
    painter_status.drawRect(win_width - 8 * L / 24, win_heigth, 8 * L / 24,
                            L / 24);
  }
  QString time = QString("time: %3:%2:%1")
                     .arg(m_elapsed_time % 60, 2, 10, QChar('0'))
                     .arg(m_elapsed_time / 60, 2, 10, QChar('0'))
                     .arg(m_elapsed_time / 60 / 60, 2, 10, QChar('0'));
  painter_status.drawText(QPoint(win_width - 8 * L / 24, 2 * L / 24), time);
}

void SpinPuzzleWidget::paint_congratulation(QPainter &painter) {
  const int L = this->m_length;
  painter.setBrush(
      QColor(125 * abs(std::cos(m_rotation_congratulation / 62.8) *
                       std::sin(m_rotation_congratulation / 120)),
             255 * abs(std::sin(m_rotation_congratulation / 62.8)),
             255 * abs(std::cos(m_rotation_congratulation / 100.0))));
  painter.drawRect(L / 4 + m_tx, L / 2 - L / 16 + m_ty, L / 2, L / 8);
  painter.drawText(QPoint(L / 4 + L / 8 + m_tx, L / 2 + m_ty),
                   "CONGRATULATION!");
}

void SpinPuzzleWidget::set_size(int win_width, int win_height) {
  m_length = std::min(win_width, win_height);
  m_tx = (win_width - m_length) / 2.0;
  m_ty = (win_height - m_length) / 2.0;
  m_win_width = win_width;
  m_win_height = win_height;

  const int r = get_radius_internal() - width;
  radius_marble = r * sin(M_PI / 5) / 2;
  create_polygon(this->m_polygon);
  const int L = this->m_length;

  // pb->setGeometry(QRect(0, 6 * L / 24, 5 * L / 24, L / 24));
  m_twist_side->setGeometry(
      QRect(0 + m_tx, 5 * L / 24 + m_ty, 5 * L / 24, 2 * L / 24));

  if (m_allow_play) {
    m_spin_north->setGeometry(
        QRect(L / 2 - 2 * L / 24 + m_tx, 0 + m_ty, 5 * L / 24, L / 24));
    m_spin_east->setGeometry(
        QRect(L - 3 * L / 24 + m_tx, L / 2 + m_ty, 5 * L / 24, L / 24));
    m_spin_west->setGeometry(QRect(0 + m_tx, L / 2 + m_ty, 5 * L / 24, L / 24));

    double h = get_height_button_bottom();
    double w = get_width_button_bottom();

    m_start_btn->setGeometry(QRect(0 * w, m_win_height - h, w, h));
    m_reset_btn->setGeometry(QRect(1 * w, m_win_height - h, w, h));
    m_load_btn->setGeometry(QRect(2 * w, m_win_height - h, w, h));
    m_save_btn->setGeometry(QRect(3 * w, m_win_height - h, w, h));

    m_load_records_btn->setGeometry(QRect(
        0, 0, (8.0 * 10.0 - 2.0) / 10.0 * L / 24.0, 17.0 / 10.0 * L / 24.0));
    QPalette pal = m_load_records_btn->palette();
    pal.setColor(QPalette::Button, QColor(Qt::gray));
    m_load_records_btn->setAutoFillBackground(true);
    m_load_records_btn->setPalette(pal);
    m_load_records_btn->update();
  }
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

  auto originalBrush = painter.brush();
  const QPoint center = QPoint(L / 2, (2 * std::sqrt(3) + 3) * L / 12);
  QRadialGradient radialGrad(center, 3 / 2 * L);
  radialGrad.setColorAt(0, (primary) ? Qt::cyan : Qt::lightGray);
  radialGrad.setColorAt(1, Qt::black);
  painter.setBrush(radialGrad);
  // painter.setBrush((primary) ? Qt::cyan : Qt::darkCyan);
  painter.drawPolygon(m_polygon);

  QRadialGradient radialGrad2(centerTop, 2 * R);
  radialGrad2.setColorAt(0, color_body);
  radialGrad2.setColorAt(1, Qt::white);
  painter.setBrush(radialGrad2);
  painter.drawPie(R + (R - internalRadius), (R - internalRadius),
                  2 * internalRadius, 2 * internalRadius, 360 * 8, 360 * 8);

  painter.drawLine(L / 2 - R, L / 4, L / 2 + R, L / 4);

  painter.setBrush(originalBrush);
}

void SpinPuzzleWidget::paint_background() {
  QPainter painter(this);
  painter.setBrush(
      (m_allow_play)
          ? QColor(
                208, 228,
                225) // * abs(std::cos(10 * qDegreesToRadians(m_elapsed_time)))
          :
          // QColor(0xb5d5c3)
          QColor(0xf0eb9a));
  painter.drawRect(0, 0, m_win_width - 1, m_win_height - 1);
}

void SpinPuzzleWidget::paintEvent(QPaintEvent * /*event*/) {
  paint_background();
  paint_status();
  paint_game();
  paint_marbles();
  paint_timer();
}

void SpinPuzzleWidget::paint_game() {
  auto &game_side = m_game.get_side(m_game.get_active_side());
  const int L = this->m_length;
  const QPoint center = QPoint(L / 2, (2 * std::sqrt(3) + 3) * L / 12);

  QPainter painter(this);
  painter.translate(QPoint(m_tx, m_ty));
  next_section(painter, m_rotation_congratulation);
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
  auto originalBrush = painter.brush();
  const int internalRadius = get_radius_internal() + width / 2;
  QRadialGradient radialGrad(center, internalRadius);
  radialGrad.setColorAt((color_side) % 2, Qt::magenta);
  radialGrad.setColorAt((color_side + 1) % 2, Qt::darkMagenta);
  // radialGrad.setColorAt(1, Qt::magenta);
  painter.setBrush(radialGrad);
  // painter.setBrush(Qt::magenta);
  painter.drawEllipse(center, internalRadius, internalRadius);
  painter.setBrush(originalBrush);

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
}

void SpinPuzzleWidget::paint_marbles() {
  QPainter painter(this);
  painter.translate(QPoint(m_tx, m_ty));
  next_section(painter, m_rotation_congratulation);
  painter.save();

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

QColor SpinPuzzleWidget::toMarbleColor(puzzle::Color value) {
  switch (value) {
  case puzzle::blue:
    return QColor(0x2654a5);
  case puzzle::green:
    return QColor(0x39a526);
  case puzzle::magenta:
    return QColor(0xe170f2);
  case puzzle::cyan:
    return QColor(0x29c6c8);
  case puzzle::red:
    return QColor(0xfc2b2b);
  case puzzle::yellow:
    return QColor(0xf2f270);
  default:
    return toQtColor(value);
  }
}

void SpinPuzzleWidget::paint_marble(double shift_local, QPainter &painter,
                                    puzzle::SpinPuzzleSide<>::iterator it,
                                    const QPoint center, size_t n,
                                    const int r) {
  const auto position =
      center + QPoint(r * cos(shift_local + M_PI_2 + n * M_PI / 5),
                      r * sin(shift_local + M_PI_2 + n * M_PI / 5));
  auto originalBrush = painter.brush();
  QRadialGradient radialGrad(position, r);
  radialGrad.setColorAt(0, toMarbleColor(it->color()));
  radialGrad.setColorAt(1, Qt::black);
  painter.setBrush(radialGrad);
  painter.drawEllipse(position, radius_marble, radius_marble);
#if DEBUG_MARBLES == 1
  painter.drawText(position, QString::number(it->id()));
#endif
  painter.setBrush(originalBrush);
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

  for (int n = start; n < end; n += increment, ++it) {
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
    4*R^2sin^2(pi/5) = (a - c + (cos(11pi/30) - cos(13pi/10))^2 + (a - c +
  (sin(11pi/30) - sin(13pi/10))^2 with origin (0,0) bottom left a = 1/4, b =
  1/4;    -> center left c = 1/2, d = sqrt(3)/2 + 1/4 -> center
*/
double SpinPuzzleWidget::get_radius_internal() const {
  const int L = this->m_length;
  // return 0.19945 * L;
  // return (m_allow_play) ? 0.185 * L : 0.2 * L;
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
  if (!m_allow_play) {
    return;
  }

  if (m_paint_congratulations) {
    const int L = this->m_length;
    auto rec = QRect(L / 4 + m_tx, L / 2 - L / 16 + m_ty, L / 2, L / 8);
    if (rec.contains(e->pos())) {
      reset();
    }
  }

#if HIDDEN_BUTTON == 1
  const int L = this->m_length;
  if (e->pos().x() > m_win_width - L / 24 &&
      e->pos().y() > m_win_height - L / 24) {
    int pirate =
        QMessageBox(QMessageBox::Question, "pirate", "Fake resolution of game",
                    QMessageBox::Ok | QMessageBox::Cancel)
            .exec();
    if (pirate == QMessageBox::Ok) {
      m_game.reset();
      do_spin_east();
    }
  }
#endif

  m_lastPositionMause = e->pos() - QPoint(m_tx, m_ty);
}

void SpinPuzzleWidget::mouseMoveEvent(QMouseEvent *ev) {
  if (!m_allow_play) {
    return;
  }
  if (m_lastPositionMause.x() == -1 && m_lastPositionMause.y() == -1) {
    return;
  }
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
  } else {
    m_lastPositionMause = QPoint(-1, -1);
  }
}

bool SpinPuzzleWidget::can_rotate_internal() { return true; }

bool SpinPuzzleWidget::mouse_event_inside_internal_circle(QPoint pos) {
  if (!can_rotate_internal()) {
    return false;
  }

  const int L = this->m_length;
  const QPoint center = QPoint(L / 2, (2 * std::sqrt(3) + 3) * L / 12);
  const int internalRadius = get_radius_internal() + width / 2;

  const auto pos_center = pos - center;
  const auto r2 = pos_center.dotProduct(pos_center, pos_center);
  const auto R2 = internalRadius * internalRadius;
  const auto T2 = L / 24 * L / 24;
  if (r2 > R2 || r2 < T2) {
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

  if (c_pos.y() * last.y() < 0) {
    angle_new = 0;
    angle_old = 0;
  }

  double delta_alpha = get_scaled_angle(angle_new, angle_old);
  m_lastPositionMause = pos;
  m_game.rotate_internal_disk(delta_alpha);
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

  if (c_pos.y() * last.y() < 0) {
    angle_new = 0;
    angle_old = 0;
  }

  const double delta_angle = get_scaled_angle(angle_new, angle_old);

  auto &side = m_game.get_side(m_game.get_active_side());
  side.rotate_marbles(leaf, delta_angle);
  m_lastPositionMause = pos;

  return true;
}

double SpinPuzzleWidget::get_scaled_angle(double a1, double a2) {
  return (a1 - a2) * 11.0 * M_PI;
}

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
  if (!m_allow_play) {
    return;
  }
  if (processKey(e->key(), 1.0 / 4.0)) {
    update();
    this->setFocus();
  }
}

void SpinPuzzleWidget::resizeEvent(QResizeEvent *e) {
  this->set_size(e->size().width(), e->size().height());
  update();
}

void SpinPuzzleWidget::set_game(int time, const puzzle::SpinPuzzleGame &game) {
  m_elapsed_time = time;
  m_game = game;
}

void SpinPuzzleWidget::delete_history_popup() {
  m_history_widget->deleteLater();
  m_history_widget = nullptr;
}
void SpinPuzzleWidget::set_elapsed_time(int t) { m_elapsed_time = t; }

puzzle::SpinPuzzleGame &SpinPuzzleWidget::get_game() { return m_game; }

void SpinPuzzleWidget::closeEvent(QCloseEvent * /*event*/) {
  if (m_history_widget) {
    m_history_widget->deleteLater();
  }
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
  if ((m_timer->isActive() || m_elapsed_time > 0) && !m_solved) {
    auto msg = QMessageBox(QMessageBox::Question, "reset",
                           "Are you sure you waht to reset the puzzle?",
                           QMessageBox::Ok | QMessageBox::Cancel);
    msg.setInformativeText("You will lose yuor progress");
    if (msg.exec() == QMessageBox::Ok) {
      // Save was clicked
      m_paint_congratulations = false;
      stop_spinning_winning();
      m_elapsed_time = 0;
      m_timer->stop();
      m_game = puzzle::SpinPuzzleGame();
      reset_leaf_colors();
    }
  }
  m_paint_congratulations = false;
  stop_spinning_winning();
  update();
}

void create_dir() {
  auto path =
      QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  auto dir = QDir(path);
  // qDebug() << "[DEBUG][PATH] path: " << dir;
  if (!dir.exists()) {
    dir.mkpath(".");
  }
}

std::string SpinPuzzleWidget::get_current_puzzle_file() {
  auto path =
      QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  auto filename = QDir(path + "/current_pruzzle.txt");
  return filename.path().toStdString();
}

std::string SpinPuzzleWidget::get_puzzle_file() {
  auto path =
      QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  auto filename = QDir(path + "/pruzzle.txt");
  return filename.path().toStdString();
}

std::string SpinPuzzleWidget::get_records_puzzle_file() {
  auto path =
      QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  auto filename = QDir(path + "/records.txt");
  return filename.path().toStdString();
}

void SpinPuzzleWidget::stop_spinning_winning() {
  if (m_congratulation_timer->isActive()) {
    m_congratulation_timer->stop();
    m_rotation_congratulation = 0;
  }
}

void SpinPuzzleWidget::start_game() {
  m_paint_congratulations = false;
  stop_spinning_winning();
  if (m_timer->isActive()) {
    return;
  }

  m_elapsed_time = 0;
  m_solved = false;
  m_timer->start(1000);
  m_game.shuffle();
  if (m_allow_play) {
    create_dir();
    store_puzzle_begin();
    save_progress();
  }

  update();
  this->setFocus();
}

void SpinPuzzleWidget::start_with_game(const puzzle::SpinPuzzleGame &game) {
  set_game(0, game);
  start_game();
}
