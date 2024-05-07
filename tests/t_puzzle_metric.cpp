#include <gtest/gtest.h>

#include <sstream>

#include "puzzle/spin_metrics.h"
#include "puzzle/spin_puzzle_game.h"

using namespace puzzle;

TEST(PuzzleSide, naive_metrics)
{
  SpinPuzzleGame game;
  MetricProvider metric;

  ASSERT_DOUBLE_EQ(metric.naive_disorder(game), 1.0);
  game.shuffle();
  ASSERT_LT(metric.naive_disorder(game), 1.0);
}
