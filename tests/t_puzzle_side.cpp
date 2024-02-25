#include <gtest/gtest.h>

#include "puzzle/spin_marble.h"
#include "puzzle/spin_puzzle_side.h"

#include <array>
#include <iostream>

template <std::size_t N>
auto getPuzzle() {
  std::array<puzzle::SpinMarble, N * 3> marbles;
  int id = 0;
  puzzle::Color color = puzzle::black;
  for (auto &m : marbles) {
    m = puzzle::SpinMarble(id++, color);
    if (id == N) {
      color = puzzle::red;
    } else if (id == 2 * N) {
      color = puzzle::green;
    }
  }
  return puzzle::SpinPuzzleSide<N>(std::move(marbles));
}

// clang-format off
template <std::size_t N>
void no_rotation(std::array<puzzle::SpinMarble, N> &north_expected,
                 std::array<puzzle::SpinMarble, N> &east_expected,
                 std::array<puzzle::SpinMarble, N> &west_expected) {
  for (size_t i = 0; i < N; ++i) {
    north_expected[i] = puzzle::SpinMarble(i, puzzle::black);
  }
  for (size_t i = 0; i < N; ++i) {
    east_expected[i] = puzzle::SpinMarble(N + i, puzzle::red);
  }
  for (size_t i = 0; i < N; ++i) {
    west_expected[i] = puzzle::SpinMarble(2 * N + i, puzzle::green);
  }
}

template <std::size_t N>
void rotate_north_east_west(std::array<puzzle::SpinMarble, N> &north_expected,
                            std::array<puzzle::SpinMarble, N> &east_expected,
                            std::array<puzzle::SpinMarble, N> &west_expected) {
  for (size_t i = 0; i < N - 3; ++i) {
    north_expected[i] = puzzle::SpinMarble(i, puzzle::black);
  }
  for (size_t i = 0; i < N - 3; ++i) {
    east_expected[i] = puzzle::SpinMarble(N + i, puzzle::red);
  }
  for (size_t i = 0; i < N - 3; ++i) {
    west_expected[i] = puzzle::SpinMarble(2 * N + i, puzzle::green);
  }
  for (size_t i = N - 3; i < N; ++i) {
    north_expected[i] = puzzle::SpinMarble(2 * N + i, puzzle::green);
  }
  for (size_t i = N - 3; i < N; ++i) {
    east_expected[i] = puzzle::SpinMarble(i, puzzle::black);
  }
  for (size_t i = N - 3; i < N; ++i) {
    west_expected[i] = puzzle::SpinMarble(N + i, puzzle::red);
  }
}

template <std::size_t N>
void rotate_north_west_east(std::array<puzzle::SpinMarble, N> &north_expected,
                            std::array<puzzle::SpinMarble, N> &west_expected,
                            std::array<puzzle::SpinMarble, N> &east_expected) {
  for (size_t i = 0; i < N - 3; ++i) {
    north_expected[i] = puzzle::SpinMarble(i, puzzle::black);
  }
  for (size_t i = 0; i < N - 3; ++i) {
    east_expected[i] = puzzle::SpinMarble(N + i, puzzle::red);
  }
  for (size_t i = 0; i < N - 3; ++i) {
    west_expected[i] = puzzle::SpinMarble(2 * N + i, puzzle::green);
  }
  // ==================================== //
  for (size_t i = N - 3; i < N; ++i) {
    north_expected[i] = puzzle::SpinMarble(N + i, puzzle::red);
  }
  for (size_t i = N - 3; i < N; ++i) {
    east_expected[i] = puzzle::SpinMarble(2 * N + i, puzzle::green);
  }
  for (size_t i = N - 3; i < N; ++i) {
    west_expected[i] = puzzle::SpinMarble(i, puzzle::black);
  }
}

