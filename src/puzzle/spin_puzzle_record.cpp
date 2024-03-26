#include "spin_puzzle_record.h"

namespace puzzle {
SpinPuzzleRecord::SpinPuzzleRecord(const std::string name,
                                   int time,
                                   int level,
                                   const puzzle::SpinPuzzleGame& game)
  : m_username(name)
  , m_time(time)
  , m_difficult_level(level)
  , m_game(game)
{
}

std::string
SpinPuzzleRecord::username() const
{
  return m_username;
}
int
SpinPuzzleRecord::time() const
{
  return m_time;
}
int
SpinPuzzleRecord::level() const
{
  return m_difficult_level;
}
const SpinPuzzleGame&
SpinPuzzleRecord::game() const
{
  return m_game;
}

void
SpinPuzzleRecord::update_time(int time)
{
    m_time = time;
}

bool
SpinPuzzleRecord::serialize(std::ofstream& out)
{
  out << "record\n";
  out << username() << "\n";
  out << time() << "\n";
  out << level() << "\n";
  m_game.serialize(out);
  return true;
}

bool
SpinPuzzleRecord::serialize(std::stringstream& out)
{
  out << "record\n";
  out << username() << "\n";
  out << time() << "\n";
  out << level() << "\n";
  m_game.serialize(out);
  return true;
}

bool
SpinPuzzleRecord::load(std::ifstream& in)
{
  // look for start of record
  while(!in.eof()) {
    std::string buffer;
    std::getline(in, buffer);
    if (buffer == "record") {
      break;
    }
  }
  std::string buffer_time;
  std::string buffer_level;
  std::getline(in, m_username);
  std::getline(in, buffer_time);
  std::getline(in, buffer_level);
  m_time = std::atoi(buffer_time.c_str());
  m_difficult_level = std::atoi(buffer_level.c_str());
  m_game.load(in);
  return true;
}

bool
SpinPuzzleRecord::load(std::stringstream& in)
{
  // look for start of record
  while(!in.eof()) {
    std::string buffer;
    std::getline(in, buffer);
    if (buffer == "record") {
      break;
    }
  }
  std::string buffer_time;
  std::string buffer_level;
  std::getline(in, m_username);
  std::getline(in, buffer_time);
  std::getline(in, buffer_level);
  m_time = std::atoi(buffer_time.c_str());
  m_difficult_level = std::atoi(buffer_level.c_str());
  m_game.load(in);
  return true;
}

}
