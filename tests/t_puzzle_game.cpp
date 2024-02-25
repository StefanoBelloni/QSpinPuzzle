#include <gtest/gtest.h>

#include "puzzle/spin_puzzle_game.h"
#include "puzzle/spin_puzzle_side.h"

using namespace puzzle;

TEST(PuzzleSide, game_creation) {

  SpinPuzzleGame game;

  ASSERT_EQ(game.get_active_side(), SIDE::FRONT);
  auto &front = game.get_side(SIDE::FRONT);
  auto &back = game.get_side(SIDE::BACK);
  ASSERT_EQ(front.begin()->id(), 0);
  ASSERT_EQ(back.begin()->id(), 30);

  ASSERT_EQ(*front.begin(LEAF::NORTH), SpinMarble(0, puzzle::blue));
  ASSERT_EQ(*front.begin(LEAF::EAST), SpinMarble(10, puzzle::green));
  ASSERT_EQ(*front.begin(LEAF::WEST), SpinMarble(20, puzzle::magenta));

  ASSERT_EQ(*back.begin(LEAF::NORTH), SpinMarble(30, puzzle::cyan));
  ASSERT_EQ(*back.begin(LEAF::EAST), SpinMarble(40, puzzle::red));
  ASSERT_EQ(*back.begin(LEAF::WEST), SpinMarble(50, puzzle::yellow));
}

TEST(PuzzleSide, game_spin_north) {

  SpinPuzzleGame game;
  game.spin_leaf(LEAF::NORTH);

  auto &front = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*front.marbles(LEAF::NORTH), SpinMarble(0, puzzle::blue));
  for (size_t n = 1; n <= 5; ++n) {
    ASSERT_EQ(*front.marbles(LEAF::NORTH, n), SpinMarble(30 + n, puzzle::cyan));
  }
  for (size_t n = 6; n < 10; ++n) {
    ASSERT_EQ(*front.marbles(LEAF::NORTH, n), SpinMarble(n, puzzle::blue));
  }

  // spin again
  game.spin_leaf(LEAF::NORTH);
  for (size_t n = 0; n < 10; ++n) {
    ASSERT_EQ(*front.marbles(LEAF::NORTH, n), SpinMarble(n, puzzle::blue));
  }
}

TEST(PuzzleSide, game_rotate_marbles) {
  SpinPuzzleGame game;
  double STEP = SpinPuzzleSide<>::STEP;
  game.rotate_marbles(LEAF::NORTH, 3 * STEP);
  game.rotate_marbles(LEAF::EAST, 4 * STEP);
  game.rotate_marbles(LEAF::WEST, 5 * STEP);
  game.swap_side();
  game.rotate_marbles(LEAF::NORTH, 3 * STEP);
  game.rotate_marbles(LEAF::EAST, 4 * STEP);
  game.rotate_marbles(LEAF::WEST, 5 * STEP);
  game.swap_side();
  auto &front = game.get_side(SIDE::FRONT);
  auto &back = game.get_side(SIDE::BACK);

  ASSERT_EQ(*front.begin(LEAF::NORTH), SpinMarble(10 - 3, puzzle::blue));
  ASSERT_EQ(*front.begin(LEAF::EAST), SpinMarble(20 - 4, puzzle::green));
  ASSERT_EQ(*front.begin(LEAF::WEST), SpinMarble(30 - 5, puzzle::magenta));
  ASSERT_EQ(*back.begin(LEAF::NORTH), SpinMarble(40 - 3, puzzle::cyan));
  ASSERT_EQ(*back.begin(LEAF::EAST), SpinMarble(50 - 4, puzzle::red));
  ASSERT_EQ(*back.begin(LEAF::WEST), SpinMarble(60 - 5, puzzle::yellow));

  game.spin_leaf(LEAF::NORTH);
  game.spin_leaf(LEAF::EAST);
  game.spin_leaf(LEAF::WEST);

  ASSERT_EQ(*front.begin(LEAF::NORTH), SpinMarble(10 - 3, puzzle::blue));
  ASSERT_EQ(*front.begin(LEAF::EAST), SpinMarble(20 - 4, puzzle::green));
  ASSERT_EQ(*front.begin(LEAF::WEST), SpinMarble(30 - 5, puzzle::magenta));
  ASSERT_EQ(*back.begin(LEAF::NORTH), SpinMarble(40 - 3, puzzle::cyan));
  ASSERT_EQ(*back.begin(LEAF::EAST), SpinMarble(50 - 4, puzzle::red));
  ASSERT_EQ(*back.begin(LEAF::WEST), SpinMarble(60 - 5, puzzle::yellow));

  ASSERT_EQ(*(front.begin(LEAF::NORTH) + 1), SpinMarble(40 - 3 + 1, puzzle::cyan));
  ASSERT_EQ(*(front.begin(LEAF::EAST) + 1), SpinMarble(60 - 5 + 1, puzzle::yellow));
  ASSERT_EQ(*(front.begin(LEAF::WEST) + 1), SpinMarble(50 - 4 + 1, puzzle::red));
  ASSERT_EQ(*(back.begin(LEAF::NORTH) + 1), SpinMarble(10 - 3 + 1, puzzle::blue));
  ASSERT_EQ(*(back.begin(LEAF::EAST) + 1), SpinMarble(30 - 5 + 1, puzzle::magenta));
  ASSERT_EQ(*(back.begin(LEAF::WEST) + 1), SpinMarble(20 - 4 + 1, puzzle::green));
}