template <std::size_t N>
void check_marbles(std::array<puzzle::SpinMarble, N> &north_expected,
                   std::array<puzzle::SpinMarble, N> &east_expected,
                   std::array<puzzle::SpinMarble, N> &west_expected,
                   puzzle::SpinPuzzleSide<N> &puzzle) {
  auto north_it = puzzle.begin(puzzle::LEAF::NORTH);
  auto east_it = puzzle.begin(puzzle::LEAF::EAST);
  auto west_it = puzzle.begin(puzzle::LEAF::WEST);
#if 0
  std::cout << "=============================================================\n";
  std::cout << "|                  CHECK MARBLES                            |\n";
  std::cout << "=============================================================\n";
  std::cout << "FOUND <=> EXPECTED\n";
  for (size_t i = 0; i < N; ++i) {
    std::cout << "NORTH: " << (north_it + i)->id() << " <=> " <<  north_expected[i].id() << "\t";
    std::cout << "EAST: " << (east_it + i)->id() << " <=> " <<  east_expected[i].id() << "\t";
    std::cout << "WEST: " << (west_it + i)->id() << " <=> " <<  west_expected[i].id() << "\n";
  }
#endif
  for (size_t i = 0; i < N; ++i) {
    ASSERT_EQ(*(north_it + i), north_expected[i]);
    ASSERT_EQ(*(east_it + i), east_expected[i]);
    ASSERT_EQ(*(west_it + i), west_expected[i]);
  }
}

template <std::size_t N>
void check_marbles_border(std::array<puzzle::SpinMarble, 3 * N>& expected, puzzle::SpinPuzzleSide<N>& puzzle) {
  auto exp_it = expected.begin();
  auto mrb_it = puzzle.begin();
#if 0
  std::cout << "=============================================================\n";
  std::cout << "|                  CHECK MARBLES                            |\n";
  std::cout << "=============================================================\n";
  for (size_t i = 0; i < 3* N; ++i) {
    if ((i%10) == 0) {
      std::cout << "------------------------------------------\n";
    }
    std::cout << "EXPECTED: " << (exp_it + i)->id() << " <=> " <<  (mrb_it + i)->id() << " FOUND\n";
  }
#endif
  for (size_t i = 0; i < 3 * N; ++i) {
    ASSERT_EQ(*(exp_it + i), *(mrb_it + i));
  }
}

void start_border_east(std::array<puzzle::SpinMarble, 30>& marbles) {
  for (size_t i = 0; i < puzzle::SpinPuzzleSide<>::GROUP_SIZE - 3; ++i) {
    marbles[i] = puzzle::SpinMarble(i, puzzle::black);
  }
  for (size_t i = 0; i < puzzle::SpinPuzzleSide<>::GROUP_SIZE - 3; ++i) {
    marbles[10 + i] = puzzle::SpinMarble(10 + i, puzzle::red);
  }
  for (size_t i = 0; i < puzzle::SpinPuzzleSide<>::GROUP_SIZE - 3; ++i) {
    marbles[20 + i] = puzzle::SpinMarble(20 + i, puzzle::green);
  }
  // =========================================== //
  marbles[7] = puzzle::SpinMarble(9, puzzle::black);
  marbles[8] = puzzle::SpinMarble(8, puzzle::black);
  marbles[9] = puzzle::SpinMarble(7, puzzle::black);
  // =========================================== //
  marbles[17] = puzzle::SpinMarble(19, puzzle::red);
  marbles[18] = puzzle::SpinMarble(18, puzzle::red);
  marbles[19] = puzzle::SpinMarble(17, puzzle::red);
  // =========================================== //
  marbles[27] = puzzle::SpinMarble(29, puzzle::green);
  marbles[28] = puzzle::SpinMarble(28, puzzle::green);
  marbles[29] = puzzle::SpinMarble(27, puzzle::green);
}

