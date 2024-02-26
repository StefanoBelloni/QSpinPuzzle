#include <gtest/gtest.h>

#include "puzzle/spin_puzzle_definitions.h"

#include <array>

// Test if with or without Qt the result is the same.
TEST(SpinPuzzleDefinition, Colors) {
  ASSERT_EQ(puzzle::color_to_str(puzzle::white), "white");
  ASSERT_EQ(puzzle::color_to_str(puzzle::black), "black");
  ASSERT_EQ(puzzle::color_to_str(puzzle::red), "red");
  ASSERT_EQ(puzzle::color_to_str(puzzle::darkRed), "darkRed");
  ASSERT_EQ(puzzle::color_to_str(puzzle::green), "green");
  ASSERT_EQ(puzzle::color_to_str(puzzle::darkGreen), "darkGreen");
  ASSERT_EQ(puzzle::color_to_str(puzzle::blue), "blue");
  ASSERT_EQ(puzzle::color_to_str(puzzle::darkBlue), "darkBlue");
  ASSERT_EQ(puzzle::color_to_str(puzzle::cyan), "cyan");
  ASSERT_EQ(puzzle::color_to_str(puzzle::darkCyan), "darkCyan");
  ASSERT_EQ(puzzle::color_to_str(puzzle::magenta), "magenta");
  ASSERT_EQ(puzzle::color_to_str(puzzle::darkMagenta), "darkMagenta");
  ASSERT_EQ(puzzle::color_to_str(puzzle::yellow), "yellow");
  ASSERT_EQ(puzzle::color_to_str(puzzle::darkYellow), "darkYellow");
  ASSERT_EQ(puzzle::color_to_str(puzzle::gray), "gray");
  ASSERT_EQ(puzzle::color_to_str(puzzle::darkGray), "darkGray");
  ASSERT_EQ(puzzle::color_to_str(puzzle::lightGray), "lightGray");
  ASSERT_EQ(puzzle::color_to_str(-1), "invalid");
}