TEST(PuzzleSide, game_reset_marbles) {
  SpinPuzzleGame game;
  double STEP = SpinPuzzleSide<>::STEP;
  game.rotate_marbles(LEAF::NORTH, 3 * STEP);
  game.rotate_marbles(LEAF::EAST, 4 * STEP);
  game.rotate_marbles(LEAF::WEST, 5 * STEP);

  auto &front = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*front.begin(LEAF::NORTH), SpinMarble(10 - 3, puzzle::blue));

  game.reset();
  auto &front_reset = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*front_reset.begin(LEAF::NORTH), SpinMarble(0, puzzle::blue));
}

TEST(PuzzleSide, game_spin_multistep_positive) {
  SpinPuzzleGame game;

  auto &front_0 = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*(front_0.begin(LEAF::NORTH) + 1), SpinMarble(1, puzzle::blue));

  game.spin_leaf(LEAF::NORTH, 45.0);
  auto &front_1 = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*(front_1.begin(LEAF::NORTH) + 1), SpinMarble(1, puzzle::blue));

  game.spin_leaf(LEAF::NORTH, 44.0);
  auto &front_2 = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*(front_2.begin(LEAF::NORTH) + 1), SpinMarble(1, puzzle::blue));

  game.spin_leaf(LEAF::NORTH, 90.0);
  auto &front_3 = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*(front_3.begin(LEAF::NORTH) + 1), SpinMarble(31, puzzle::cyan));

  game.spin_leaf(LEAF::NORTH, 100.0);
  auto &front_4 = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*(front_4.begin(LEAF::NORTH) + 1), SpinMarble(1, puzzle::blue));
}

TEST(PuzzleSide, game_spin_multistep_negative) {
  SpinPuzzleGame game;

  auto &front_0 = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*(front_0.begin(LEAF::NORTH) + 1), SpinMarble(1, puzzle::blue));

  game.spin_leaf(LEAF::NORTH, -45.0);
  auto &front_1 = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*(front_1.begin(LEAF::NORTH) + 1), SpinMarble(1, puzzle::blue));

  game.spin_leaf(LEAF::NORTH, -44.0);
  auto &front_2 = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*(front_2.begin(LEAF::NORTH) + 1), SpinMarble(1, puzzle::blue));

  game.spin_leaf(LEAF::NORTH, -90.0);
  auto &front_3 = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*(front_3.begin(LEAF::NORTH) + 1), SpinMarble(31, puzzle::cyan));

  game.spin_leaf(LEAF::NORTH, -100.0);
  auto &front_4 = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*(front_4.begin(LEAF::NORTH) + 1), SpinMarble(1, puzzle::blue));
}

TEST(PuzzleSide, game_rotate_internal_disk) {
  SpinPuzzleGame game;

  for (size_t n = 5; n <= 125; n += 5) {
    game.rotate_internal_disk(5);
  }
  auto &front = game.get_side(SIDE::FRONT);
  ASSERT_EQ(*(front.begin(LEAF::NORTH) + 9), SpinMarble(29, puzzle::magenta));
}

TEST(PuzzleSide, game_shuffle) {
  SpinPuzzleGame game;
  for (size_t n = 0; n < 1; ++n) {
    game.shuffle(0, 10000, true);
  }
}