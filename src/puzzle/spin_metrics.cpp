#include "spin_metrics.h"
#include <limits>

namespace puzzle {

double
MetricProvider::naive_disorder(const puzzle::SpinPuzzleGame& game)
{
  double disorder = 0.0;
  auto current_step = game.current_time_step();
  using colors_occupation = std::array<int8_t, 6>;
  std::array<colors_occupation, 6> treefoils;
  for (size_t treefoil = 0; treefoil < 6; ++treefoil) {
    auto& colors = treefoils[treefoil];
    for (auto& color : colors) {
      color = 0;
    }
  }
  auto colorToIndex = [](puzzle::Color color) {
    switch (color) {
      case puzzle::blue:
        return 0;
      case puzzle::green:
        return 1;
      case puzzle::magenta:
        return 2;
      case puzzle::cyan:
        return 3;
      case puzzle::red:
        return 4;
      case puzzle::yellow:
      default:
        return 5;
    }
  };
  auto indexToColor = [](size_t index) {
    switch (index) {
      case 0:
        return puzzle::blue;
      case 1:
        return puzzle::green;
      case 2:
        return puzzle::magenta;
      case 3:
        return puzzle::cyan;
      case 4:
        return puzzle::red;
      default:
        return puzzle::yellow;
    }
  };
  for (size_t treefoil = 0; treefoil < 6; ++treefoil) {
    auto& colors = treefoils[treefoil];
    size_t start = 1 + treefoil * 13 - (treefoil >= 3);
    for (size_t index = 0; index < 10; ++index) {
      ++colors[colorToIndex(current_step[start + index])];
    }
  }

#if 0
  std::cout << "treefoils:\n";
  for (size_t treefoil = 0; treefoil < 6; ++treefoil) {
    std::cout << "T-" << treefoil + 1 << ": ";
    auto& colors = treefoils[treefoil];
    size_t start = 1 + treefoil * 13;
    for (auto& color : colors) {
      std::cout << static_cast<int>(color) << ", ";
    }
    std::cout << "\n";
  }
  std::cout << "\n";
#endif

  std::array<int8_t, 6> max_colors;
  max_colors.fill(0);
  for (size_t treefoil = 0; treefoil < 6; ++treefoil) {
    for (size_t colors = 0; colors < 6; ++colors) {
      max_colors[colors] =
        std::max(treefoils[treefoil][colors], max_colors[colors]);
    }
  }
  for (size_t colors = 0; colors < 6; ++colors) {
    disorder += max_colors[colors];
  }

#if 0
  std::cout << "max:\n";
  for (size_t colors = 0; colors < 6; ++colors) {
    disorder += max_colors[colors];
    std::cout << static_cast<int>(max_colors[colors]) << ", ";
  }
  std::cout << "\n";
#endif

  return disorder / 60.0;
}

}
