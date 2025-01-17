#include "spin_puzzle_widget.h"

#include <stdint.h>

#include <QBrush>
#include <QDir>
#include <QGridLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QRadialGradient>
#include <QStandardPaths>
#include <QStyleOption>
#include <QTimer>
#include <QtLogging>
#include <QtMath>
#include <fstream>
#include <sstream>

#include "puzzle/spin_metrics.h"
#include "puzzle/spin_puzzle_cipher.h"
#include "spin_puzzle_config_widget.h"
#include "spin_puzzle_replay_widget.h"
#include "spin_puzzle_history_widget.h"

#define DEBUG_MARBLES 0
#define DEBUG_CIPHER 0
#define HIDDEN_BUTTON 1

#define PAINT_PUZZLE_SECTION(__leaf__)                                         \
  paint_puzzle_section(                                                        \
    painter,                                                                   \
    (m_game.get_keybord_state() == puzzle::LEAF::__leaf__) ? true : false,     \
    m_colors_leaves[COLOR_SIDE][__leaf__],                                     \
    m_colors_leaves_internal[COLOR_SIDE][__leaf__],                            \
    m_colors_leaves_body[COLOR_SIDE][__leaf__]);                               \
  move_to_next_section(painter);

#define PAINT_MARBLES_ON_LEAF()                                                \
  for (size_t n = 0; n < 7; ++n, ++it) {                                       \
    paint_marble(shift_local, painter, it, center, n + 2, r);                  \
  }                                                                            \
  move_to_next_section(painter, internal_dist_angle);                          \
  for (size_t n = 9; n >= 7; --n, ++it) {                                      \
    paint_marble(-shift_local, painter, it, center, n + 2, r);                 \
  }

SpinPuzzleWidget::SpinPuzzleWidget(int win_width,
                                   int win_heigth,
                                   TypePuzzle typePuzzle,
                                   QWidget* parent)
  : QWidget(parent)
  , m_typePuzzle(typePuzzle)
{

  create_widget_buttons();
  create_widget_timers();
  setup_initial_configuration();

  set_size(win_width, win_heigth);
  reset_leaf_colors();

  this->setFocus();
}

void
SpinPuzzleWidget::create_widget_timers()
{
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
}

void
SpinPuzzleWidget::setup_initial_configuration()
{
  if (isInteractiveGame()) {
    load_configuration();
    m_game.set_config(m_config);
  }
}

void
SpinPuzzleWidget::create_widget_buttons()
{
  if (isInteractiveGame()) {
    m_spin_north = new QPushButton(this);
    m_spin_east = new QPushButton(this);
    m_spin_west = new QPushButton(this);
    // m_reset_btn = new QPushButton("RESET", this);
    // m_start_btn = new QPushButton("START", this);
    // m_load_btn = new QPushButton("LATEST", this);
    // m_save_btn = new QPushButton("SAVE", this);
    m_reset_btn = new QPushButton("⟲", this);
    m_start_btn = new QPushButton("▶", this);
    m_load_btn = new QPushButton("⟪", this);
    m_save_btn = new QPushButton("⇓", this);
  }
  m_twist_side = new QPushButton(this);

  connect_widget_buttons();
}

void
SpinPuzzleWidget::connect_widget_buttons()
{
  if (isInteractiveGame()) {
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

    connect(
      m_reset_btn, &QPushButton::released, this, &SpinPuzzleWidget::reset_game);

    connect(
      m_start_btn, &QPushButton::released, this, &SpinPuzzleWidget::start_game);

    connect(m_load_btn, &QPushButton::released, this, [this] {
      m_paint_congratulations = false;
      stop_spinning_winning();
      load_latest_game();
      update();
    });

    connect(m_save_btn,
            &QPushButton::released,
            this,
            &SpinPuzzleWidget::save_progress);
  }

  if (isInteractiveGame()) {
    connect(m_twist_side, &QPushButton::released, this, [this] {
      this->m_game.swap_side();
      update();
    });
  } else {
    connect(m_twist_side, &QPushButton::released, this, [this] {
      this->m_game.swap_side();
      update();
    });
    connect(m_twist_side, &QPushButton::pressed, this, [this] {
      this->m_game.swap_side();
      update();
    });
  }
}

void
SpinPuzzleWidget::save_progress()
{
  if (m_game.is_game_solved() ||
      (!(m_timer->isActive() || m_elapsed_time > 0))) {
    return;
  }
  if (do_save_progress()) {
    QMessageBox(QMessageBox::Information, "progress", "progress saved.").exec();
    update();
  }
}

void
SpinPuzzleWidget::reset_recording()
{
  this->stop_recording();
  // m_rec_btn->setText("REC");
}

