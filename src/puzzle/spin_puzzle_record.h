#ifndef SPIN_PUZZLE_RECORD_H
#define SPIN_PUZZLE_RECORD_H

#include "spin_puzzle_game.h"
#include <fstream>
#include <sstream>
#include <string>

namespace puzzle {

class SpinPuzzleRecord
{
public:
  SpinPuzzleRecord() = default;

  SpinPuzzleRecord(const std::string name,
                   int time,
                   int level,
                   const puzzle::SpinPuzzleGame& game);

  SpinPuzzleRecord(const SpinPuzzleRecord& record) = default;

  std::string username() const;
  int time() const;
  int level() const;
  const puzzle::SpinPuzzleGame& game() const;

  void update_time(int time);
  void update_username(const std::string& username);

  bool serialize(std::ofstream& out);
  bool serialize(std::stringstream& out);
  bool load(std::ifstream& in);
  bool load(std::stringstream& in);

private:
  std::string m_username = "";
  int m_time = 0;
  int m_difficult_level = -1;
  puzzle::SpinPuzzleGame m_game{};
};

}

#endif