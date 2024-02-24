#include "spin_marble.h"

#include <iostream>

namespace puzzle {

SpinMarble::SpinMarble(int32_t id, QColor color) : m_id(id), m_color(color) {}

QColor SpinMarble::INVALID_COLOR = QColor();

} // namespace puzzle