void
SpinPuzzleWidget::exec_puzzle_records_dialog()
{
  std::vector<puzzle::SpinPuzzleRecord> records;
  load_records(records);
  m_history_widget =
    new SpinPuzzleHistoryWidget(m_win_width, m_win_height, this, records);
  m_history_widget->setFixedSize(m_length, m_length);
  m_history_widget->show();
  m_history_widget->setVisible(true);
}

void
SpinPuzzleWidget::exec_puzzle_config_dialog()
{
  if (m_timer->isActive()) {
    QMessageBox(QMessageBox::Warning,
                "configuration",
                "Cannot chage configuration while a game is running.")
      .exec();
    return;
  }
  m_config_widget = new SpinPuzzleConfigurationWidget(
    m_win_width, m_win_height, this, m_config);
  m_config_widget->show();
  m_config_widget->setVisible(true);
}

void
SpinPuzzleWidget::update_configuration(const puzzle::Configuration& config)
{
  m_config = config;
  std::ofstream file(m_files.get_config_puzzle_file());
  if (file.is_open()) {
    std::stringstream s;
    m_config.dump(s);
    file << s.str();
  }
  m_game.set_config(m_config);
}

bool
SpinPuzzleWidget::load_configuration()
{
  std::ifstream file(m_files.get_config_puzzle_file());
  if (file.is_open()) {
    std::stringstream s;
    s << file.rdbuf();
    m_config.load(s);
    return true;
  }
  return false;
}

double
SpinPuzzleWidget::get_height_button_bottom() const
{
  return 17.0 / 10.0 * m_length / 24.0;
}

double
SpinPuzzleWidget::get_width_button_bottom() const
{
  return 5.0 * m_length / 24.0;
}

double
SpinPuzzleWidget::get_length_status_square() const
{
  return m_length / 24.0;
}

void
SpinPuzzleWidget::delete_game_recordings()
{
  // delete all recordings
  QDir dir(m_files.get_recoding_puzzle_directory().c_str());
  dir.setNameFilters(QStringList() << "*.*");
  dir.setFilter(QDir::Files);
  foreach (QString dirFile, dir.entryList()) {
    if (dirFile.startsWith("recording_")) {
      dir.remove(dirFile);
    }
  }
}

void
SpinPuzzleWidget::delete_puzzle_files()
{
  std::vector<std::string> files;
  files.emplace_back((m_files.get_current_puzzle_file().c_str()));
  files.emplace_back((m_files.get_puzzle_file().c_str()));
  files.emplace_back((m_files.get_records_puzzle_file().c_str()));
  // files.emplace_back((m_files.get_config_puzzle_file().c_str()));
  for (const auto& d : files) {
    QFile file(d.c_str());
    file.remove();
  }
}

int
SpinPuzzleWidget::messageBoxDeleteFiles()
{
  auto msg = QMessageBox(QMessageBox::Warning,
                         "reset",
                         "Do you want to delete all saved puzzles",
                         QMessageBox::Ok | QMessageBox::Cancel);
  msg.setInformativeText("This operation cannot be reversed!");
  msg.setDefaultButton(QMessageBox::Cancel);
  return msg.exec();
}

void
SpinPuzzleWidget::reset_file_app()
{
  int delete_progress = messageBoxDeleteFiles();
  if (delete_progress == QMessageBox::Cancel) {
    return;
  }
  delete_puzzle_files();
  delete_game_recordings();
  reset_game();
  delete_history_popup();
  delete_config_popup();
  update();
}

bool
SpinPuzzleWidget::do_save_progress()
{
  auto file_name = m_files.get_current_puzzle_file();
  std::ofstream f(file_name, std::ios::trunc);
  if (f.is_open()) {
    puzzle::SpinPuzzleRecord record(
      m_config.name(), m_elapsed_time, m_config.level(), m_game);
    record.serialize(f);
    f.close();
    return true;
  }
  return false;
}

bool
SpinPuzzleWidget::quit()
{
  if (m_elapsed_time > 0 && !m_solved) {
    int save = QMessageBox(QMessageBox::Question,
                           "save?",
                           "Do you want to save your progress?",
                           QMessageBox::Ok | QMessageBox::Cancel)
                 .exec();
    if (save == QMessageBox::Ok) {
      do_save_progress();
    }
  }
  int do_quit = QMessageBox(QMessageBox::Question,
                            "quit?",
                            "Do you want to quit the app?",
                            QMessageBox::Ok | QMessageBox::Cancel)
                  .exec();
  return do_quit == QMessageBox::Ok;
}

bool
SpinPuzzleWidget::start_recording()
{
  if (m_recorderPtr == nullptr) {
    m_recorderPtr = std::make_shared<puzzle::Recorder>();
  }
  m_recorderPtr->reset();
  m_game.attach_recorder(m_recorderPtr);
  m_recorderPtr->rec(m_game);
  return true;
}

