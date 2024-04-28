#include "spin_metrics.h"

namespace puzzle {

double
MetricProvider::naive_disorder(const puzzle::SpinPuzzleGame& game)
{
  auto current_step = game.current_time_step();
  return 0.0;
}

}
