#ifndef ACTIONPROVIDER_H
#define ACTIONPROVIDER_H

#include "spin_puzzle_definitions.h"
#include <vector>

class ActionProvider {
public:
  ActionProvider();

  std::vector<int> getSequenceOfCommands(int seed, int size);

  static constexpr int N = 8;

  int keys[N] = {
      puzzle::Key_N,      puzzle::Key_E,    puzzle::Key_W,     puzzle::Key_I,
      puzzle::Key_PageUp, puzzle::Key_Left, puzzle::Key_Right, puzzle::Key_P,
  };
};

#endif // ACTIONPROVIDER_H