void start_border_west(std::array<puzzle::SpinMarble, 30>& marbles) {
  for (size_t i = 0; i < puzzle::SpinPuzzleSide<>::GROUP_SIZE - 3; ++i) {
    marbles[i] = puzzle::SpinMarble(i, puzzle::black);
  }
  for (size_t i = 0; i < puzzle::SpinPuzzleSide<>::GROUP_SIZE - 3; ++i) {
    marbles[10 + i] = puzzle::SpinMarble(10 + i, puzzle::red);
  }
  for (size_t i = 0; i < puzzle::SpinPuzzleSide<>::GROUP_SIZE - 3; ++i) {
    marbles[20 + i] = puzzle::SpinMarble(20 + i, puzzle::green);
  }
  // =========================================== //
  marbles[7] = puzzle::SpinMarble(19, puzzle::red);
  marbles[8] = puzzle::SpinMarble(18, puzzle::red);
  marbles[9] = puzzle::SpinMarble(17, puzzle::red);
  // =========================================== //
  marbles[17] = puzzle::SpinMarble(29, puzzle::green);
  marbles[18] = puzzle::SpinMarble(28, puzzle::green);
  marbles[19] = puzzle::SpinMarble(27, puzzle::green);
  // =========================================== //
  marbles[27] = puzzle::SpinMarble(9, puzzle::black);
  marbles[28] = puzzle::SpinMarble(8, puzzle::black);
  marbles[29] = puzzle::SpinMarble(7, puzzle::black);
}

void start_configuration(std::array<puzzle::SpinMarble, 30>& marbles) {
  for (size_t i = 0; i < 10; ++i) {
    marbles[i] = puzzle::SpinMarble(i, puzzle::black);
  }
  for (size_t i = 0; i < 10; ++i) {
    marbles[10 + i] = puzzle::SpinMarble(10 + i, puzzle::red);
  }
  for (size_t i = 0; i < 10; ++i) {
    marbles[20 + i] = puzzle::SpinMarble(20 + i, puzzle::green);
  }
}
// clang-format on

// Test if with or without Qt the result is the same.
TEST(PuzzleSide, iterate_leaf) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  int id = 0;
  for (auto marbles_it = puzzle.marbles(puzzle::LEAF::NORTH); id <= 6 * N;
       ++marbles_it, ++id) {
    ASSERT_EQ(marbles_it->id(), id % N);
    ASSERT_EQ(marbles_it->color(), puzzle::black);
  }
  id = N;
  for (auto marbles_it = puzzle.marbles(puzzle::LEAF::EAST); id <= 6 * N;
       ++marbles_it, ++id) {
    ASSERT_EQ(marbles_it->id(), id % N + N);
    ASSERT_EQ(marbles_it->color(), puzzle::red);
  }
  id = 2 * N;
  for (auto marbles_it = puzzle.marbles(puzzle::LEAF::WEST); id <= 6 * N;
       ++marbles_it, ++id) {
    ASSERT_EQ(marbles_it->id(), id % N + 2 * N);
    ASSERT_EQ(marbles_it->color(), puzzle::green);
  }
}

TEST(PuzzleSide, rotate_leaf_cw_fraction_next_marble) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  // No rotation
  auto it_north_begin_0 = puzzle.begin(puzzle::LEAF::NORTH);
  ASSERT_EQ(it_north_begin_0.get_angle(), 0.0);
  // theta_1 = 9°
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, 360.0 / N / 4));
  auto it_north = puzzle.marbles(puzzle::LEAF::NORTH);
  auto it_north_begin_1 = puzzle.begin(puzzle::LEAF::NORTH);
  ASSERT_EQ(it_north_begin_1.get_angle(), 360.0 / N / 4);
  ASSERT_EQ(it_north->id(), it_north_begin_1->id());
  ASSERT_EQ(*it_north, *it_north_begin_1);

  // theta_2 = 18° => theta_tot = 27°
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, 2 * 360.0 / N / 4));
  // theta_tot = 27° > 36° = end of interval of first marble.
  // marble n.9 is now the first marble
  auto it_north_begin_2 = puzzle.begin(puzzle::LEAF::NORTH);
  ASSERT_EQ(puzzle.get_phase_shift_leaf(puzzle::LEAF::NORTH), 27);
  ASSERT_EQ(it_north_begin_2->id(), 9);
  ASSERT_EQ((it_north - 1)->id(), it_north_begin_2->id());
  ASSERT_EQ(*(it_north - 1), *it_north_begin_2);
  // the marble n.9 is now 9° before it middle point (36° - 27°)
  ASSERT_EQ(it_north_begin_2.get_angle(), -9);
}

