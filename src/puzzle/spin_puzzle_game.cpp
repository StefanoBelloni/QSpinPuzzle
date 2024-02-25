#include "spin_puzzle_game.h"
#include "spin_action_provider.h"

#include <sstream>

namespace puzzle {

SpinPuzzleGame::SpinPuzzleGame(std::array<SpinMarble, 30> front,
                               std::array<SpinMarble, 30> back)
    : m_sides({SpinPuzzleSide<10, 3>(std::move(front)),
               SpinPuzzleSide<10, 3>(std::move(back))}){};

bool SpinPuzzleGame::rotate_marbles(LEAF leaf, double angle) {
  uint8_t n = static_cast<uint8_t>(m_active_side);
  return m_sides[n].rotate_marbles(leaf, angle);
}

bool SpinPuzzleGame::rotate_internal_disk(double angle) {
  uint8_t n = static_cast<uint8_t>(m_active_side);
  return m_sides[n].rotate_internal_disk(angle);
}

// TODO: check of spin is possible based on the position of the marbles ...
bool SpinPuzzleGame::spin_leaf(LEAF leaf, double angle) {

  if (!m_sides[static_cast<uint8_t>(m_active_side)].is_rotation_possible(
          leaf)) {
    return false;
  }

  angle = fmod(angle, 360.0);
  uint8_t n = static_cast<uint8_t>(leaf);
  const double current_spin_angle = m_spin_rotation[n];
  double updated_spin_angle = current_spin_angle + angle;

  // spin angle is always between -90° and +90°:
  // not enough rotation: no spin.
  if (-90 <= updated_spin_angle && updated_spin_angle < 90) {
    m_spin_rotation[n] = updated_spin_angle;
    return false;
  }
  // whenever the new angle exceeds -90 or +90 then,
  // swap the marbles and reset the angle
  LEAF opposite_leaf = get_opposite_leaf(leaf);
  uint8_t active_side_int = static_cast<uint8_t>(m_active_side);
  uint8_t opposite_side_int =
      static_cast<uint8_t>(get_opposite_side(m_active_side));
  uint8_t opposite_leaf_int = static_cast<uint8_t>(opposite_leaf);
  uint8_t leaf_int = static_cast<uint8_t>(leaf);
  // swap from it + 1 to it + 5
  auto state_active = m_sides[active_side_int].get_trifoild_status();
  auto state_opposite = m_sides[opposite_side_int].get_trifoild_status();
  auto it_current = m_sides[active_side_int].begin(
      (state_active != TREFOIL::BORDER_ROTATION) ? leaf : LEAF::TREFOIL);
  auto it_opposite = m_sides[opposite_side_int].begin(
      (state_opposite != TREFOIL::BORDER_ROTATION) ? opposite_leaf
                                                   : LEAF::TREFOIL);
  if (state_active == TREFOIL::BORDER_ROTATION) {
    it_current += leaf_int * SpinPuzzleSide<>::GROUP_SIZE + 1;
  } else {
    it_current += 1;
  }

  if (state_opposite == TREFOIL::BORDER_ROTATION) {
    it_opposite += leaf_int * SpinPuzzleSide<>::GROUP_SIZE + 1;
  } else {
    it_opposite += 1;
  }

  for (size_t n = 1; n <= 5; ++n) {
    debug_iter("spin_leaf: it_current =  ", it_current);
    debug_iter("spin_leaf: it_opposite = ", it_opposite);
    std::iter_swap(it_current, it_opposite);
    debug_iter("spin_leaf: it_current =  ", it_current);
    debug_iter("spin_leaf: it_opposite = ", it_opposite);
    ++it_current;
    ++it_opposite;
  }
  update_spin_rotation_angle(leaf, updated_spin_angle);
  return true;
}

void SpinPuzzleGame::debug_iter(const char *name,
                                SpinPuzzleSide<>::iterator it) {
  if (it->id() == it->INVALID_ID) {
    std::cout << "[DEBUG][" << name << "] marble: id => " << it->id() << "\n";
  }
}

/**
 * @brief swap the active side of the trefoil
 */
void SpinPuzzleGame::swap_side() {
  m_active_side = get_opposite_side(m_active_side);
  update_spin_rotation_angle(LEAF::NORTH);
  update_spin_rotation_angle(LEAF::EAST);
  update_spin_rotation_angle(LEAF::WEST);
}

bool SpinPuzzleGame::check_consistency(bool verbose) {
  return check_consistency_side(SIDE::FRONT, verbose) &&
         check_consistency_side(SIDE::BACK, verbose);
}

std::string SpinPuzzleGame::game_as_stirng() {

  std::stringstream s;
  {
    auto &puzzle = m_sides[static_cast<int8_t>(SIDE::FRONT)];
    s << "FRONT: ";
    auto it = puzzle.begin(puzzle::LEAF::TREFOIL);
    for (size_t n = 0; n < puzzle::SpinPuzzleSide<>::N_MARBLES; ++n, ++it) {
      s << it->id() << ", ";
    }
  }
  s << "\n";
  {
    auto &puzzle = m_sides[static_cast<int8_t>(SIDE::BACK)];
    s << "BACK: ";
    auto it = puzzle.begin(puzzle::LEAF::TREFOIL);
    for (size_t n = 0; n < puzzle::SpinPuzzleSide<>::N_MARBLES; ++n, ++it) {
      s << it->id() << ", ";
    }
  }
  s << "\n";
  return s.str();
}

bool SpinPuzzleGame::check_consistency_side(SIDE side, bool verbose) {
  if (verbose) {
    std::cout << ((side == SIDE::BACK) ? "BACK" : "FRONT") << "\n";
  }
  auto &puzzle = m_sides[static_cast<int8_t>(side)];
  int errors = 0;
  for (int i = 0; i < puzzle::SpinPuzzleSide<>::N_MARBLES * 2; ++i) {
    int count = 0;
    auto it = puzzle.begin(puzzle::LEAF::TREFOIL);
    for (size_t n = 0; n < puzzle::SpinPuzzleSide<>::N_MARBLES; ++n, ++it) {
      if (it->id() == it->INVALID_ID || it->id() < 0 ||
          it->id() > puzzle::SpinPuzzleSide<>::N_MARBLES * 2) {
        if (verbose) {
          std::cout << "ERROR at it->id() " << it->id() << "\n";
          return false;
        }
        ++errors;
      }
      if (it->id() == i) {
        ++count;
      }
    }
    if (count > 1) {
      if (verbose) {
        std::cout << "ERROR at i = " << i << "count is " << i << "\n";
        return false;
      }
      ++errors;
    }
  }
  return errors == 0;
}

void SpinPuzzleGame::update_spin_rotation_angle(LEAF leaf) {
  uint8_t n = static_cast<uint8_t>(leaf);
  update_spin_rotation_angle(leaf, m_spin_rotation[n]);
}

void SpinPuzzleGame::update_spin_rotation_angle(LEAF leaf, double angle) {
  uint8_t n = static_cast<uint8_t>(leaf);
  if (angle > 90) {
    angle = angle - 180;
  } else {
    angle = 180 + angle;
  }
  m_spin_rotation[n] = angle;
}

LEAF SpinPuzzleGame::get_opposite_leaf(LEAF leaf) {
  switch (leaf) {
  case LEAF::NORTH:
    return LEAF::NORTH;
  case LEAF::EAST:
    return LEAF::WEST;
  case LEAF::WEST:
  default:
    return LEAF::EAST;
  }
}

SIDE SpinPuzzleGame::get_opposite_side(SIDE side) {
  switch (side) {
  case SIDE::FRONT:
    return SIDE::BACK;
  case SIDE::BACK:
  default:
    return SIDE::FRONT;
  }
  return side;
}

std::array<SpinMarble, 30> SpinPuzzleGame::createFrontMarbles() {
  std::array<SpinMarble, 30> array = {
      // NORTH
      SpinMarble(0, puzzle::blue), SpinMarble(1, puzzle::blue), SpinMarble(2, puzzle::blue),
      SpinMarble(3, puzzle::blue), SpinMarble(4, puzzle::blue), SpinMarble(5, puzzle::blue),
      SpinMarble(6, puzzle::blue), SpinMarble(7, puzzle::blue), SpinMarble(8, puzzle::blue),
      SpinMarble(9, puzzle::blue),
      // EAST
      SpinMarble(10, puzzle::green), SpinMarble(11, puzzle::green),
      SpinMarble(12, puzzle::green), SpinMarble(13, puzzle::green),
      SpinMarble(14, puzzle::green), SpinMarble(15, puzzle::green),
      SpinMarble(16, puzzle::green), SpinMarble(17, puzzle::green),
      SpinMarble(18, puzzle::green), SpinMarble(19, puzzle::green),
      // WEST
      SpinMarble(20, puzzle::magenta), SpinMarble(21, puzzle::magenta),
      SpinMarble(22, puzzle::magenta), SpinMarble(23, puzzle::magenta),
      SpinMarble(24, puzzle::magenta), SpinMarble(25, puzzle::magenta),
      SpinMarble(26, puzzle::magenta), SpinMarble(27, puzzle::magenta),
      SpinMarble(28, puzzle::magenta), SpinMarble(29, puzzle::magenta)};
  return array;
}

std::array<SpinMarble, 30> SpinPuzzleGame::createBackMarbles() {
  std::array<SpinMarble, 30> array = {
      // NORTH
      SpinMarble(30, puzzle::cyan), SpinMarble(31, puzzle::cyan),
      SpinMarble(32, puzzle::cyan), SpinMarble(33, puzzle::cyan),
      SpinMarble(34, puzzle::cyan), SpinMarble(35, puzzle::cyan),
      SpinMarble(36, puzzle::cyan), SpinMarble(37, puzzle::cyan),
      SpinMarble(38, puzzle::cyan), SpinMarble(39, puzzle::cyan),
      // EAST
      SpinMarble(40, puzzle::red), SpinMarble(41, puzzle::red), SpinMarble(42, puzzle::red),
      SpinMarble(43, puzzle::red), SpinMarble(44, puzzle::red), SpinMarble(45, puzzle::red),
      SpinMarble(46, puzzle::red), SpinMarble(47, puzzle::red), SpinMarble(48, puzzle::red),
      SpinMarble(49, puzzle::red),
      // WEST
      SpinMarble(50, puzzle::yellow), SpinMarble(51, puzzle::yellow),
      SpinMarble(52, puzzle::yellow), SpinMarble(53, puzzle::yellow),
      SpinMarble(54, puzzle::yellow), SpinMarble(55, puzzle::yellow),
      SpinMarble(56, puzzle::yellow), SpinMarble(57, puzzle::yellow),
      SpinMarble(58, puzzle::yellow), SpinMarble(59, puzzle::yellow)};
  return array;
}

void SpinPuzzleGame::reset() {
  m_sides[0] = SpinPuzzleSide(std::move(createFrontMarbles()));
  m_sides[1] = SpinPuzzleSide(std::move(createBackMarbles()));
}

bool SpinPuzzleGame::process_key(int key, double fraction_angle) {
  switch (key) {
  // ========================== //
  case puzzle::Key_N:
    // ========================== //
    keyboard.selectSection(puzzle::LEAF::NORTH);
    return true;
  // ========================== //
  case puzzle::Key_E:
    // ========================== //
    keyboard.selectSection(puzzle::LEAF::EAST);
    return true;
  // ========================== //
  case puzzle::Key_W:
    // ========================== //
    keyboard.selectSection(puzzle::LEAF::WEST);
    return true;
  // ========================== //
  case puzzle::Key_I:
    // ========================== //
    keyboard.selectSection(puzzle::LEAF::CENTER);
    return true;
  // ========================== //
  case puzzle::Key_Left:
  case puzzle::Key_Right:
    // ========================== //
    {
      double direction = (key == puzzle::Key_Left) ? -1.0 : 1.0;
      auto &side = get_side(get_active_side());
      if (keyboard.getSection() == puzzle::LEAF::CENTER) {
        side.rotate_internal_disk(direction * 60.0 * fraction_angle);
      } else if (static_cast<uint8_t>(keyboard.getSection()) <
                 static_cast<uint8_t>(LEAF::TREFOIL)) {
        // make scure only leaves are used
        side.rotate_marbles(keyboard.getSection(),
                            direction * puzzle::SpinPuzzleSide<>::STEP *
                                fraction_angle);
      }
      return true;
    }
  case puzzle::Key_PageUp:
  case puzzle::Key_PageDown:
    // make scure only leaves are used
    if (static_cast<uint8_t>(keyboard.getSection()) <
        static_cast<uint8_t>(LEAF::TREFOIL)) {
      spin_leaf(keyboard.getSection());
    }
    return true;
  case puzzle::Key_P:
    swap_side();
    return true;
  }

  return false;
}

void SpinPuzzleGame::shuffle(int seed, int commands, bool check) {
  int command = 0;
  ActionProvider ap;
  auto vec = ap.getSequenceOfCommands(seed, commands);
  for (int key : vec) {
    process_key(key, 1);
    if (check && !check_consistency()) {
      std::cerr << "[DEBUG][suffle] marbles are in an invalid state after "
                   "processing key "
                << key << ", command: " << command << "\n";
      std::cerr << "[DEBUG][shuffle] GAME:"
                << "\n";
      std::cerr << game_as_stirng().c_str() << "\n";
      assert(check_consistency(true));
    }
    ++command;
  }
}

} // namespace puzzle
