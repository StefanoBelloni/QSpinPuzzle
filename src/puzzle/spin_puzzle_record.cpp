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

std::string
SpinPuzzleRecord::encrypt(const char* prefix, char end_of_message)
{
  puzzle::Cipher cipher(m_version);
  std::stringstream s;
  s << "spinpuzzlegame " << static_cast<int>(m_version);
  std::stringstream game_s;
  serialize(game_s);
  std::string g = game_s.str();
  std::string out = cipher.encrypt(g);
  s << out << "|";
  return s.str();
}

bool
SpinPuzzleRecord::decrypt(std::string input,
                          const char* expected_prefix,
                          char end_of_message)
{
  std::stringstream s;
  std::string prefix;
  s << input;
  s >> prefix;
  bool error = false;
  if (prefix != expected_prefix) {
    return false;
  }
  int cipher_version;
  s >> cipher_version;
  error = static_cast<puzzle::Cipher::VERSION>(cipher_version) >=
          puzzle::Cipher::VERSION::INVALID;
  if (error) {
    return false;
  }
  puzzle::Cipher::VERSION v = puzzle::Cipher::VERSION(cipher_version);
  puzzle::Cipher cipher(v);
  std::string game_str;
  std::getline(s, game_str, '|');
  game_str = cipher.decrypt(game_str);
  std::stringstream s2;
  s2 << game_str;
  return load(s2);
}

}