TEST(PuzzleSide, rotate_leaf_cw_next_marble) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  // No rotation
  auto it_north_begin_0 = puzzle.begin(puzzle::LEAF::NORTH);
  ASSERT_EQ(it_north_begin_0.get_angle(), 0.0);
  // theta_1 = 18° => so already the next marble
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, 360.0 / N / 2));
  auto it_north = puzzle.marbles(puzzle::LEAF::NORTH);
  auto it_north_begin_1 = puzzle.begin(puzzle::LEAF::NORTH);
  ASSERT_EQ(it_north_begin_1.get_angle(), -360.0 / N / 2);
  ASSERT_EQ((it_north - 1)->id(), it_north_begin_1->id());
  ASSERT_EQ(*(it_north - 1), *it_north_begin_1);

  // theta_2 = 18° => theta_tot = 36°
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, 360.0 / N / 2));
  // theta_tot = 27° = 36° = end of interval of first marble.
  // marble n.9 is now the first marble at its baricenter
  auto it_north_begin_2 = puzzle.begin(puzzle::LEAF::NORTH);
  ASSERT_EQ(puzzle.get_phase_shift_leaf(puzzle::LEAF::NORTH), 36);
  ASSERT_EQ(it_north_begin_2->id(), 9);
  ASSERT_EQ((it_north - 1)->id(), it_north_begin_2->id());
  ASSERT_EQ(*(it_north - 1), *it_north_begin_2);
  // at its baricenter
  ASSERT_EQ(it_north_begin_2.get_angle(), 0);
}

TEST(PuzzleSide, rotate_leaf_cw_multiple_next_marbles) {
  constexpr int N = 10;
  const int angle = 20;
  for (int k = 1; k < 2 * N; ++k) {
    auto puzzle = getPuzzle<N>();
    // rotate 20° after the marble k
    ASSERT_TRUE(
        puzzle.rotate_marbles(puzzle::LEAF::NORTH, (k - 1) * 36 + angle));
    auto it_north = puzzle.marbles(puzzle::LEAF::NORTH);
    auto it_north_begin_1 = puzzle.begin(puzzle::LEAF::NORTH);
    // the marble is (36 - 20)° before its baricenter
    ASSERT_EQ(fmod(it_north_begin_1.get_angle() + 360.0, 360.0),
              fmod(angle - 36 + 360.0, 360.0));
    ASSERT_EQ((it_north - k)->id(), it_north_begin_1->id());
    ASSERT_EQ(*(it_north - k), *it_north_begin_1);
  }
}

TEST(PuzzleSide, rotate_leaf_acw_fraction_next_marble) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  // No rotation
  auto it_north_begin_0 = puzzle.begin(puzzle::LEAF::NORTH);
  ASSERT_EQ(it_north_begin_0.get_angle(), 0.0);
  // theta_1 = -9°
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, -360.0 / N / 4));
  auto it_north = puzzle.marbles(puzzle::LEAF::NORTH);
  auto it_north_begin_1 = puzzle.begin(puzzle::LEAF::NORTH);
  ASSERT_EQ(it_north_begin_1.get_angle(), 360.0 - 360.0 / N / 4);
  ASSERT_EQ(it_north->id(), it_north_begin_1->id());
  ASSERT_EQ(*it_north, *it_north_begin_1);

  // theta_2 = -18° => theta_tot = -27°
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, -2 * 360.0 / N / 4));
  // theta_tot = -27° < -36° = end of interval of first marble.
  // marble n.1 is now the first marble
  auto it_north_begin_2 = puzzle.begin(puzzle::LEAF::NORTH);
  ASSERT_EQ(puzzle.get_phase_shift_leaf(puzzle::LEAF::NORTH), 360 - 27);
  ASSERT_EQ(it_north_begin_2->id(), 1);
  ASSERT_EQ((it_north + 1)->id(), it_north_begin_2->id());
  ASSERT_EQ(*(it_north + 1), *it_north_begin_2);
  // the marble n.1 is now 9° ahead it middle point (-36° + 27°)
  ASSERT_EQ(it_north_begin_2.get_angle(), 9);
}

