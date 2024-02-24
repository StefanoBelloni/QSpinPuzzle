#ifndef SPINMARBLE_H
#define SPINMARBLE_H

#include "spin_puzzle_definitions.h"

namespace puzzle {

/**
 * @brief The SpinMarble class rappresent a single marble in the Spin Game
 *
 * It is identified by a number (integer) and a color (QColor):
 * This helps to debug and develop the code.
 *
 * The default constructor creates a marble with id '-1' and color '-1'
 */
class SpinMarble {
public:
  //! \brief invalid value
  static QColor INVALID_COLOR;
  static constexpr int32_t INVALID_ID = -1;
  /**
   * @brief  default constructor: it create a Marble in an invalid state.
   */
  SpinMarble() = default;
  /**
   * @brief  constructor with id and color
   *
   * @param  id: id to assign to this marble
   * @param  color: to assign to this marble
   */
  SpinMarble(int32_t id, QColor color);
  /**
   * @brief Move constructor: to avoid duplicate marbles
   *
   * @param  other: marble to be moved
   */
  SpinMarble(SpinMarble &&other) {
    m_id = other.m_id;
    m_color = other.m_color;
    other.m_id = INVALID_ID;
    other.m_color = INVALID_COLOR;
  }

  /**
   * @brief Move assignment
   *
   * @param  other: marbled to be swap
   */
  SpinMarble &operator=(SpinMarble &&other) {
    m_id = other.m_id;
    m_color = other.m_color;
    other.m_id = INVALID_ID;
    other.m_color = INVALID_COLOR;
    return *this;
  }

  /**
   * @brief  Copy contructor is deleted, to avoid duplicate of marbles
   *
   * @param  other:
   */
  SpinMarble(const SpinMarble &other) = delete;
  /**
   * @brief  assign operator is deleted, to avoid duplicate of marbles
   *
   * @param  other:
   */
  SpinMarble &operator=(const SpinMarble &other) = delete;

  //! \brief getter for the color
  QColor color() const { return m_color; }
  //! \brief getter for the id
  int32_t id() const { return m_id; }
  //! \brief sanity check
  bool is_valid() const { return m_color != INVALID_COLOR && m_id != -1; }

  bool operator==(const SpinMarble &other) const {
    return m_id == other.m_id && m_color == other.m_color;
  }

  bool operator!=(const SpinMarble &other) const { return !(*this == other); }

private:
  //! \brief id of the marble
  int32_t m_id = SpinMarble::INVALID_ID;
  //! \brief color of this marble
  QColor m_color = SpinMarble::INVALID_COLOR;
};

} // namespace puzzle

#endif // SPINMARBLE_H
