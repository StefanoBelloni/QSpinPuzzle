#include "spin_puzzle_config_widget.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSlider>

SpinPuzzleConfigurationWidget::SpinPuzzleConfigurationWidget(
  int win_width,
  int win_heigth,
  SpinPuzzleWidget* parent,
  puzzle::Configuration& config)
  : m_parent(parent)
  , m_config(config)
{

  m_nameLineEdit = new QLineEdit(this);
  m_realism = new QSlider(Qt::Orientation::Horizontal, this);
  m_speed = new QSlider(Qt::Orientation::Horizontal, this);
  m_level = new QSlider(Qt::Orientation::Horizontal, this);

  if (config.name() != "QSpinPuzzle") {
    m_nameLineEdit->setText(config.name().c_str());
  }
  set_config_values(config);

  QVBoxLayout* principalLayout = new QVBoxLayout(this);

  QFormLayout* formLayout = new QFormLayout();

  auto style = R"(font: italic small "Times New Roman")";
  auto* usernamedesc = new QLabel("Enter the username");
  usernamedesc->setStyleSheet(style);
  auto* leveldesc = new QLabel("Select the difficult level for the puzzle");
  leveldesc->setStyleSheet(style);
  leveldesc->setWordWrap(true);
  auto* speeddesc = new QLabel("Select the speed when moving marbles");
  speeddesc->setStyleSheet(style);
  speeddesc->setWordWrap(true);
  auto* realismdesc =
    new QLabel("Select the realism of the game: in a more realistic game you "
               "have to be more precise when positioning the marbles");
  realismdesc->setStyleSheet(style);
  realismdesc->setWordWrap(true);

  QFrame* line1;
  line1 = new QFrame();
  line1->setFrameShape(QFrame::HLine);
  QFrame* line2;
  line2 = new QFrame();
  line2->setFrameShape(QFrame::HLine);
  QFrame* line3;
  line3 = new QFrame();
  line3->setFrameShape(QFrame::HLine);

  formLayout->addRow("  ", usernamedesc);
  formLayout->addRow(tr("Name Player:"), m_nameLineEdit);
  formLayout->addWidget(line1);

  formLayout->addRow("  ", leveldesc);
  formLayout->addRow(tr("Difficult Level:"), create_level_section());
  formLayout->addWidget(line2);

  formLayout->addRow("  ", speeddesc);
  formLayout->addRow(tr("Marbles Speed:"), create_speed_section());
  formLayout->addWidget(line3);

  formLayout->addRow("  ", realismdesc);
  formLayout->addRow(tr("Game Realism:"), create_realism_section());

  auto* buttonBox =
    new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  auto* reset = buttonBox->addButton(QDialogButtonBox::Reset);

  connect(buttonBox,
          &QDialogButtonBox::rejected,
          m_parent,
          &SpinPuzzleWidget::delete_config_popup);
  connect(buttonBox, &QDialogButtonBox::accepted, m_parent, [this] {
    if (QMessageBox(QMessageBox::Question,
                    "save",
                    "Do you want to save the setting",
                    QMessageBox::Ok | QMessageBox::Cancel)
          .exec() == QMessageBox::Cancel) {
      return;
    }
    puzzle::Configuration config;
    config.set_name(this->m_nameLineEdit->text().toStdString());
    config.set_level(this->m_level->value());
    config.set_speed(this->m_speed->value());
    config.set_realism(this->m_realism->value());
    m_parent->update_configuration(config);
    m_parent->delete_config_popup();
  });
  connect(reset, &QPushButton::clicked, m_parent, [this] {
    if (QMessageBox(QMessageBox::Question,
                    "reset",
                    "Do you want to reset the setting",
                    QMessageBox::Ok | QMessageBox::Cancel)
          .exec() == QMessageBox::Cancel) {
      return;
    }
    puzzle::Configuration config;
    config.set_name(this->m_config.name());
    m_parent->update_configuration(config);
    set_config_values(config);
    update();
    // m_parent->delete_config_popup();
  });

  principalLayout->addLayout(formLayout);
  principalLayout->addWidget(buttonBox);
}

void
SpinPuzzleConfigurationWidget::set_config_values(
  const puzzle::Configuration& config)
{
  m_realism->setValue(config.realism());
  m_speed->setValue(config.speed());
  m_level->setValue(config.level());
}

QLayout*
SpinPuzzleConfigurationWidget::create_level_section()
{
  // https://stackoverflow.com/questions/27661877/qt-slider-widget-with-tick-text-labels
  m_level->setRange(1, 10);
  m_level->setSingleStep(1);
  m_level->setTickPosition(QSlider::TickPosition::TicksBelow);
  m_level->setTickInterval(1);
  QLabel* level_label1 = new QLabel("Novice", this);
  QLabel* level_label2 = new QLabel("Intermediate", this);
  QLabel* level_label3 = new QLabel("Advanced", this);
  QLabel* level_label4 = new QLabel("Expert", this);
  QGridLayout* level_layout = new QGridLayout;
  level_layout->addWidget(m_level, 0, 0, 1, 4);
  level_layout->addWidget(level_label1, 1, 0, 1, 1);
  level_layout->addWidget(level_label2, 1, 1, 1, 1);
  level_layout->addWidget(level_label3, 1, 2, 1, 1);
  level_layout->addWidget(level_label4, 1, 3, 1, 1);

  return level_layout;
}

QLayout*
SpinPuzzleConfigurationWidget::create_speed_section()
{
  m_speed->setRange(5, 20);
  m_speed->setSingleStep(0.1);
  m_speed->setTickPosition(QSlider::TickPosition::TicksBelow);
  m_speed->setTickInterval(1);
  QLabel* speed_label1 = new QLabel("Slow", this);
  QLabel* speed_label2 = new QLabel("Moderate", this);
  QLabel* speed_label3 = new QLabel("Fast", this);
  QGridLayout* speed_layout = new QGridLayout;
  speed_layout->addWidget(m_speed, 0, 0, 1, 3);
  speed_layout->addWidget(speed_label1, 1, 0, 1, 1);
  speed_layout->addWidget(speed_label2, 1, 1, 1, 1);
  speed_layout->addWidget(speed_label3, 1, 2, 1, 1);
  return speed_layout;
}

QLayout*
SpinPuzzleConfigurationWidget::create_realism_section()
{

  m_realism->setRange(1, puzzle::SpinPuzzleSide<>::STEP / 2 - 1);
  m_realism->setSingleStep(1);
  m_realism->setTickPosition(QSlider::TickPosition::TicksBelow);
  m_realism->setTickInterval(1);
  QLabel* realism_label3 = new QLabel("Fast Game", this);
  QLabel* realism_label2 = new QLabel("Normal", this);
  QLabel* realism_label1 = new QLabel("Realistic", this);
  QGridLayout* realism_layout = new QGridLayout;
  realism_layout->addWidget(m_realism, 0, 0, 1, 3);
  realism_layout->addWidget(realism_label1, 1, 0, 1, 1);
  realism_layout->addWidget(realism_label2, 1, 1, 1, 1);
  realism_layout->addWidget(realism_label3, 1, 2, 1, 1);
  return realism_layout;
}