TEST(PuzzleSide, rotate_leaf_acw_next_marble) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  // No rotation
  auto it_north_begin_0 = puzzle.begin(puzzle::LEAF::NORTH);
  ASSERT_EQ(it_north_begin_0.get_angle(), 0.0);
  // theta_1 = 18° => so already the next marble
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, -360.0 / N / 2));
  auto it_north = puzzle.marbles(puzzle::LEAF::NORTH);
  auto it_north_begin_1 = puzzle.begin(puzzle::LEAF::NORTH);
  ASSERT_EQ(it_north_begin_1.get_angle(), 360.0 - 360.0 / N / 2);
  ASSERT_EQ((it_north)->id(), it_north_begin_1->id());
  ASSERT_EQ(*(it_north), *it_north_begin_1);

  // theta_2 = 18° => theta_tot = 36°
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, -360.0 / N / 2));
  // theta_tot = 27° = 36° = end of interval of first marble.
  // marble n.9 is now the first marble at its baricenter
  auto it_north_begin_2 = puzzle.begin(puzzle::LEAF::NORTH);
  ASSERT_EQ(puzzle.get_phase_shift_leaf(puzzle::LEAF::NORTH), 360 - 36);
  ASSERT_EQ(it_north_begin_2->id(), 1);
  ASSERT_EQ((it_north + 1)->id(), it_north_begin_2->id());
  ASSERT_EQ(*(it_north + 1), *it_north_begin_2);
  // at its baricenter
  ASSERT_EQ(it_north_begin_2.get_angle(), 0);
}

TEST(PuzzleSide, rotate_leaf_acw_multiple_next_marbles) {
  constexpr int N = 10;
  const int angle = 20;
  for (int k = 1; k < 2 * N; ++k) {
    auto puzzle = getPuzzle<N>();
    // rotate 20° after the marble k
    ASSERT_TRUE(
        puzzle.rotate_marbles(puzzle::LEAF::NORTH, -(k - 1) * 36 - angle));
    auto it_north = puzzle.marbles(puzzle::LEAF::NORTH);
    auto it_north_begin_1 = puzzle.begin(puzzle::LEAF::NORTH);
    // the marble is (36 - 20)° before its baricenter
    ASSERT_EQ(fmod(it_north_begin_1.get_angle() + 360.0, 360.0),
              fmod(-angle + 36 + 360.0, 360.0));
    ASSERT_EQ((it_north + k)->id(), it_north_begin_1->id());
    ASSERT_EQ(*(it_north + k), *it_north_begin_1);
  }
}

//!< Check spinning block aother operations
TEST(PuzzleSide, start_end_spinning_leaf) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  ASSERT_TRUE(puzzle.start_spinning_leaf(puzzle::LEAF::NORTH));
  ASSERT_FALSE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, 10));
  ASSERT_FALSE(puzzle.rotate_marbles(puzzle::LEAF::EAST, 10));
  ASSERT_FALSE(puzzle.rotate_marbles(puzzle::LEAF::WEST, 10));
  ASSERT_TRUE(puzzle.end_spinning_leaf());
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, 10));
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::EAST, 10));
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::WEST, 10));
}

//!< Check spinning block aother operations
TEST(PuzzleSide, start_spinning_leaf_not_allowed) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, 18));
  ASSERT_FALSE(puzzle.end_spinning_leaf());
  ASSERT_FALSE(puzzle.is_leaf_spinning_possible(puzzle::LEAF::NORTH));
  ASSERT_TRUE(puzzle.is_leaf_spinning_possible(puzzle::LEAF::WEST));
  ASSERT_TRUE(puzzle.is_leaf_spinning_possible(puzzle::LEAF::EAST));
  ASSERT_FALSE(puzzle.start_spinning_leaf(puzzle::LEAF::NORTH));
  ASSERT_TRUE(puzzle.get_trifoild_status() == puzzle::TREFOIL::LEAF_ROTATION);
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, 18));
  ASSERT_TRUE(puzzle.start_spinning_leaf(puzzle::LEAF::NORTH));
  ASSERT_TRUE(puzzle.get_trifoild_status() == puzzle::TREFOIL::LEAF_SPINNING);
  ASSERT_TRUE(puzzle.end_spinning_leaf());
  ASSERT_TRUE(puzzle.get_trifoild_status() == puzzle::TREFOIL::LEAF_ROTATION);
}

