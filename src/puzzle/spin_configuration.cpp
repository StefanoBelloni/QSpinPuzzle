#include "spin_configuration.h"

namespace puzzle {

void
Configuration::set_name(const std::string name)
{
  m_name = name;
}
void
Configuration::set_level(int level)
{
  m_level = level;
}
void
Configuration::set_speed(double speed)
{
  m_speed = speed;
}
void
Configuration::set_realism(int realism)
{
  m_realism = realism;
}

std::string
Configuration::name() const
{
  return m_name;
}
int
Configuration::level() const
{
  return m_level;
}
double
Configuration::speed() const
{
  return m_speed;
}
int
Configuration::realism() const
{
  return m_realism;
}

void
Configuration::dump(std::stringstream& s)
{
  s << config_header << "\n";
  s << m_version << "\n";
  s << m_name << "\n";
  s << m_level << "\n";
  s << m_speed << "\n";
  s << m_realism << "\n";
}

bool
Configuration::load(std::stringstream& s)
{
  std::string buffer;
  std::getline(s, buffer);
  if (buffer != config_header) {
    return false;
  }
  std::getline(s, buffer);
  m_version = std::atoi(buffer.c_str());
  if (m_version != 1) {
    return false;
  }
  std::getline(s, m_name);
  // level
  std::getline(s, buffer);
  m_level = std::atoi(buffer.c_str());
  // speed
  std::getline(s, buffer);
  m_speed = std::atof(buffer.c_str());
  // realism
  std::getline(s, buffer);
  m_realism = std::atoi(buffer.c_str());
  return true;
}
}