bool
SpinPuzzleWidget::stop_recording()
{
  if (m_recorderPtr == nullptr || !m_recorderPtr->isRecording()) {
    return false;
  }
  m_recorderPtr->stop();
  m_game.detached_recorder();
  return true;
}

void
SpinPuzzleWidget::store_puzzle_begin()
{
  std::ofstream f(m_files.get_puzzle_file(), std::ios::trunc);
  if (f.is_open()) {
    puzzle::SpinPuzzleRecord record(
      m_config.name(), m_elapsed_time, m_config.level(), m_game);
    record.serialize(f);
    f.close();
  }
}

std::string
SpinPuzzleWidget::store_recorded_game() const
{
  if (m_recorderPtr == nullptr || m_recorderPtr->isRecording()) {
    return "NONE";
  }
  std::cout << "[INFO] storing file into "
            << m_files.get_recoding_puzzle_directory() << "\n";
  std::string name = m_files.get_recoding_puzzle_name();
  std::ofstream f(m_files.get_recoding_puzzle(name), std::ios::trunc);
  if (f.is_open()) {
    m_recorderPtr->serialize(f);
    f.close();
  }
  return name;
}

int
SpinPuzzleWidget::load_records(
  std::vector<puzzle::SpinPuzzleRecord>& games) const
{
  std::cout << "[INFO] loading records from "
            << m_files.get_records_puzzle_file() << "\n";
  games.clear();
  int max_time = 0;
  std::ifstream f1(m_files.get_records_puzzle_file());
  int t = 0;
  if (f1.is_open()) {
    std::string s;
    f1 >> s;
    if (s == "records:") {
      while (!f1.eof()) {
        puzzle::SpinPuzzleRecord r;
        if (!r.load(f1)) {
          break;
        }
        max_time = std::max(max_time, r.time());
        games.emplace_back(r);
      }
    }
  }
  return max_time;
}

bool
SpinPuzzleWidget::store_puzzle_record(const std::string& recording_name) const
{
  puzzle::SpinPuzzleRecord record{};
  std::ifstream f0(m_files.get_puzzle_file());
  // load beginning
  if (f0.is_open()) {
    record.load(f0);
    record.update_time(m_elapsed_time);
    f0.close();
  }
  record.set_file_recording(recording_name);
  bool ok = store_puzzle_record(record);
  return ok;
}

bool
SpinPuzzleWidget::store_puzzle_record() const
{
  return store_puzzle_record("NONE");
}

bool
SpinPuzzleWidget::store_puzzles_record(
  std::vector<puzzle::SpinPuzzleRecord> records) const
{
  std::sort(records.begin(),
            records.end(),
            [](const puzzle::SpinPuzzleRecord& first,
               const puzzle::SpinPuzzleRecord& second) {
              if (first.level() == second.level()) {
                return first.time() < second.time();
              } else {
                return first.level() > second.level();
              }
            });

  std::ofstream f(m_files.get_records_puzzle_file());
  if (!f.is_open()) {
    return false;
  }
  f << "records:\n";
  int m = records.size();
  for (int n = 0; n < std::min(m, m_max_saved_games); ++n) {
    records[n].serialize(f);
  }
  f.close();

  return true;
}

bool
SpinPuzzleWidget::store_puzzle_record(
  const puzzle::SpinPuzzleRecord& record) const
{
  // store in records - max 5
  std::vector<puzzle::SpinPuzzleRecord> games;
  // check if current is faster then the others
  int max_time = load_records(games);
  int size = games.size();
  if (size > m_max_saved_games && record.time() > max_time) {
    return false;
  }

  auto game = record.game();
  auto current = game.current_time_step();
  // check for duplicates
  bool found_duplicate = false;
  for (auto& p : games) {
    auto g = p.game();
    if (g.current_time_step() == current) {
      if (p.time() > record.time()) {
        p.update_time(record.time());
        p.update_username(record.username());
      }
      found_duplicate = true;
      break;
    }
  }
  // add to games
  if (!found_duplicate) {
    games.emplace_back(record);
  }

  store_puzzles_record(games);
  return !found_duplicate;
}

void
SpinPuzzleWidget::start_timer()
{
  m_timer->start(1000);
}

// TODO:
// do not overwrite while iterating
void
SpinPuzzleWidget::load(int index, puzzle::SpinPuzzleGame& game)
{
  m_solved = false;
  m_elapsed_time = 0;
  std::string path;
  path = m_files.get_records_puzzle_file();

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
    puzzle::SpinPuzzleRecord record;
    if (!record.load(f)) {
      msg.setInformativeText("Puzzle not found.");
      f.close();
      return;
    }
    m_elapsed_time = record.time();
    set_game(record.game());
    current++;
  } while (current != index);
  f.close();
}

void
SpinPuzzleWidget::load(int index)
{
  load(index, m_game);
}

