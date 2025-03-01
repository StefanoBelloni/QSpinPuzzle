#ifndef SPIN_PUZZLE_DEFINITIONS_H
#define SPIN_PUZZLE_DEFINITIONS_H

#include <stdint.h>

#include <string>

/**
 * @brief Mock of the namespace Qt and its constants.
 *
 * For compatibility with Qt if, Qt is not available for the library
 */
namespace puzzle {

// keys
const inline int Key_NONE = 0x0;
const inline int Key_N = 0x4e;
const inline int Key_P = 0x50;
const inline int Key_E = 0x45;
const inline int Key_W = 0x57;
const inline int Key_Left = 0x01000012;
const inline int Key_Right = 0x01000014;
const inline int Key_PageUp = 0x01000016;
const inline int Key_PageDown = 0x01000017;
const inline int Key_I = 0x49;

// colors
const inline int white = 3;
const inline int black = 2;
const inline int red = 7;
const inline int darkRed = 13;
const inline int green = 8;
const inline int darkGreen = 14;
const inline int blue = 9;
const inline int darkBlue = 15;
const inline int cyan = 10;
const inline int darkCyan = 16;
const inline int magenta = 11;
const inline int darkMagenta = 17;
const inline int yellow = 12;
const inline int darkYellow = 18;
const inline int gray = 5;
const inline int darkGray = 4;
const inline int lightGray = 6;

// keys
inline int Key_T = 0x54; //!< see https://doc.qt.io/qt-6/qt.html

//!< size of the array use to store the state of the game for tf_enviroments
constexpr size_t SIZE_STEP_ARRAY = 1 + 2 * 3 * (10 + 3);

/**
 * @brief The Color class is a mock for the Qt version, in case is not
 * available
 */

using Color = int;

std::string
color_to_str(int color);

/**
 * @brief enumeration to determine the leaf of a trefoil
 */
enum class LEAF : uint8_t
{
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
enum class ROTATION : uint8_t
{
  OK = 0,     //!< rotation is possible
  INVALID = 1 //!< rotation is NOT possible
};

/**
 * @brief enumeration for the different state the mechanical parts of a Puzzle
 * can be.
 *
 * @note
 */
enum class TREFOIL : uint8_t
{
  INVALID = 0,         //!< the central disk is rotating out of phase
  LEAF_ROTATION = 1,   //!< rotation of the leaf is possible
  LEAF_SPINNING = 2,   //!< the leaf is rotating on its axis
  BORDER_ROTATION = 3, //!< possible rotation around the outer ring is possible
};

/**
 * @brief enumeration to take into account time evolution or time related states
 */
enum class TIME : uint8_t
{
  PREVIOUS = 0, //!< index of previous state
  CURRENT = 1   //!< index of current state
};

/**
 * @brief  enumeration to identify the side of a trefoil
 */
enum class SIDE : uint8_t
{
  FRONT = 0, //!< front side of a trefoil
  BACK = 1,  //!< back side in a trefoil
};

/**
 * @brief  enumeration of allowed commands
 * @note  This is a **complete** enumeration of commands
 */
enum class COMMANDS : uint8_t
{
  NORTH_RIGHT = 0,
  EAST_RIGHT = 1,
  WEST_RIGHT = 2,
  // ------------------- //
  NORTH_LEFT = 3,
  EAST_LEFT = 4,
  WEST_LEFT = 5,
  // ------------------- //
  NORTH_SPIN = 6,
  EAST_SPIN = 7,
  WEST_SPIN = 8,
  // ------------------- //
  INTERNAL_LEFT = 9,
  INTERNAL_RIGHT = 10,
  // ------------------- //
  SWAP_SIDE = 11,
  N_COMMANDS = 12,
};

} // namespace puzzle

#endif // SPIN_PUZZLE_DEFINITIONS_H
