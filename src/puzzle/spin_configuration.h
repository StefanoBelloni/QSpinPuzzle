#ifndef SPIN_CONFIGURATION_H
#define SPIN_CONFIGURATION_H

#include <sstream>
#include <string>

namespace puzzle {

class Configuration
{

public:
  Configuration() = default;
  Configuration(const Configuration& config) = default;
  void set_name(const std::string name);
  void set_level(int level);
  void set_speed(double speed);
  void set_realism(int realism);

  std::string name() const;
  int level() const;
  double speed() const;
  int realism() const;

  void dump(std::stringstream& s);
  bool load(std::stringstream& s);

private:
  std::string config_header = "configuration QSpinPUzzle";
  std::string m_name = "QSpinPuzzle";
  int m_level = 5;
  double m_speed = 11.0;
  int m_realism = 5;
  int m_version = 1;
};

}

#endif