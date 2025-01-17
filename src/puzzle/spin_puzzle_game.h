#ifndef SPINPUZZLEGAME_H
#define SPINPUZZLEGAME_H

#include <functional>
#include <memory>
#include <sstream>

#include "spin_configuration.h"
#include "spin_puzzle_side.h"

namespace puzzle {

class Recorder;

/**
 * @brief This class rappresent the Two-sided Trefoil, the base for the game
 *
 * It consist of two sides of type \ref SpinPuzzleSide
 * @note
 */
class SpinPuzzleGame
{
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

  SpinPuzzleGame(const SpinPuzzleGame& game) = default;

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

  bool process_command(puzzle::COMMANDS commands);

  /**
   * @brief  shuffle the marbles with a sequence of operations
   * @note
   * @param  seed: seed to be used
   * @param  commands: number of commands
   * @param  check: check consistency after shuffle
   */
  void shuffle(int seed = 0, int commands = 20000, bool check = false);

  /**
   * @brief  shuffle the marbles with a sequence of commands
   * @note   commands are aggregated action: not a simple key press but a
   *         coherent sequence. For example NORTH_RIGHT is the sequence:
   *         Key_N + Arrow_Right.
   * @param  seed: seed to be used
   * @param  commands: number of commands
   * @param  check: check consistency after shuffle
   */
  void shuffle_with_commands(int seed = 0,
                             int commands = 20000,
                             bool check = false);

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
  bool spin_leaf(LEAF leaf, double angle);

  bool spin_leaf(LEAF leaf);

  /**
   * @brief swap the active side of the trefoil
   */
  void swap_side();

  puzzle::SpinPuzzleSide<10, 3>& get_side(SIDE side);

  puzzle::SpinPuzzleSide<10, 3>& get_side();

  /**
   * @brief get the SpinPuzzle For the given Side.

   * @param  side: side to get
   * @retval returns a \ref SpinPuzzleSide for the side.
   */
  const puzzle::SpinPuzzleSide<10, 3>& get_side(SIDE side) const;

  /**
   * @brief get the SpinPuzzle For the given Side.

   * @param  side: side to get
   * @retval returns a \ref SpinPuzzleSide for the side.
   */
  const puzzle::SpinPuzzleSide<10, 3>& get_side() const;

  bool check_consistency(bool verbose = false);

  LEAF get_keybord_state() const { return keyboard.getSection(); }

  /**
   * @brief  utility to print the game state to screen
   * @note  This is useful when using the python binding as in print(game)
   * @retval string rappresentation
   */
  std::string to_string();

  /**
   * @brief  this function check if a game is solved
   * @note   the game must be in LEAF_ROTATION state
   * @retval true if the game is solved.
   */
  bool is_game_solved() const;

  /**
   * @brief Convenient callback to be able to implement
   * tensorflow::tf_environment
   *
   * This rappresentantation is as follow:
   *    - the array is composed of the leaves + 3 places that are the connection
   *      between the leaves when the game is in BORDER_ROTATION state.
   *      So the layout of the array is as follow:
   *                    NORTH if LEAF R.        For EAST and WEST.
   *      [active side] [0-->6][7 8 9][9 8 7]    [0-->6] ...
   *                                if BORDER R.
   * In this case the
   *
   * @note see:
   * https://www.tensorflow.org/agents/api_docs/python/tf_agents/environments/tf_environment
   * @retval array rappresentation of the state.
   */
  std::array<Color, puzzle::SIZE_STEP_ARRAY> current_time_step() const;

  /**
   * @brief  set parameter form the configuration
   * @note
   * @param  config: config to apply
   */
  void set_config(puzzle::Configuration& config);

  /**
   * @brief  function to serialize the game to save it
   * @note
   * @param  buffer: location where to save the data
   * @retval
   */
  template<typename Buffer>
  Buffer& serialize(Buffer& buffer) const
  {
    buffer << "v0"
           << " " << static_cast<uint32_t>(m_active_side) << " "
           << m_spin_rotation[0] << " " << m_spin_rotation[1] << " "
           << m_spin_rotation[2] << " ";
    m_sides[static_cast<uint8_t>(SIDE::FRONT)].serialize(buffer);
    m_sides[static_cast<uint8_t>(SIDE::BACK)].serialize(buffer);
    buffer << "\n";
    return buffer;
  }

  template<typename Buffer>
  Buffer& load(Buffer& buffer)
  {
    std::string version;
    uint32_t active_side;

    buffer >> version;
    buffer >> active_side;
    m_active_side = static_cast<SIDE>(active_side);
    buffer >> m_spin_rotation[0];
    buffer >> m_spin_rotation[1];
    buffer >> m_spin_rotation[2];

    m_sides[static_cast<uint8_t>(SIDE::FRONT)].load(buffer);
    m_sides[static_cast<uint8_t>(SIDE::BACK)].load(buffer);

    return buffer;
  }

  std::FILE* serialize(std::FILE* file) const;

  std::string serialize(std::string& string) const;

  std::FILE* load(std::FILE* file);

  std::string load(std::string& string);

  void attach_recorder(std::shared_ptr<Recorder> recorder);
  void start_recording();

  std::shared_ptr<Recorder> detached_recorder();

private:
  class KeyboardState
  {
  public:
    KeyboardState() = default;

    void selectSection(puzzle::LEAF s) { this->section = s; }

    puzzle::LEAF getSection() const { return this->section; }

  private:
    puzzle::LEAF section = puzzle::LEAF::INVALID;
  };

  bool is_leaf_complete(const puzzle::SpinPuzzleSide<>& side, LEAF leaf) const;

  //!< sides of a trefoil
  std::array<puzzle::SpinPuzzleSide<>, 2> m_sides;
  //!< which side of a trefoil is currently active
  SIDE m_active_side = SIDE::FRONT;
  double m_spin_rotation[static_cast<uint8_t>(LEAF::TREFOIL)] = { 0.0,
                                                                  0.0,
                                                                  0.0 };

  // void debug_iter(const char *name, SpinPuzzleSide<>::iterator it);

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

  std::shared_ptr<Recorder> m_recorder = nullptr;
};

} // namespace puzzle

#endif // SPINPUZZLEGAME_H