TEST(PuzzleSide, central_disk_spinning_not_possible) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, 18));
  ASSERT_FALSE(puzzle.rotate_internal_disk(60));
}

TEST(PuzzleSide, central_disk_spinning_east) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  ASSERT_TRUE(puzzle.rotate_internal_disk(60));
  ASSERT_TRUE(puzzle.rotate_internal_disk(60));
  // rotation NORTH->EAST->WEST
  std::array<puzzle::SpinMarble, N> north_expected;
  std::array<puzzle::SpinMarble, N> east_expected;
  std::array<puzzle::SpinMarble, N> west_expected;
  rotate_north_east_west(north_expected, east_expected, west_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);
  ASSERT_FLOAT_EQ(0.0, puzzle.get_phase_shift_internal_disk());
}

TEST(PuzzleSide, central_disk_spinning_east_till_west) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  ASSERT_TRUE(puzzle.rotate_internal_disk(60));
  ASSERT_TRUE(puzzle.rotate_internal_disk(179));
  // rotation NORTH->EAST->WEST
  std::array<puzzle::SpinMarble, N> north_expected;
  std::array<puzzle::SpinMarble, N> east_expected;
  std::array<puzzle::SpinMarble, N> west_expected;
  rotate_north_west_east(north_expected, west_expected, east_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);
  ASSERT_FLOAT_EQ(-1.0, puzzle.get_phase_shift_internal_disk());
}

TEST(PuzzleSide, central_disk_spinning_west) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  ASSERT_TRUE(puzzle.rotate_internal_disk(-60));
  ASSERT_TRUE(puzzle.rotate_internal_disk(-60));
  // rotation NORTH->EAST->WEST
  std::array<puzzle::SpinMarble, N> north_expected;
  std::array<puzzle::SpinMarble, N> east_expected;
  std::array<puzzle::SpinMarble, N> west_expected;
  rotate_north_west_east(north_expected, west_expected, east_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);
  ASSERT_FLOAT_EQ(0.0, puzzle.get_phase_shift_internal_disk());
}

TEST(PuzzleSide, central_disk_spinning_west_till_east) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  ASSERT_TRUE(puzzle.rotate_internal_disk(-60));
  ASSERT_TRUE(puzzle.rotate_internal_disk(-181));
  // rotation NORTH->EAST->WEST
  std::array<puzzle::SpinMarble, N> north_expected;
  std::array<puzzle::SpinMarble, N> east_expected;
  std::array<puzzle::SpinMarble, N> west_expected;
  rotate_north_east_west(north_expected, east_expected, west_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);
  ASSERT_FLOAT_EQ(-1.0, puzzle.get_phase_shift_internal_disk());
}

TEST(PuzzleSide, central_disk_spinning_east_and_back) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  // rotation NORTH->EAST->WEST
  std::array<puzzle::SpinMarble, N> north_expected;
  std::array<puzzle::SpinMarble, N> east_expected;
  std::array<puzzle::SpinMarble, N> west_expected;

  no_rotation(north_expected, east_expected, west_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);

  ASSERT_TRUE(puzzle.rotate_internal_disk(118));
  rotate_north_east_west(north_expected, east_expected, west_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);

  ASSERT_TRUE(puzzle.rotate_internal_disk(118));
  rotate_north_west_east(north_expected, west_expected, east_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);

  ASSERT_TRUE(puzzle.rotate_internal_disk(-120));
  rotate_north_east_west(north_expected, east_expected, west_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);

  ASSERT_TRUE(puzzle.rotate_internal_disk(-120));
  no_rotation(north_expected, east_expected, west_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);

  ASSERT_FLOAT_EQ(-4.0, puzzle.get_phase_shift_internal_disk());
}

