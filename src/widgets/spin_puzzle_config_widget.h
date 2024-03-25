
#ifndef SPIN_PUZZLE_CONFIG_WIDGET_H
#define SPIN_PUZZLE_CONFIG_WIDGET_H

#include <QWidget>

#include <QLineEdit>
#include <QSlider>

#include "puzzle/spin_configuration.h"
#include "spin_puzzle_widget.h"

class SpinPuzzleWidget;

/**
 * @brief  Class to display/modify configuration of the game
 */
class SpinPuzzleConfigurationWidget : public QWidget
{
  Q_OBJECT
public:
  SpinPuzzleConfigurationWidget(int win_width,
                                int win_heigth,
                                SpinPuzzleWidget* parent,
                                puzzle::Configuration& config);

private:
  QLayout* create_level_section();
  QLayout* create_speed_section();
  QLayout* create_realism_section();
  void set_config_values(const puzzle::Configuration& config);

  QLineEdit* m_nameLineEdit;
  QSlider* m_realism;
  QSlider* m_speed;
  QSlider* m_level;
  puzzle::Configuration m_config;

  SpinPuzzleWidget* m_parent;
};

#endif // SPIN_PUZZLE_CONFIG_WIDGET_H