void
SpinPuzzleWidget::load_latest_game()
{
  m_solved = false;
  int do_load = QMessageBox::Cancel;
  QMessageBox msgBox(QMessageBox::Question,
                     "load",
                     "Are you sure you what to reload the puzzle?");
  QAbstractButton* pButtonLatest =
    msgBox.addButton(tr("latest progress"), QMessageBox::YesRole);
  QAbstractButton* pButtonBegin =
    msgBox.addButton(tr("starting configuration"), QMessageBox::YesRole);
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
    path = m_files.get_current_puzzle_file();
  } else if (msgBox.clickedButton() == pButtonBegin) {
    path = m_files.get_puzzle_file();
  } else {
    return;
  }

  std::ifstream f(path);
  if (f.is_open()) {
    puzzle::SpinPuzzleRecord record{};
    if (!record.load(f)) {
      QMessageBox(QMessageBox::Warning, "load", "Unable to load latest game")
        .exec();
      return;
    }
    set_game(record.game());
    m_elapsed_time = record.time();
    reset_recording();
    f.close();
  }
  start_timer();
}

// TODO: after impl. records this is broken!!
bool
SpinPuzzleWidget::import_game()
{
  if (m_timer->isActive()) {
    QMessageBox(
      QMessageBox::Information,
      "info",
      "A game is running: press first RESET before importing a new puzzle",
      QMessageBox::Ok)
      .exec();
    return false;
  }
  bool ok;
  QString text = QInputDialog::getText(
    this, tr("import"), tr("game:"), QLineEdit::Normal, "", &ok);
  if (ok && !text.isEmpty()) {
    puzzle::SpinPuzzleRecord record{};
    if (!record.decrypt(text.toStdString())) {
      QMessageBox(QMessageBox::Warning,
                  "error",
                  "Not a valid game - load failed",
                  QMessageBox::Ok)
        .exec();
      return false;
    }
    bool inserted = store_puzzle_record(record);
    if (!inserted) {
      QMessageBox(QMessageBox::Information,
                  "info",
                  "Game already present",
                  QMessageBox::Ok)
        .exec();
      return false;
    }
    start_with_game(record.game());
    return true;
  }
  return false;
}

void
SpinPuzzleWidget::do_spin_north()
{
  int color_side = static_cast<uint8_t>(m_game.get_active_side());
  int color_opposite = (color_side + 1) % 2;
  std::swap(m_colors_leaves[color_side][0], m_colors_leaves[color_opposite][0]);
  std::swap(m_colors_leaves_internal[color_side][0],
            m_colors_leaves_internal[color_opposite][0]);
  this->m_game.spin_leaf(puzzle::LEAF::NORTH);
}

void
SpinPuzzleWidget::do_spin_east()
{
  int color_side = static_cast<uint8_t>(m_game.get_active_side());
  int color_opposite = (color_side + 1) % 2;
  std::swap(m_colors_leaves[color_side][1], m_colors_leaves[color_opposite][2]);
  std::swap(m_colors_leaves_internal[color_side][1],
            m_colors_leaves_internal[color_opposite][2]);
  this->m_game.spin_leaf(puzzle::LEAF::EAST);
}

void
SpinPuzzleWidget::do_spin_west()
{
  int color_side = static_cast<uint8_t>(m_game.get_active_side());
  int color_opposite = (color_side + 1) % 2;
  std::swap(m_colors_leaves[color_side][2], m_colors_leaves[color_opposite][1]);
  std::swap(m_colors_leaves_internal[color_side][2],
            m_colors_leaves_internal[color_opposite][1]);
  this->m_game.spin_leaf(puzzle::LEAF::WEST);
}

void
SpinPuzzleWidget::reset_leaf_colors()
{
  m_colors_leaves[0] = { QColor(0xee6c6c), QColor(0x756ef3), QColor(0xf3dd6e) };
  m_colors_leaves_internal[0] = { QColor(0x7e413c),
                                  QColor(0x93a1d7),
                                  QColor(0xe2c67f) };
  m_colors_leaves_body[0] = { QColor(0xee6c6d),
                              QColor(0x756ef4),
                              QColor(0xf3dd60) };

  m_colors_leaves[1] = { QColor(0x84bf72), QColor(0x1000ab), QColor(0xefc111) };
  m_colors_leaves_internal[1] = { QColor(0x619e02),
                                  QColor(0x029e98),
                                  QColor(0xf2ea41) };
  m_colors_leaves_body[1] = { QColor(0x84bf72),
                              QColor(0x1000ab),
                              QColor(0xefc111) };
}

