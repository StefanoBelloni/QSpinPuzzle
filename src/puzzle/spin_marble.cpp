#include "spin_marble.h"

#include <iostream>

namespace puzzle {

SpinMarble::SpinMarble(int32_t id, Color color)
  : m_id(id)
  , m_color(color)
{
}

Color SpinMarble::INVALID_COLOR = Color();

} // namespace puzzle
