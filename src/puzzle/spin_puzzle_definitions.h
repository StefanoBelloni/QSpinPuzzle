#ifndef SPIN_PUZZLE_DEFINITIONS_H
#define SPIN_PUZZLE_DEFINITIONS_H

#include <stdint.h>

#ifdef QT_CORE_LIB
#include <QColor>
#else

/**
 * @brief Mock of the namespace Qt and its constants.
 *
 * For compatibility with Qt if, Qt is not available for the library
 */
namespace Qt {
// colors
inline int black = 2; //!< see https://doc.qt.io/qt-6/qt.html
inline int red = 7;   //!< see https://doc.qt.io/qt-6/qt.html
inline int green = 8; //!< see https://doc.qt.io/qt-6/qt.html
// keys
inline int Key_T = 0x54; //!< see https://doc.qt.io/qt-6/qt.html
} // namespace Qt

/**
 * @brief The QColor class is a mock for the Qt version, in case is not
 * available
 */

class QColor {

public:
  QColor() : c(0) {}
  QColor(int n) : c(n) {}
  QColor(const QColor &c) : c(c.c) {}
  QColor(QColor &&c) : c(c.c) {}

  QColor &operator=(const QColor &other) {
    this->c = other.c;
    return *this;
  }
  QColor &operator=(QColor &&other) {
    this->c = other.c;
    return *this;
  }
  bool operator==(const QColor &other) const { return this->c == other.c; }
  bool operator!=(const QColor &other) const { return !(*this == other); }
  bool operator==(int other) const { return this->c == other; }
  bool operator!=(int other) const { return !(*this == other); }

private:
  int c;
};

#endif

namespace puzzle {

/**
 * @brief enumeration to determine the leaf of a trefoil
 */
enum class LEAF : uint8_t {
  NORTH = 0,   //!< leaf on the top (NORTH)
  EAST = 1,    //!< leaf on the right (EAST)
  WEST = 2,    //!< leaf on the left (WEST)
  TREFOIL = 3, //!< rotating around outer ring (border)
  CENTER = 4,  //!< central disk (to use a keybord)
  INVALID = 5, //!< INVALID state
};

/**
 * @brief  enumeration to determine if a rotation (any kind) is possible
 */
enum class ROTATION : uint8_t {
  OK = 0,     //!< rotation is possible
  INVALID = 1 //!< rotation is NOT possible
};

/**
 * @brief enumeration for the different state the mechanical parts of a Puzzle
 * can be.
 *
 * @note
 */
enum class TREFOIL : uint8_t {
  INVALID = 0,         //!< the central disk is rotating out of phase
  LEAF_ROTATION = 1,   //!< rotation of the leaf is possible
  LEAF_SPINNING = 2,   //!< the leaf is rotating on its axis
  BORDER_ROTATION = 3, //!< possible rotation around the outer ring is possible
};

/**
 * @brief enumeration to take into account time evolution or time related states
 */
enum class TIME : uint8_t {
  PREVIOUS = 0, //!< index of previous state
  CURRENT = 1   //!< index of current state
};

/**
 * @brief  enumeration to identify the side of a trefoil
 */
enum class SIDE : uint8_t {
  FRONT = 0, //!< front side of a trefoil
  BACK = 1,  //!< back side in a trefoil
};

} // namespace puzzle

#endif // SPIN_PUZZLE_DEFINITIONS_H
