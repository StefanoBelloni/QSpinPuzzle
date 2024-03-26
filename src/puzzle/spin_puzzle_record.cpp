#include "spin_puzzle_record.h"
#include <algorithm>

namespace {
// https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
// trim from start (in place)
inline void
ltrim(std::string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
}

// trim from end (in place)
inline void
rtrim(std::string& s)
{
  s.erase(std::find_if(s.rbegin(),
                       s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
            .base(),
          s.end());
}
}

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

void
SpinPuzzleRecord::update_username(const std::string& username)
{
  m_username = username;
}

bool
SpinPuzzleRecord::serialize(std::ofstream& out)
{
  out << "spin_puzzle_single_record\n";
  out << username() << "\n";
  out << time() << "\n";
  out << level() << "\n";
  m_game.serialize(out);
  return true;
}

bool
SpinPuzzleRecord::serialize(std::stringstream& out)
{
  out << "spin_puzzle_single_record\n";
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
  bool found = false;
  while (!in.eof()) {
    std::string buffer;
    std::getline(in, buffer);
    rtrim(buffer);
    ltrim(buffer);
    if (buffer == "spin_puzzle_single_record") {
      found = true;
      break;
    }
  }
  if (!found) {
    return false;
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
  bool found = false;
  while (!in.eof()) {
    std::string buffer;
    std::getline(in, buffer);
    rtrim(buffer);
    ltrim(buffer);
    if (buffer == "spin_puzzle_single_record") {
      found = true;
      break;
    }
  }
  if (!found) {
    return false;
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
