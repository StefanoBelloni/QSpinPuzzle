#ifndef SPIN_METRICS_H
#define SPIN_METRICS_H

#include "spin_puzzle_game.h"

namespace puzzle {

class MetricProvider
{
public:
  MetricProvider() = default;

  double naive_disorder(const puzzle::SpinPuzzleGame& game);

private:
};
}

#endif // SPIN_METRICS_H