TEST(PuzzleSide, central_disk_spinning_west_and_back) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  // rotation NORTH->EAST->WEST
  std::array<puzzle::SpinMarble, N> north_expected;
  std::array<puzzle::SpinMarble, N> east_expected;
  std::array<puzzle::SpinMarble, N> west_expected;

  no_rotation(north_expected, east_expected, west_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);

  ASSERT_TRUE(puzzle.rotate_internal_disk(-118));
  rotate_north_west_east(north_expected, west_expected, east_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);

  ASSERT_TRUE(puzzle.rotate_internal_disk(-118));
  rotate_north_east_west(north_expected, east_expected, west_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);

  ASSERT_TRUE(puzzle.rotate_internal_disk(+120));
  rotate_north_west_east(north_expected, west_expected, east_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);

  ASSERT_TRUE(puzzle.rotate_internal_disk(+120));
  no_rotation(north_expected, east_expected, west_expected);
  check_marbles(north_expected, east_expected, west_expected, puzzle);

  ASSERT_FLOAT_EQ(4.0, puzzle.get_phase_shift_internal_disk());
}

TEST(PuzzleSide, start_border_rotation_east_side) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  ASSERT_TRUE(puzzle.rotate_internal_disk(60.0));
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, 10.0));
  std::array<puzzle::SpinMarble, 3 * N> expected;
  start_border_east(expected);
  check_marbles_border(expected, puzzle);

  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::EAST, -10.0));
  ASSERT_TRUE(puzzle.rotate_internal_disk(-60.0));
  start_configuration(expected);
  check_marbles_border(expected, puzzle);
}

TEST(PuzzleSide, start_border_rotation_west_side) {
  constexpr int N = 10;
  auto puzzle = getPuzzle<N>();
  ASSERT_TRUE(puzzle.rotate_internal_disk(-60.0));
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, 10.0));
  std::array<puzzle::SpinMarble, 3 * N> expected;
  start_border_west(expected);
  check_marbles_border(expected, puzzle);

  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::EAST, -10.0));
  ASSERT_TRUE(puzzle.rotate_internal_disk(+60.0));
  start_configuration(expected);
  check_marbles_border(expected, puzzle);
}

puzzle::SpinMarble MRB(int32_t id) {
  if (id < 10) {
    return puzzle::SpinMarble(id, puzzle::black);
  } else if (id < 20) {
    return puzzle::SpinMarble(id, puzzle::red);
  } else if (id < 30) {
    return puzzle::SpinMarble(id, puzzle::green);
  }
  return puzzle::SpinMarble();
}