void
SpinPuzzleWidget::paint_status()
{
  if (!isInteractiveGame()) {
    return;
  }
  const auto& side = m_game.get_side(m_game.get_active_side());

  QPainter painter_status(this);

  auto s = get_length_status_square();
  QString hello = QString("Hi, ") + QString(m_config.name().c_str());
  painter_status.drawText(QPoint(s / 5, s - s / 5), hello);

  if (side.get_trifoild_status() == puzzle::TREFOIL::BORDER_ROTATION ||
      side.is_border_rotation_possible()) {
    painter_status.setBrush(Qt::green);
  } else if (side.get_trifoild_status() == puzzle::TREFOIL::INVALID) {
    painter_status.setBrush(Qt::red);
  } else {
    painter_status.setBrush(Qt::yellow);
  }

  // painter_status.translate(0, s);
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

void
SpinPuzzleWidget::paint_timer()
{
  const int L = this->m_length;
  QPainter painter_status(this);
  double metric = 1.0;
  if (m_game.is_game_solved()) {
    painter_status.setBrush(Qt::green);
    if (m_timer->isActive() && m_elapsed_time > 0 && !m_solved) {
      this->stop_recording();
      auto name = store_recorded_game();
      store_puzzle_record(name);
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
    metric = puzzle::MetricProvider().naive_disorder(m_game);
  }
  if (m_paint_congratulations) {
    paint_congratulation(painter_status);
  }
  int win_width = (isInteractiveGame()) ? m_win_width : 8 * L / 24;
  int win_heigth = (isInteractiveGame()) ? 0 : (L - L / 24);
  if (isInteractiveGame()) {
    painter_status.drawRect(
      win_width - 8 * L / 24, win_heigth, 8 * L / 24, L / 24);
  }
  QString time = QString("time: %3:%2:%1")
                   .arg(m_elapsed_time % 60, 2, 10, QChar('0'))
                   .arg(m_elapsed_time / 60, 2, 10, QChar('0'))
                   .arg(m_elapsed_time / 60 / 60, 2, 10, QChar('0'));
  painter_status.drawText(QPoint(win_width - 7 * L / 24, 2 * L / 24), time);
  QString metric_str = QString("metric: %1").arg(metric);
  painter_status.drawText(QPoint(win_width - 7 * L / 24, 3 * L / 24),
                          metric_str);
}

void
SpinPuzzleWidget::paint_congratulation(QPainter& painter)
{
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

void
SpinPuzzleWidget::set_size(int win_width, int win_height)
{
  m_length = std::min(win_width, win_height);
  m_tx = (win_width - m_length) / 2.0;
  m_ty = (win_height - m_length) / 2.0;
  m_win_width = win_width;
  m_win_height = win_height;

  const int r = get_radius_internal() - width;
  radius_marble = r * sin(M_PI / 5) / 2;
  create_polygon(this->m_polygon);
  const int L = this->m_length;

  QPixmap pixmap(":/images/swap_side.png");
  QIcon ButtonIcon(pixmap);
  m_twist_side->setIcon(ButtonIcon);
  m_twist_side->setGeometry(
    QRect(0 + m_tx, 5 * L / 24 + m_ty, 5 * L / 24, 2 * L / 24));
  m_twist_side->setIconSize(QSize(5 * L / 24, 2 * L / 24));

  if (isInteractiveGame()) {
    double s = 3 * L / 48;

    auto pixmap = QPixmap(":/images/spin_leaf.png");
    QIcon spinIcon(pixmap.scaled(s, s));

    m_spin_north->setIcon(spinIcon);
    m_spin_east->setIcon(spinIcon);
    m_spin_west->setIcon(spinIcon);

    m_spin_north->setIconSize(QSize(5 * L / 24, L / 24));
    m_spin_east->setIconSize(QSize(5 * L / 24, L / 24));
    m_spin_west->setIconSize(QSize(5 * L / 24, L / 24));

    m_spin_north->setGeometry(QRect(L / 2 + m_tx, 0 + m_ty, s, s));
    m_spin_east->setGeometry(QRect(L - s + m_tx, 2 * L / 3 + m_ty, s, s));
    m_spin_west->setGeometry(QRect(0 + m_tx, 2 * L / 3 + m_ty, s, s));

    double h = get_height_button_bottom();
    double w = get_width_button_bottom();

    double t = (m_win_width - 4 * w) / 2; // center buttons

    m_start_btn->setGeometry(QRect(t + 0 * w, m_win_height - h, w, h));
    m_reset_btn->setGeometry(QRect(t + 1 * w, m_win_height - h, w, h));
    m_load_btn->setGeometry(QRect(t + 2 * w, m_win_height - h, w, h));
    m_save_btn->setGeometry(QRect(t + 3 * w, m_win_height - h, w, h));
    // m_rec_btn->setGeometry(QRect(t + 4 * w, m_win_height - h, w, h));
  }
}

void
SpinPuzzleWidget::move_to_next_section(QPainter& painter, int angle) const
{
  const int L = this->m_length;
  const QPoint center = QPoint(L / 2, (2 * std::sqrt(3) + 3) * L / 12);
  painter.translate(center);
  painter.rotate(angle);
  painter.translate(-center);
}

void
SpinPuzzleWidget::paint_puzzle_section(QPainter& painter,
                                       bool active,
                                       QColor color,
                                       QColor color_internal,
                                       QColor color_body) const
{
  const int L = this->m_length;
  const double R = L / 4;
  const QPoint centerTop = QPoint(L / 2, L / 4);
  const int internalRadius = get_radius_internal();
  const bool primary = m_game.get_active_side() == puzzle::SIDE::FRONT;

  painter.setBrush(color);
  auto pen = painter.pen();
  if (active) {
    QPen activePen(Qt::black, 3);
    painter.setPen(activePen);
  }
  // The span angle for an ellipse segment to angle , which is in 16ths of a
  // degree 360 * 16 / 2 is half the full ellipse
  painter.drawPie(R, 0, 2 * R, 2 * R, 0, 360 * 8);

  painter.setBrush(color_internal);
  painter.drawPie(R + (R - internalRadius),
                  (R - internalRadius),
                  2 * internalRadius,
                  2 * internalRadius,
                  0,
                  360 * 8);

  painter.setBrush(color);
  painter.drawPie(R + (R - internalRadius + 2 * width),
                  (R - internalRadius + 2 * width),
                  2 * (internalRadius - 2 * width),
                  2 * (internalRadius - 2 * width),
                  0,
                  360 * 8);

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
  painter.drawPie(R + (R - internalRadius),
                  (R - internalRadius),
                  2 * internalRadius,
                  2 * internalRadius,
                  360 * 8,
                  360 * 8);

  painter.drawLine(L / 2 - R, L / 4, L / 2 + R, L / 4);

  painter.setBrush(originalBrush);
  painter.setPen(pen);
}

void
SpinPuzzleWidget::paint_background()
{
  QPainter painter(this);
  QColor background;
  if (isInteractiveGame())
    background = QColor(208, 228, 225);
  else if (isHistoryShow()) {
    background = QColor(0xf0eb9a);
  } else if (isReplayGame()) {
    background = QColor(0x00eb9a);
  }
  painter.setBrush(background);
  painter.drawRect(0, 0, m_win_width - 1, m_win_height - 1);
}

void
SpinPuzzleWidget::paintEvent(QPaintEvent* /*event*/)
{
  paint_background();
  paint_status();
  paint_game();
  paint_marbles();
  paint_timer();
}

void
SpinPuzzleWidget::paint_game()
{
  auto& game_side = m_game.get_side(m_game.get_active_side());
  const int L = this->m_length;
  const QPoint center = QPoint(L / 2, (2 * std::sqrt(3) + 3) * L / 12);

  QPainter painter(this);
  painter.translate(QPoint(m_tx, m_ty));
  move_to_next_section(painter, m_rotation_congratulation);
  painter.save();
  // ====================================================================== //
  // NORTH
  const int COLOR_SIDE = static_cast<uint8_t>(m_game.get_active_side());
  const int NORTH = static_cast<uint8_t>(puzzle::LEAF::NORTH);
  const int EAST = static_cast<uint8_t>(puzzle::LEAF::EAST);
  const int WEST = static_cast<uint8_t>(puzzle::LEAF::WEST);
  PAINT_PUZZLE_SECTION(NORTH);
  PAINT_PUZZLE_SECTION(EAST);
  PAINT_PUZZLE_SECTION(WEST);
  // ====================================================================== //

  painter.restore();
  painter.save();
  auto originalBrush = painter.brush();
  auto originalPen = painter.pen();
  const int internalRadius = get_radius_internal() + width / 2;
  QRadialGradient radialGrad(center, internalRadius);
  if (m_game.get_keybord_state() == puzzle::LEAF::CENTER) {
    QPen activePen(Qt::black, 3);
    painter.setPen(activePen);
  }
  radialGrad.setColorAt((COLOR_SIDE) % 2, Qt::magenta);
  radialGrad.setColorAt((COLOR_SIDE + 1) % 2, Qt::darkMagenta);
  // radialGrad.setColorAt(1, Qt::magenta);
  painter.setBrush(radialGrad);
  // painter.setBrush(Qt::magenta);
  painter.drawEllipse(center, internalRadius, internalRadius);
  painter.setBrush(originalBrush);
  painter.setPen(originalPen);

  painter.restore();
  // NORTH
  move_to_next_section(painter, game_side.get_phase_shift_internal_disk());
  painter.setBrush(Qt::darkBlue);
  paint_internal_circular_guide(painter, Qt::darkBlue);
  // EAST
  move_to_next_section(painter);
  paint_internal_circular_guide(painter, Qt::darkBlue);
  // WEST
  move_to_next_section(painter);
  paint_internal_circular_guide(painter, Qt::darkBlue);
  // ====================================================================== //
}

void
SpinPuzzleWidget::paint_marbles()
{
  QPainter painter(this);
  painter.translate(QPoint(m_tx, m_ty));
  move_to_next_section(painter, m_rotation_congratulation);

  painter.save();
  auto& game_side = m_game.get_side(m_game.get_active_side());
  if (game_side.get_trifoild_status() != puzzle::TREFOIL::BORDER_ROTATION) {
    do_paint_marbles(painter);
  } else {
    do_paint_marbles_on_border(painter);
  }
}

void
SpinPuzzleWidget::do_paint_marbles(QPainter& painter)
{
  auto& game_side = m_game.get_side(m_game.get_active_side());
  // ====================================================================== //
  // paint marbles
  paint_marbles_on_leaf(painter, puzzle::LEAF::NORTH);
  move_to_next_section(painter);
  paint_marbles_on_leaf(painter, puzzle::LEAF::EAST);
  move_to_next_section(painter);
  paint_marbles_on_leaf(painter, puzzle::LEAF::WEST);
  // -------------------------- //
  painter.restore();
  // -------------------------- //
  move_to_next_section(painter, game_side.get_phase_shift_internal_disk());
  paint_marbles_on_internal_circle(painter, puzzle::LEAF::NORTH);
  move_to_next_section(painter);
  paint_marbles_on_internal_circle(painter, puzzle::LEAF::EAST);
  move_to_next_section(painter);
  paint_marbles_on_internal_circle(painter, puzzle::LEAF::WEST);
}

void
SpinPuzzleWidget::do_paint_marbles_on_border(QPainter& painter)
{
  painter.restore();
  auto& game_side = m_game.get_side(m_game.get_active_side());
  auto it = game_side.begin();
  const int L = this->m_length;
  const int r = get_radius_internal() - width;
  const QPoint center = QPoint(L / 2, L / 4);
  double shift_local = qDegreesToRadians(it.get_angle() * 12);
  double internal_dist_angle = game_side.get_phase_shift_internal_disk();

  // NORTH
  PAINT_MARBLES_ON_LEAF();
  // EAST
  move_to_next_section(painter, 120 - internal_dist_angle);
  PAINT_MARBLES_ON_LEAF();
  // WEST
  move_to_next_section(painter, 120 - internal_dist_angle);
  PAINT_MARBLES_ON_LEAF();
}

QColor
SpinPuzzleWidget::toQtColor(puzzle::Color value)
{
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

QColor
SpinPuzzleWidget::toMarbleColor(puzzle::Color value)
{
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

void
SpinPuzzleWidget::paint_marble(double shift_local,
                               QPainter& painter,
                               puzzle::SpinPuzzleSide<>::iterator it,
                               const QPoint center,
                               size_t n,
                               const int r)
{
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

void
SpinPuzzleWidget::paint_marbles_on_leaf(QPainter& painter, puzzle::LEAF leaf)
{
  auto& game_side = m_game.get_side(m_game.get_active_side());
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

void
SpinPuzzleWidget::paint_marbles_on_internal_circle(QPainter& painter,
                                                   puzzle::LEAF leaf)
{
  auto& game_side = m_game.get_side(m_game.get_active_side());
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

void
SpinPuzzleWidget::create_polygon(QPolygon& polygon)
{
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
double
SpinPuzzleWidget::get_radius_internal() const
{
  const int L = this->m_length;
  // return 0.19945 * L;
  // return (isInteractiveGame()) ? 0.185 * L : 0.2 * L;
  return 0.185 * L;
}

inline void
SpinPuzzleWidget::paint_internal_circular_guide(QPainter& painter,
                                                QColor color) const
{
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

void
SpinPuzzleWidget::mousePressEvent(QMouseEvent* e)
{
  if (!isInteractiveGame()) {
    return;
  }

  if (m_paint_congratulations) {
    // const int L = this->m_length;
    // auto rec = QRect(L / 4 + m_tx, L / 2 - L / 16 + m_ty, L / 2, L / 8);
    // if (rec.contains(e->pos())) {
    // reset();
    // }
    reset_game();
    return;
  }

#if HIDDEN_BUTTON == 1
  const int L = this->m_length;
  if (e->pos().x() > m_win_width - L / 24 &&
      e->pos().y() > m_win_height - L / 24) {
    int pirate = QMessageBox(QMessageBox::Question,
                             "pirate",
                             "Fake resolution of game",
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

void
SpinPuzzleWidget::mouseMoveEvent(QMouseEvent* ev)
{
  if (!isInteractiveGame()) {
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

bool
SpinPuzzleWidget::can_rotate_internal() const
{
  return true;
}

bool
SpinPuzzleWidget::mouse_event_inside_internal_circle(QPoint pos)
{
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

bool
SpinPuzzleWidget::mouse_event_inside_leaf(QPoint pos,
                                          QPoint center,
                                          puzzle::LEAF leaf)
{
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

  m_game.rotate_marbles(leaf, delta_angle);
  m_lastPositionMause = pos;

  return true;
}

double
SpinPuzzleWidget::get_scaled_angle(double a1, double a2) const
{
  return (a1 - a2) * m_config.speed() * M_PI;
}

bool
SpinPuzzleWidget::is_mause_on_leaf_marbles(QPoint pos, QPoint center) const
{
  const double internalRadius = get_radius_internal();

  pos = pos - center;
  auto r = sqrt(pos.dotProduct(pos, pos));
  if (r < internalRadius && r > internalRadius - 2 * radius_marble) {
    return true;
  }
  return false;
}

void
SpinPuzzleWidget::keyPressEvent(QKeyEvent* e)
{
  if (!isInteractiveGame()) {
    return;
  }
  if (processKey(e->key(), 1.0 / 4.0)) {
    update();
    this->setFocus();
  }
}

void
SpinPuzzleWidget::resizeEvent(QResizeEvent* e)
{
  this->set_size(e->size().width(), e->size().height());
  update();
}

void
SpinPuzzleWidget::set_game(const puzzle::SpinPuzzleGame& game)
{
  set_game(m_elapsed_time, game);
}

void
SpinPuzzleWidget::set_game(int time, const puzzle::SpinPuzzleGame& game)
{
  reset_recording();
  m_elapsed_time = time;
  m_game = game;
  m_game.set_config(m_config);
}

void
SpinPuzzleWidget::add_replay_popup(SpinPuzzleReplayWidget* m_replay_widget)
{
  m_replay_widget = m_replay_widget;
}

void
SpinPuzzleWidget::delete_replay_popup()
{
  m_replay_widget->deleteLater();
  // m_replay_widget = nullptr;
}

void
SpinPuzzleWidget::delete_history_popup()
{
  m_history_widget->deleteLater();
  // m_history_widget = nullptr;
}

void
SpinPuzzleWidget::delete_config_popup()
{
  m_config_widget->deleteLater();
  // m_config_widget = nullptr;
}

void
SpinPuzzleWidget::set_elapsed_time(int t)
{
  m_elapsed_time = t;
}

puzzle::SpinPuzzleGame&
SpinPuzzleWidget::get_game()
{
  return m_game;
}

void
SpinPuzzleWidget::closeEvent(QCloseEvent* /*event*/)
{
  if (m_history_widget) {
    m_history_widget->deleteLater();
  }
  if (m_config_widget) {
    m_config_widget->deleteLater();
  }
}

bool
SpinPuzzleWidget::processKey(int key, double fraction_angle)
{
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

void
SpinPuzzleWidget::reset_game()
{
  if ((m_timer->isActive() || m_elapsed_time > 0) && !m_solved) {
    auto msg = QMessageBox(QMessageBox::Question,
                           "reset",
                           "Are you sure you waht to reset the puzzle?",
                           QMessageBox::Ok | QMessageBox::Cancel);
    msg.setInformativeText("You will lose yuor progress");
    if (msg.exec() == QMessageBox::Ok) {
      // Save was clicked
      m_paint_congratulations = false;
      stop_spinning_winning();
      m_elapsed_time = 0;
      m_timer->stop();
      set_game(puzzle::SpinPuzzleGame());
      m_game.set_config(m_config);
      reset_leaf_colors();
    }
  }
  m_paint_congratulations = false;
  m_lastPositionMause = QPoint(-1, -1);
  stop_spinning_winning();
  update();
}

void
SpinPuzzleWidget::stop_spinning_winning()
{
  if (m_congratulation_timer->isActive()) {
    m_congratulation_timer->stop();
    m_rotation_congratulation = 0;
  }
}

void
SpinPuzzleWidget::start_game()
{
  do_start_game(1);
}

void
SpinPuzzleWidget::do_start_game(int shuffle_level)
{
  m_paint_congratulations = false;
  stop_spinning_winning();
  if (m_timer->isActive()) {
    QMessageBox(QMessageBox::Information,
                "info",
                "A game is running: press first RESET to start a new puzzle",
                QMessageBox::Ok)
      .exec();
    return;
  }

  m_elapsed_time = 0;
  m_solved = false;
  start_timer();
  if (shuffle_level > 0) {
    // TODO: use some metric to determin the difficulty, for example
    // fill-grad for leaf + max distance marbles of the same color ...
    int n_random_commands = 100 + std::pow(m_config.level(), 5);
    // qDebug() << "Shuffling with " << n_random_commands << " commands";
    m_game.shuffle(0, n_random_commands);
  }
  if (isInteractiveGame()) {
    m_files.create_filesystem();
    store_puzzle_begin();
    start_recording();
    do_save_progress();
  }

  update();
  this->setFocus();
}

void
SpinPuzzleWidget::start_with_game(const puzzle::SpinPuzzleGame& game)
{
  set_game(game);
  do_start_game(0);
}
