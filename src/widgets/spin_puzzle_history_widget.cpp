#include "spin_puzzle_history_widget.h"

#include <QBoxLayout>
#include <QClipboard>
#include <QComboBox>
#include <QGridLayout>
#include <QGuiApplication>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "puzzle/spin_puzzle_cipher.h"
#include "spin_puzzle_widget.h"

#define DEBUG_CIPHER 0

SpinPuzzleHistoryWidget::SpinPuzzleHistoryWidget(
  int win_width,
  int win_heigth,
  SpinPuzzleWidget* parent,
  const std::vector<puzzle::SpinPuzzleRecord>&
    records) //, std::array<puzzle::SpinPuzzleGame>& game)
  : m_win_width(win_width)
  , m_win_heigth(win_heigth)
  , m_parent(parent)
  , m_games(records)
{
  QPushButton* cancel_btn = new QPushButton("Cancel", this);
  QPushButton* import_btn = new QPushButton("import", this);
  QPushButton* reset_files_btn = new QPushButton("Reset Files", this);

  connect(cancel_btn,
          &QPushButton::released,
          m_parent,
          &SpinPuzzleWidget::delete_history_popup);
  connect(reset_files_btn,
          &QPushButton::released,
          m_parent,
          &SpinPuzzleWidget::reset_file_app);
  connect(import_btn,
          &QPushButton::released,
          this,
          &SpinPuzzleHistoryWidget::import_game);

  m_stackedWidget = new QStackedWidget();

  QVBoxLayout* layout = new QVBoxLayout;
  QLabel* title =
    new QLabel((m_games.size() > 0) ? "Select the puzzle you want to retry"
                                    : "import a game");

  QHBoxLayout* layout_btn = new QHBoxLayout;
  layout_btn->addWidget(cancel_btn);
  layout_btn->addWidget(import_btn);
  layout_btn->addWidget(reset_files_btn);
  m_pageComboBox = new QComboBox();

  populateStackedWidget();

  layout->addWidget(title);
  layout->addWidget(m_stackedWidget);
  layout->addWidget(m_pageComboBox);
  layout->addLayout(layout_btn);

  connect(m_pageComboBox,
          &QComboBox::activated,
          m_stackedWidget,
          &QStackedWidget::setCurrentIndex);

  setLayout(layout);
}

void
SpinPuzzleHistoryWidget::import_game()
{
  if (m_parent->import_game()) {
    m_parent->delete_history_popup();
  }
}

void
SpinPuzzleHistoryWidget::populateStackedWidget()
{
  // auto *stackedWidget = new QStackedWidget();
  // m_pageComboBox->clear();
  size_t n = 0;
  for (auto& g : m_games) {
    m_stackedWidget->addWidget(this->get_puzzle(g));
    QString time = QString("time: %3:%2:%1")
                     .arg(g.time() % 60, 2, 10, QChar('0'))
                     .arg(g.time() / 60, 2, 10, QChar('0'))
                     .arg(g.time() / 60 / 60, 2, 10, QChar('0'));
    QString str = QString::number(n + 1) + QString("° Puzzle - level: ") +
                  QString::number(g.level()) + " -> " + time;
    str += "  (" + g.username() + ")";
    m_pageComboBox->addItem(tr(str.toStdString().c_str()));
    ++n;
  }
}

QWidget*
SpinPuzzleHistoryWidget::get_puzzle(const puzzle::SpinPuzzleRecord& record)
{
  QWidget* widget = new QWidget();
  QWidget* btns = new QWidget();

  QVBoxLayout* layout = new QVBoxLayout(widget);
  QHBoxLayout* layout_btn = new QHBoxLayout(btns);

  QPushButton* select = new QPushButton("challenge this puzzle");
  QPushButton* export_btn = new QPushButton("export to clipboard");
  QPushButton* delete_btn = new QPushButton("delete");
  layout_btn->addWidget(select);
  layout_btn->addWidget(export_btn);
  layout_btn->addWidget(delete_btn);
  btns->setLayout(layout_btn);

  auto w1 = new SpinPuzzleWidget(m_win_width, m_win_heigth, false, this);
  w1->set_game(record.game());

  layout->addWidget(w1);
  layout->addWidget(btns);
  widget->setLayout(layout);
  layout->setStretch(0, 10);
  layout->setStretch(1, 1);

  connect(select, &QPushButton::released, m_parent, [this] {
    m_parent->start_with_game(m_games[m_stackedWidget->currentIndex()].game());
    m_parent->delete_history_popup();
  });

  connect(export_btn, &QPushButton::released, m_parent, [this] {
    auto& record = m_games[m_stackedWidget->currentIndex()];
    QClipboard* clipboard = QGuiApplication::clipboard();
    std::string s = record.encrypt();
    clipboard->setText(QString(s.c_str()));
    auto m = QMessageBox(QMessageBox::Information,
                         "copyed",
                         "game copied to clipboard",
                         QMessageBox::Ok);
    m.exec();
    m_parent->delete_history_popup();
  });

  connect(delete_btn, &QPushButton::released, m_parent, [this] {
    if (m_games.size() == 0) {
      return;
    }
    if (QMessageBox(QMessageBox::Question,
                    "info",
                    "Are you sure to delete this puzzle?",
                    QMessageBox::Ok | QMessageBox::Cancel)
          .exec() != QMessageBox::Ok) {
      return;
    }
    m_games.erase(m_games.begin() + m_stackedWidget->currentIndex());
    m_parent->store_puzzles_record(m_games);
    m_parent->delete_history_popup();
  });

  return widget;
}
