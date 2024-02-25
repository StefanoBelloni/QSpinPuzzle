#include "spin_puzzle_definitions.h"
namespace puzzle {

std::string color_to_str(int color) {
  switch (color) {
  case puzzle::white:
    return "white";
  case puzzle::black:
    return "black";
  case puzzle::red:
    return "red";
  case puzzle::darkRed:
    return "darkRed";
  case puzzle::green:
    return "green";
  case puzzle::darkGreen:
    return "darkGreen";
  case puzzle::blue:
    return "blue";
  case puzzle::darkBlue:
    return "darkBlue";
  case puzzle::cyan:
    return "cyan";
  case puzzle::darkCyan:
    return "darkCyan";
  case puzzle::magenta:
    return "magenta";
  case puzzle::darkMagenta:
    return "darkMagenta";
  case puzzle::yellow:
    return "yellow";
  case puzzle::darkYellow:
    return "darkYellow";
  case puzzle::gray:
    return "gray";
  case puzzle::darkGray:
    return "darkGray";
  case puzzle::lightGray:
    return "lightGray";
  default:
    return "invalid";
    break;
  }
  return "invalid";
}
} // namespace puzzle
