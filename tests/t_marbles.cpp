#include <gtest/gtest.h>

#include "puzzle/spin_marble.h"

#include <array>

// Test if with or without Qt the result is the same.
TEST(PuzzleMarbles, Default) {
  puzzle::SpinMarble marble;
  ASSERT_EQ(marble.color(), puzzle::SpinMarble::INVALID_COLOR);
  ASSERT_EQ(marble.id(), puzzle::SpinMarble::INVALID_ID);
  ASSERT_FALSE(marble.is_valid());

  puzzle::Color c1(puzzle::black);
  puzzle::Color c2(puzzle::red);

  ASSERT_NE(c1, c2);
}

TEST(PuzzleMarbles, Assign) {
  int id = 10;
  puzzle::SpinMarble marble(id, puzzle::black);
  auto copy = std::move(marble);

  // moved-from object should not be used!
  // ASSERT_EQ(marble.id(), puzzle::SpinMarble::INVALID_ID);
  // ASSERT_EQ(marble.color(), puzzle::SpinMarble::INVALID_COLOR);
  ASSERT_EQ(copy.id(), id);
  ASSERT_EQ(copy.color(), puzzle::black);
  ASSERT_TRUE(copy != marble);
  ASSERT_EQ(copy.to_string(), "(10:2)");
}

/**
 * @brief  test operation with array of marbles
 */
TEST(PuzzleMarbles, Array) {
  std::array<puzzle::SpinMarble, 20> marbles;
  std::array<puzzle::SpinMarble, 20> tmp;

  int id = 0;
  // initialize
  for (auto &m : marbles) {
    m = puzzle::SpinMarble(id, puzzle::black);
    ++id;
  }
  // revert order using tmp array
  for (size_t n = 0; n < marbles.size(); ++n) {
    std::swap(marbles[n], tmp[marbles.size() - n - 1]);
  }
  // check original array is in an invalid state
  for (auto &m : marbles) {
    ASSERT_EQ(m, puzzle::SpinMarble());
  }
  // check tmp array
  for (auto &m : tmp) {
    ASSERT_EQ(m, puzzle::SpinMarble(--id, puzzle::black));
  }
  // swap tmp and marbles and check it
  std::swap(tmp, marbles);
  id = marbles.size();
  for (auto &m : marbles) {
    ASSERT_EQ(m, puzzle::SpinMarble(--id, puzzle::black));
  }
}
