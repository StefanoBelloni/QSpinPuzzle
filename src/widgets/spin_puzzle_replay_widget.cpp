#include "spin_puzzle_replay_widget.h"

#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "spin_puzzle_widget.h"

SpinPuzzleReplayWidget::SpinPuzzleReplayWidget(int win_width,
                                               int win_heigth,
                                               SpinPuzzleWidget* parent,
                                               const puzzle::Recorder& recorder)
  : m_win_width(win_width)
  , m_win_heigth(win_heigth)
  , m_parent(parent)
  , m_recorder(recorder)
{
  m_recorder.rewind(m_game);
  setup_ui();
  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, [this] {
    if (!this->m_recorder.isEnd()) {
      size_t n = this->m_recorder.play(m_game, 5 * m_speed);
      this->m_spinPuzzleWidget->set_game(this->m_recorder.current_time() / 1000,
                                         m_game);
      this->update();
    } else {
      this->m_timer->stop();
      this->m_play->setText("▶");
    }
  });
}

void
SpinPuzzleReplayWidget::setup_ui()
{
  create_buttons();
  setup_buttons();
  setup_layout();
}

void
SpinPuzzleReplayWidget::setup_layout()
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  QHBoxLayout* layout_btn = new QHBoxLayout;
  QHBoxLayout* layout_btn2 = new QHBoxLayout;

  m_spinPuzzleWidget = new SpinPuzzleWidget(
    m_win_width, m_win_heigth, SpinPuzzleWidget::TypePuzzle::SHOW_REPLAY, this);
  m_spinPuzzleWidget->set_game(m_game);
  this->setFixedSize(m_win_width, m_win_heigth);

  layout_btn->addWidget(m_play);
  layout_btn->addWidget(m_rewind);
  layout_btn->addWidget(m_next);
  layout_btn->addWidget(m_faster);
  layout_btn->addWidget(m_quit);

  layout->addWidget(m_spinPuzzleWidget);
  layout->addLayout(layout_btn);
  layout->setStretch(0, 10);
  layout->setStretch(1, 1);
}

void
SpinPuzzleReplayWidget::create_buttons()
{
  m_faster = new QPushButton(QString::number(m_speed), this);
  m_play = new QPushButton("▶", this);
  m_rewind = new QPushButton("⏮", this);
  m_next = new QPushButton("⏩", this);
  m_quit = new QPushButton("Quit", this);
}

void
SpinPuzzleReplayWidget::setup_buttons()
{
  connect(m_quit, &QPushButton::released, m_parent, [this] { this->close(); });
  connect(m_play, &QPushButton::released, this, [this] {
    static std::string_view pause("⏸");
    static std::string_view play("▶");
    if (!this->m_timer->isActive()) {
      this->m_timer->start(this->m_timer_step);
      this->m_play->setText(pause.data());
    } else {
      this->m_timer->stop();
      this->m_play->setText(play.data());
    }
  });
  connect(m_faster, &QPushButton::released, this, [this] {
    m_speed = m_speed + 1;
    if (m_speed > m_speed_max) {
      m_speed = 1;
    }
    this->m_faster->setText(QString::number(m_speed));
  });
  connect(m_rewind, &QPushButton::released, this, [this] {
    this->m_recorder.rewind(m_game);
    this->m_spinPuzzleWidget->set_game(m_game);
    this->update();
  });
  connect(m_next, &QPushButton::released, this, [this] {
    this->m_recorder.step_forward(m_game, m_speed);
    this->m_spinPuzzleWidget->set_game(this->m_recorder.current_time() / 1000,
                                       m_game);
    this->update();
  });
}