TEST(PuzzleSide, basic_move_sequence_0) {
  using namespace puzzle;
  constexpr int N = 10;
  constexpr double DTHETA = 36.0;
  auto puzzle = getPuzzle<N>();
  std::array<puzzle::SpinMarble, 3 * 10> expected;
  std::array<puzzle::SpinMarble, N> north_expected;
  std::array<puzzle::SpinMarble, N> east_expected;
  std::array<puzzle::SpinMarble, N> west_expected;

  start_configuration(expected);
  check_marbles_border(expected, puzzle);

  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, 3 * DTHETA));
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::EAST, 4 * DTHETA));
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::WEST, 7 * DTHETA));

  north_expected = {
      SpinMarble{7, puzzle::black}, {8, puzzle::black}, {9, puzzle::black}, {0, puzzle::black},
      {1, puzzle::black},           {2, puzzle::black}, {3, puzzle::black}, {4, puzzle::black},
      {5, puzzle::black},           {6, puzzle::black},
  };
  east_expected = {
      SpinMarble{16, puzzle::red}, {17, puzzle::red}, {18, puzzle::red}, {19, puzzle::red},
      {10, puzzle::red},           {11, puzzle::red}, {12, puzzle::red}, {13, puzzle::red},
      {14, puzzle::red},           {15, puzzle::red},
  };
  west_expected = {
      SpinMarble{23, puzzle::green}, {24, puzzle::green}, {25, puzzle::green},
      {26, puzzle::green},           {27, puzzle::green}, {28, puzzle::green},
      {29, puzzle::green},           {20, puzzle::green}, {21, puzzle::green},
      {22, puzzle::green},
  };
  check_marbles(north_expected, east_expected, west_expected, puzzle);
  ASSERT_TRUE(puzzle.rotate_internal_disk(120));
  north_expected = {MRB(7), MRB(8), MRB(9),  MRB(0),  MRB(1),
                    MRB(2), MRB(3), MRB(20), MRB(21), MRB(22)};
  east_expected = {MRB(16), MRB(17), MRB(18), MRB(19), MRB(10),
                   MRB(11), MRB(12), MRB(4),  MRB(5),  MRB(6)};
  west_expected = {MRB(23), MRB(24), MRB(25), MRB(26), MRB(27),
                   MRB(28), MRB(29), MRB(13), MRB(14), MRB(15)};
  check_marbles(north_expected, east_expected, west_expected, puzzle);
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, 3 * DTHETA));
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::EAST, -4 * DTHETA));
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::WEST, -7 * DTHETA));
  north_expected = {MRB(20), MRB(21), MRB(22), MRB(7), MRB(8),
                    MRB(9),  MRB(0),  MRB(1),  MRB(2), MRB(3)};
  east_expected = {MRB(10), MRB(11), MRB(12), MRB(4),  MRB(5),
                   MRB(6),  MRB(16), MRB(17), MRB(18), MRB(19)};
  west_expected = {MRB(13), MRB(14), MRB(15), MRB(23), MRB(24),
                   MRB(25), MRB(26), MRB(27), MRB(28), MRB(29)};
  check_marbles(north_expected, east_expected, west_expected, puzzle);
  ASSERT_TRUE(puzzle.rotate_internal_disk(180));
  north_expected = {MRB(20), MRB(21), MRB(22), MRB(7),  MRB(8),
                    MRB(9),  MRB(0),  MRB(27), MRB(28), MRB(29)};
  east_expected = {MRB(10), MRB(11), MRB(12), MRB(4), MRB(5),
                   MRB(6),  MRB(16), MRB(1),  MRB(2), MRB(3)};
  west_expected = {MRB(13), MRB(14), MRB(15), MRB(23), MRB(24),
                   MRB(25), MRB(26), MRB(17), MRB(18), MRB(19)};
  check_marbles(north_expected, east_expected, west_expected, puzzle);
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, DTHETA));
  expected = {MRB(17), MRB(20), MRB(21), MRB(22), MRB(7),  MRB(8),
              MRB(9),  MRB(0),  MRB(29), MRB(28), MRB(27), MRB(10),
              MRB(11), MRB(12), MRB(4),  MRB(5),  MRB(6),  MRB(16),
              MRB(3),  MRB(2),  MRB(1),  MRB(13), MRB(14), MRB(15),
              MRB(23), MRB(24), MRB(25), MRB(26), MRB(19), MRB(18)};
  check_marbles_border(expected, puzzle);
  ASSERT_TRUE(puzzle.rotate_marbles(puzzle::LEAF::NORTH, -10 * DTHETA));
  expected = {
      MRB(27), MRB(10), MRB(11), MRB(12), MRB(4),  MRB(5),  MRB(6),  MRB(16),
      MRB(3),  MRB(2),  MRB(1),  MRB(13), MRB(14), MRB(15), MRB(23), MRB(24),
      MRB(25), MRB(26), MRB(19), MRB(18), MRB(17), MRB(20), MRB(21), MRB(22),
      MRB(7),  MRB(8),  MRB(9),  MRB(0),  MRB(29), MRB(28),
  };
  check_marbles_border(expected, puzzle);
  ASSERT_TRUE(puzzle.rotate_internal_disk(-60));
  north_expected = {MRB(27), MRB(10), MRB(11), MRB(12), MRB(4),
                    MRB(5),  MRB(6),  MRB(2),  MRB(3),  MRB(16)};
  east_expected = {MRB(1),  MRB(13), MRB(14), MRB(15), MRB(23),
                   MRB(24), MRB(25), MRB(18), MRB(19), MRB(26)};
  west_expected = {MRB(17), MRB(20), MRB(21), MRB(22), MRB(7),
                   MRB(8),  MRB(9),  MRB(28), MRB(29), MRB(0)};
  check_marbles(north_expected, east_expected, west_expected, puzzle);
}