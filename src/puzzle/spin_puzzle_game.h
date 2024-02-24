#ifndef SPINPUZZLEGAME_H
#define SPINPUZZLEGAME_H

#include "spin_puzzle_side.h"

namespace puzzle {

/**
 * @brief This class rappresent the Two-sided Trefoil, the base for the game
 *
 * It consist of two sides of type \ref SpinPuzzleSide
 * @note
 */
class SpinPuzzleGame {
public:
  /**
   * @brief create configuration for the front side of the trefoil
   * @note
   * @retval array with the marbles in the starting position
   */
  static std::array<SpinMarble, 30> createFrontMarbles();
  /**
   * @brief create configuration for the back side of the trefoil
   * @note
   * @retval array with the marbles in the starting position
   */
  static std::array<SpinMarble, 30> createBackMarbles();

  /**
   * @brief constructor of a game.
   * @note
   * @param  front: initial configuration for \ref SIDE::FRONT side
   * @param  back: initial configuration for \ref SIDE::BACK side
   */
  SpinPuzzleGame(
      std::array<SpinMarble, 30> front = SpinPuzzleGame::createFrontMarbles(),
      std::array<SpinMarble, 30> back = SpinPuzzleGame::createBackMarbles());

  /**
   * @brief  reset the game configuration to the initial one
   *
   * @retval None
   */
  void reset();

  /**
   * @brief  utility to process a sequence of commands
   * @note
   * @param  key: operation to perform
   * @param  fraction_angle: fraction of an angle for an operation
   * @retval true on success
   */
  bool process_key(int key, double fraction_angle);

  /**
   * @brief  shuffle the marbles with a sequence of operations
   * @note
   * @param  seed: seed to be used
   * @param  commands: number of commands
   */
  void shuffle(int seed = 0, int commands = 10000, bool check = false);

  /**
   * @brief  setter of the active side
   *
   * @param  side: side to activate \ref SIDE::FRONT \ref SIDE::BACK
   */
  void set_active_side(SIDE side) { m_active_side = side; }

  /**
   * @brief getter of the active side
   * @retval active side
   */
  SIDE get_active_side() const { return m_active_side; }

  // =============================== //
  // ACTIONS:
  /**
   * @brief rotate marbles in the leaf:
   *
   * @note \ref SpinPuzzleSide::rotate_marbles
   * @param  leaf: leaf to spin (\ref puzzle LEAF)
   * @param  angle: angle to rotate (in degree)
   * @retval true on success
   */
  bool rotate_marbles(LEAF leaf, double angle);

  /**
   * @brief rotate internal disk:
   * @note see \ref SpinPuzzleSide::rotate_internal_disk
   * @param  angle: angle to rotate (in degree)
   * @retval true on success
   */
  bool rotate_internal_disk(double angle);

  /**
   * @brief start spinning the given leaf
   * @param  leaf: leaf to spin (\ref puzzle LEAF ): default is 180 i.e. swap
   *         the faces.
   * @param angle: angle (in degree) to spin the leaf - default is 180°, i.e.
   *        a complete spin of the leaf.
   * @note the swap happens as follow:
   *     * swap \f$(it(leaf_f; front) + n) \leftrightarrow it(leaf_b; back) +
   * n\f$ with \f$it = \texttt{iterator of the section leaf}\f$ and
   *      \f$n\in{1,\ldots,5}\f$
   *     * \ref LEAF::NORTH \f$\leftrightarrow\f$ LEAF::NORTH
   *     * \ref LEAF::EAST \f$\leftrightarrow\f$ LEAF::WEST
   *     * \ref LEAF::WEST \f$\leftrightarrow\f$ LEAF::EAST
   * @retval true if the marbles have been swaped and the spin succeeded
   */
  // TODO: angle 180 will not rotate, eventhough it should rotate!!!!
  bool spin_leaf(LEAF leaf, double angle = 180);

  /**
   * @brief swap the active side of the trefoil
   */
  void swap_side();

  /**
   * @brief get the SpinPuzzle For the given Side.

   * @param  side: side to get
   * @retval returns a \ref SpinPuzzleSide for the side.
   */
  puzzle::SpinPuzzleSide<10, 3> &get_side(SIDE side) {
    uint8_t n = static_cast<uint8_t>(side);
    return m_sides[n];
  }

  bool check_consistency(bool verbose = false);

  std::string game_as_stirng();

  LEAF get_keybord_state() const { return keyboard.getSection(); }

private:
  class KeyboardState {
  public:
    KeyboardState() = default;

    void selectSection(puzzle::LEAF s) { this->section = s; }

    puzzle::LEAF getSection() const { return this->section; }

  private:
    puzzle::LEAF section = puzzle::LEAF::INVALID;
  };

  //!< sides of a trefoil
  std::array<puzzle::SpinPuzzleSide<>, 2> m_sides;
  //!< which side of a trefoil is currently active
  SIDE m_active_side = SIDE::FRONT;
  double m_spin_rotation[static_cast<uint8_t>(LEAF::TREFOIL)] = {0.0, 0.0, 0.0};

  void debug_iter(const char *name, SpinPuzzleSide<>::iterator it);

  //!< state when input is given by a batch
  KeyboardState keyboard;

  /**
   * @brief  get opposite leave for for the spin
   * @param  leaf: leaf to spin
   * @retval opposite leaf
   */
  LEAF get_opposite_leaf(LEAF leaf);
  /**
   * @brief  get opposite side of the active one
   *
   * @param  side: source side
   * @retval opposite side
   */
  SIDE get_opposite_side(SIDE side);

  void update_spin_rotation_angle(LEAF leaf, double angle);
  void update_spin_rotation_angle(LEAF leaf);
  bool check_consistency_side(SIDE side, bool verbose);
};

} // namespace puzzle

#endif // SPINPUZZLEGAME_H
