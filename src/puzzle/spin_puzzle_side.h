#ifndef SPINPUZZLESIDE_H
#define SPINPUZZLESIDE_H

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <math.h>

#include "spin_marble.h"

namespace puzzle {

/**
 * @brief SpinPuzzleSide deals with a single side of a Trefoil.
 *
 * Each side of a Trefoil is characterized by 4 different states, according to
 * the configuration of the different mechanical elements:
 *   - \ref TREFOIL::LEAF_ROTATION : the rail of leaf (i.e. every leaf) are
 * close, so that the marbles can rotate inside a single leaf.
 *   - \ref TREFOIL::BORDER_ROTATION : the InternalDisk is at ~60° so that the
 * rails of the different leaf are connected together, allowing the marbles to
 * rotate along the "border" of the Trefoil
 *   - \ref TREFOIL::INVALID : the InternalDisk is rotating from
 *     \ref TREFOIL::LEAF_ROTATION to \ref TREFOIL::BORDER_ROTATION of
 * viceversa: in this state non of the marbles can rotate
 *   - \ref TREFOIL::LEAF_SPINNING : in this configuration at least on of the
 *     leaves are spinning around it axis.
 *     @note Although in the real game rotation of the internal Disk as well as
 *     rotation inside the other leaves are still possible in this state,
 *     in our implementation this state (\ref TREFOIL::LEAF_SPINNING ) will
 * block all other action. This state can be activated only from outside.
 *
 * The marble list is divided in three section, one for each leaf:
 * Each section is charachterized by a starting position. This is identified as
 * the first position outside the Common Area with the InternalDisk following a
 * clockwise rotation. In case of 10 marbles for leaf, for the NorthLeaf is the
 * second to the left starting from the bottom marble.
 *
 * According to the state the game is in, the navigation of the marbles changes.
 * The general idea is that following the sequence of marbles inside a track
 * (be it of a leaf of the one created by connecting the leaves) you can follow
 * the corresponding marbles inside the corresponding section in order.
 *
 *   - \ref TREFOIL::LEAF_ROTATION : in this configuration each section occupy N
 * consecutive position with the fisrt marble inside the NorthLeaf at position
 * 0. In this configuration, rotating the marbles move the starting position in
 * the range [n, n + N), with n equal to 0 for NorthLeaf, 1 for EastLeaf and 2
 * for WestLeaf.
 *   - \ref TREFOIL::BORDER_ROTATION : in this configuration from the start
 * position of the NorthLeaf we can follow the list of marble in the same
 * sequence as it
 *   - \ref TREFOIL::INVALID : the sequence is the same as LEAF_ROTATION.
 *
 * The configuration of the marbles is a function also of the relative rotation
 * of:
 *    - InternalDisk: how much it deviates from the vertical
 *    - the shift of a marble (i.e. all marbles) from its equilibrium position
 *      that is identified as the multiple of the angle \f$\theta_0 :=
 * \frac{2\pi}{N}\f$
 * \f[
 * pos = i \texttt{ if } \theta_{pos(i)}\in
 * (i\cdot\theta_0 - \frac{\theta_0}{2},
 * i\cdot\theta_0 - \frac{\theta_0}{2})
 * \f]
 * where \f$N\f$ is the number of marbles for each group.
 *
 * The rotation of the upper semi-leaf or the rotation of the InternalCircle can
 * happen only if no marble is inside the division border, i.e.
 *    - accross semi-leaf border
 *    - accross the division of the common section between leaf and
 * InternalCircle.
 *
 * @note in general the angles are express in degree.
 *
 * @tparam template parameter N is the number of marbles inside a single leaf.
 * @tparam template parameter M is the number of marbles inside the common area.
 */
template <std::size_t N = 10ul, std::size_t M = 3ul> class SpinPuzzleSide {

private:
  //!< number of leaves in the trefoil
  static constexpr std::size_t N_LEAVES = 3;
  //!< angle between two consecutive marbles
  static constexpr double DTHETA = 360.0 / N;

public:
  //!< tollerance of an angle in degree when checking conditions.
  static constexpr int TOLLERANCE_ANGLE = 5;
  static constexpr double STEP = SpinPuzzleSide::DTHETA;
  static constexpr double GROUP_SIZE = N;
  //!< number of marbles
  static constexpr std::size_t N_MARBLES = N_LEAVES * N;

public:
  /**
   * @brief class that collect the state of a side
   */
  class Status {
    //!< phase schifts of the different leaves
    double m_shifts_leaves[N_LEAVES] = {0.0, 0.0, 0.0};
    //!< phase schifts of the central disk
    double m_shift_cdisk = 0.0;
    //!< iterators to keep track of the start position of every section
    // typename SpinPuzzleSide<N, M>::iterator m_start_sections[N_LEAVES];
    //!< last 2 states of of the mechanical parts
    TREFOIL m_trefoil_status[2] = {TREFOIL::INVALID, TREFOIL::LEAF_ROTATION};
    //!< state of the marbles
    ROTATION m_rotation_status[N_LEAVES + 1] = {
        ROTATION::OK, ROTATION::OK, ROTATION::OK, ROTATION::INVALID};

  public:
    //!< getter for the local shift in degree of the first marble of the section
    double get_shift_of_leaf(LEAF leaf) const {
      return m_shifts_leaves[static_cast<uint8_t>(leaf)];
    }
    //!< update the local shift for the given leaf
    double update_shift_for_leaf(LEAF leaf, double angle) {
      uint8_t n = static_cast<uint8_t>(leaf);
      const double alpha = get_shift_of_leaf(leaf);
      m_shifts_leaves[n] = std::fmod(alpha + angle + 360.0, 360.0);
      return m_shifts_leaves[n];
    }
    //!< set the local shift for the given leaf
    double set_shift_for_leaf(LEAF leaf, double angle) {
      uint8_t n = static_cast<uint8_t>(leaf);
      const double alpha = get_shift_of_leaf(leaf);
      m_shifts_leaves[n] = std::fmod(angle + 360.0, 360.0);
      return m_shifts_leaves[n];
    }
    //!< setter for the status of \ref puzzle::ROTATION for a leaf
    bool set_rotation_status(LEAF leaf, ROTATION status) {
      const auto n = static_cast<uint8_t>(leaf);
      m_rotation_status[n] = status;
      return true;
    }
    //!< getter for the status of rotation for a leaf
    ROTATION get_rotation_status(LEAF leaf) const {
      const auto n = static_cast<uint8_t>(leaf);
      return m_rotation_status[n];
    }
    //!< check if a rotation is possible, i.e. every leaf is \ref ROTATION::OK
    bool is_internal_disk_rotation_possible() const {
      // TODO: what to do in case of border rotation ?
      return m_rotation_status[static_cast<uint8_t>(LEAF::NORTH)] ==
                 ROTATION::OK &&
             m_rotation_status[static_cast<uint8_t>(LEAF::EAST)] ==
                 ROTATION::OK &&
             m_rotation_status[static_cast<uint8_t>(LEAF::WEST)] ==
                 ROTATION::OK;
    }

    //!< getter for the current shift of the central disk
    double get_central_disk_shift() const { return m_shift_cdisk; }
    //!< setter for central disk shift.
    void set_central_disk_shift(double angle) { m_shift_cdisk = angle; }

    /**
     * @brief  retrieve the status of a rotation \ref puzzle::ROTATION for a
     * leaf
     *
     * @param  leaf: section in the trefoil
     * @retval status of the rotation \ref ROTATION
     */
    ROTATION get_leaf_rotation_status(LEAF leaf) const {
      int8_t n = static_cast<uint8_t>(leaf);
      return m_rotation_status[n];
    }
    /**
     * @brief retrieve the current or previous status of the trefoil
     *
     * @param  t: time parameter: previous or current status \ref TIME
     * @retval state for the given time.
     */
    TREFOIL get_trefoil_status(TIME t) const {
      int8_t n = static_cast<uint8_t>(t);
      return m_trefoil_status[n];
    }

    void set_trefoil_status(TREFOIL status) {
      int8_t n_0 = static_cast<uint8_t>(TIME::PREVIOUS);
      int8_t n_1 = static_cast<uint8_t>(TIME::CURRENT);
      m_trefoil_status[n_0] = m_trefoil_status[n_1];
      m_trefoil_status[n_1] = status;
    }
  };

public:
  /**
   * @brief  Default constructor is deleted.
   */
  SpinPuzzleSide() = delete;

  /**
   * @brief Constructor with a single list of marbles (they must be N_LEAVES *
   * N)
   *
   * @note The puzzle side has to be manually initialized with a list of marbles
   * the order is for a \ref TREFOIL::LEAF_ROTATION state with the fisrt
   * position as in the class description
   * @param  marbles: list of marbles to be used in this side of the game
   */
  template <std::size_t U>
  SpinPuzzleSide(std::array<puzzle::SpinMarble, U> &&marbles);

  /**
   * @brief iterator class to navigate inside a section of marbles.
   *
   * @note in the construction one needs to provide which section is and the
   * state of the trefoil, in order to properly navigate through the marbles
   */
  class iterator {

    // member types

  public:
    iterator() = default;

    // This does not really work:
    // if state is in BORDER_ROTATION then NORTH/EAST/WEST are K.O
    // and vice versa: this is py the python interface has specific
    // callback for north, east, west, border.
    iterator(LEAF leaf, std::array<SpinMarble, N_MARBLES> &marbles,
             Status status, std::ptrdiff_t pos = 0, double angle = 0.0)
        : m_begin_range(marbles.begin()), m_end_range(marbles.end()),
          m_curr(m_begin_range),
          m_current_status(status.get_trefoil_status(TIME::CURRENT)),
          m_leaf(leaf), m_angle(angle) {

      // trefoil
      if (leaf != LEAF::TREFOIL) {
        m_begin_range = marbles.begin() + static_cast<uint64_t>(leaf) * N;
        m_end_range = marbles.begin() + (static_cast<uint64_t>(leaf) + 1) * N;
        m_curr = m_begin_range;
      }

      this->operator+=(pos);
    }

    iterator(const iterator &other) = default;

    iterator &operator++() { return (*this += 1); }

    iterator &operator--() { return (*this -= 1); }

    iterator operator+(std::ptrdiff_t offset) {
      auto tmp = *this;
      tmp += offset;
      return tmp;
    }

    iterator operator-(std::ptrdiff_t offset) {
      auto tmp = *this;
      tmp -= offset;
      return tmp;
    }

    // index inside the complete marbles
    size_t index() {
      return static_cast<uint64_t>(m_leaf) * N + (m_curr - m_begin_range);
    }

    iterator &operator-=(std::ptrdiff_t offset) { return *this += (-offset); }

    /**
     * @brief operator to increment an iterator
     * @note  it operate iniside the given section specified with the leaf
     * @param  offset: increment to apply
     * @retval itself.
     */
    iterator &operator+=(std::ptrdiff_t offset) {
      auto n = m_end_range - m_begin_range;
      if (offset == 0 || m_current_status == TREFOIL::LEAF_SPINNING) {
        return *this;
      }
      if (offset < 0) {
        offset = ((offset % n) + n) % n;
      }
      if (offset >= n) {
        offset = offset % n;
      }
      auto remaining = m_end_range - m_curr;
      if (offset < remaining) {
        m_curr += offset;
        return *this;
      }
      offset -= remaining;
      m_curr = m_begin_range + offset;
      return *this;
    }

    //!< convinient operator
    bool friend operator==(const SpinPuzzleSide<N, M>::iterator &self,
                           const SpinPuzzleSide<N, M>::iterator &other) {
      return (*self == *other);
    }

    //!< convinient operator
    SpinMarble *operator->() { return m_curr; }
    //!< convinient operator
    const SpinMarble *operator->() const { return m_curr; }
    //!< convinient operator
    const SpinMarble &operator*() const { return *m_curr; }
    //!< convinient operator
    SpinMarble &operator*() { return *m_curr; }
    //!< relative shift of the marble
    double get_angle() const { return m_angle; }

  private:
    typename std::array<SpinMarble, N_MARBLES>::iterator m_begin_range;
    typename std::array<SpinMarble, N_MARBLES>::iterator m_end_range;
    typename std::array<SpinMarble, N_MARBLES>::iterator m_curr;

    TREFOIL m_current_status;
    LEAF m_leaf;
    double m_angle;
  };

  /**
   * @brief  retirve the marbles of a given section
   *
   * @note the fisrt marble is not necessary the first after the common aread
   *       between leaf and CentralDisk: to retrieve it use the function
   *       \ref SpinPuzzleSide::begin(LEAF)
   * @param  leaf: section of the trefoil
   * @param  pos: the iterator should point to the given position after the
   * first
   * @param angle: relative shift of the marble w.r.t. its baricenter at 2pi/N
   * @retval iterator to navigate inside this section.
   */
  iterator marbles(LEAF leaf = LEAF::NORTH, std::size_t pos = 0ul,
                   double angle = 0.0) {
    return iterator(leaf, m_marbles, m_status, pos, angle);
  }

  /**
   * @brief  method to rotate the marbles
   *
   * The marbles in the given leaf are rotate of the given angle
   *   - positive angle: clockwise
   *   - negative angle: anticlockwise
   *
   * The rotation is possible only if the state allows it, it means that
   * if the state is \ref TREFOIL::INVALID the function will return false.
   *
   * @note in case of \ref LEAF::TREFOIL all marbles are rotated
   * @note the way the rotation is executed, i.e. if the starting position is
   *       updated or only the m_shift_leaves of the rotation is an
   * implementation detail.
   *
   * @param  leaf: section to update
   * @param  angle: angle to rotate (in degree)
   * @note rotating leave is 1°, rotating border is normalized so that 360 will
   *       return to the starting point
   * @retval true in case the operation was succesful, false otherwise
   */
  bool rotate_marbles(LEAF leaf, double angle);

  /**
   * @brief check if we might pass from internal rotation to border rotation
   * @param  leaf: leaf to check
   * @retval true if it is possible
   */
  bool is_border_rotation_possible() const;

  /**
   * @brief rotate the internal this by the given angle
   *
   * @note a positive value of the angle means clockwise rotation
   * The rotation will not change the sequence of the marbles unless special
   * values are crossed.
   *
   * In particular in the interval
   * (-120° + \ref TOLLERANCE_ANGLE , 120° - \ref TOLLERANCE_ANGLE )
   * the sequence of marbles are not modified.
   * Corssing the border of the interval will rearrange the marbles, so
   * that the marbles in the newly created leaf-track starts from the
   * beginning of the section with no local shift.
   *
   * @note no anlge between 120° + \ref TOLLERANCE_ANGLE and 240° - \ref
   * TOLLERANCE_ANGLE is allowed.
   *
   * Let consider the leaves:
   * \code{.cpp}
   *    NORTH = [ 0,  1,  2, ...,  7,  8,  9]
   *    EAST  = [10, 11, 12, ..., 17, 18, 19]
   *    WEST  = [20, 21, 22, ..., 27, 28, 29]
   * \endcode
   * and suppose we move the internal disk from 0 to 120°.
   * At this point we have to swap some marbles, in particular the 3 marbles
   * belonging to the common area disk-leaf will move:
   *
   *     NORTH -> EAST -> WEST
   *
   * the new marbles are
   * \code{.cpp}
   *    NORTH = [ 0,  1,  2, ..., 27, 28, 29]
   *    EAST  = [10, 11, 12, ...,  7,  8,  9]
   *    WEST  = [20, 21, 22, ..., 17, 18, 19]
   * \endcode
   *
   * @param  angle: amount to rotate the disk: positive is clockwise
   * @retval true if the update was possible
   */
  bool rotate_internal_disk(double angle);

  /**
   * @brief return the first marble of the section
   *
   * @note this is a combination of marbles(leaf) + angle operation on
   * m_shifts_leaves[leaf]
   * @note The first marble is the first (clockwise) marble after the common
   * part with the InternalDisk. The local shift is a convivient information
   * when one would like to paint the marbles in scene.
   * @param  leaf: section to return
   * @retval iterator the first marble of the section (local shift).
   */
  iterator begin(LEAF leaf) {
    if (leaf == LEAF::TREFOIL) {
      return begin();
    }
    if (m_status.get_trefoil_status(TIME::CURRENT) !=
        TREFOIL::BORDER_ROTATION) {
      const double theta = m_status.get_shift_of_leaf(leaf);
      const double theta0 = theta + DTHETA / 2;
      const double t = fmod(theta0 + 360.0, 360.0);
      const int pos = -floor(t / DTHETA);
      const double alpha = theta + pos * DTHETA;
      return marbles(leaf, pos, alpha);
    } else {
      const double DTHETA12 = DTHETA / 12.0;
      const double theta = m_status.get_shift_of_leaf(leaf);
      const double theta0 = theta + DTHETA12 / 2;
      const double t = fmod(theta0 + 360.0, 360.0);
      const int pos = -floor(t / DTHETA12);
      const double alpha = theta + pos * DTHETA12;
      return marbles(leaf, pos, alpha);
    }
  }

  /**
   * @brief iterator for border rotation
   * @retval iterator the first marble of the section and its (local shift).
   */
  iterator begin() {
    if (m_status.get_trefoil_status(TIME::CURRENT) !=
        TREFOIL::BORDER_ROTATION) {
      const double theta = m_status.get_shift_of_leaf(LEAF::NORTH);
      const double theta0 = theta + DTHETA / 2;
      const double t = fmod(theta0 + 360.0, 360.0);
      const int pos = -floor(t / DTHETA);
      const double alpha = theta + pos * DTHETA;
      return marbles(LEAF::TREFOIL, pos, alpha);
    } else {
      const double DTHETA12 = DTHETA / 12.0;
      const double theta = m_status.get_shift_of_leaf(LEAF::NORTH);
      const double theta0 = theta + DTHETA12 / 2;
      const double t = fmod(theta0 + 360.0, 360.0);
      const int pos = -floor(t / DTHETA12);
      const double alpha = theta + pos * DTHETA12;
      return marbles(LEAF::TREFOIL, pos, alpha);
    }
  }

  /**
   * @brief retrieve the phase of the first marble when calling marbles()
   *
   * @note the first marble from the function marbles(leaf) is not necessary the
   * same as \ref SpinPuzzleSide::begin(LEAF) .
   * what is garanteed is that:
   *         \f$ m + phase-shift = it + \alpha\f$
   * with \f$ m = marbles(leaf) \f$ and \f$ (it, \alpha) = begin(leaf) \f$
   * @param  leaf: section of the Trefoil
   * @retval angle in degree to the first marble of the marbles retrieve via the
   *         function marbles(LEAF)
   */
  double get_phase_shift_leaf(LEAF leaf) {
    return m_status.get_shift_of_leaf(leaf);
  }
  //!< getter for internal disk rotation
  double get_phase_shift_internal_disk() {
    return m_status.get_central_disk_shift();
  }

  /**
   * @brief  getter for the status of the trefoil
   *
   * @note   it retrievs (at the moment) the current state
   * @retval status of the "game side" \ref TREFOIL
   */
  TREFOIL get_trifoild_status() const {
    return m_status.get_trefoil_status(TIME::CURRENT);
  }

  // TODO: maybe we can implicitly call it if a rotation of the internal
  /**
   * @brief function to be called before starting to rotate the InternalDisk
   *
   * @retval true on success.
   */
  bool start_rotating_central_disk() {
    if (!is_internal_disk_rotation_possible()) {
      return false;
    }
    m_status.m_trefoil_status = TREFOIL::INVALID;
    return true;
  }

  /**
   * @brief  check if the spinning of a leaf is possible
   *
   * @param  leaf: leaf to check
   * @retval true if is possible to spin the leaf
   */
  bool is_leaf_spinning_possible(LEAF leaf) {
    return m_status.get_rotation_status(leaf) == ROTATION::OK;
  }

  /**
   * @brief start the spinning phase.
   *
   * @note this save the current state to be retor when the spinning is
   *       concluded
   *
   * @param  leaf: leaf to spin
   * @retval true if spinning was succesful.
   */
  bool start_spinning_leaf(LEAF leaf) {
    if (!is_leaf_spinning_possible(leaf)) {
      return false;
    }
    m_status.set_trefoil_status(TREFOIL::LEAF_SPINNING);
    return true;
  }

  /**
   * @brief  conclude the spinning phase
   *
   * @note this function has to be called otherwise all other action are invalid
   * After the spinning phase is concluded, the state of the trefoil is
   * the one before the spinning started.
   *
   * @retval true if the change of state was succesful.
   */
  bool end_spinning_leaf() {
    if (m_status.get_trefoil_status(TIME::CURRENT) != TREFOIL::LEAF_SPINNING) {
      return false;
    }
    m_status.set_trefoil_status(m_status.get_trefoil_status(TIME::PREVIOUS));
    return true;
  }

  //!< check if roations are possible
  bool is_internal_disk_rotation_possible() const {
    return m_status.is_internal_disk_rotation_possible();
  }

  bool is_rotation_possible(LEAF leaf) const {
    return m_status.get_rotation_status(leaf) == ROTATION::OK;
  }

  std::string to_string() {
    std::string str;
    str += "marbles: ";
    if (m_status.get_trefoil_status(TIME::CURRENT) ==
        TREFOIL::BORDER_ROTATION) {
      str += "\nBORDER: ";
      auto it = begin(LEAF::TREFOIL);
      for (size_t n = 0; n < N_MARBLES; ++n) {
        if (n > 0) {
          str += ", ";
        }
        str += it->to_string();
        ++it;
      }
    } else {
      str += "\nNORTH: ";
      auto it = begin(LEAF::NORTH);
      for (size_t n = 0; n < GROUP_SIZE; ++n) {
        if (n > 0) {
          str += ", ";
        }
        str += it->to_string();
        ++it;
      }
      str += "\nEAST: ";
      it = begin(LEAF::EAST);
      for (size_t n = 0; n < GROUP_SIZE; ++n) {
        if (n > 0) {
          str += ", ";
        }
        str += it->to_string();
        ++it;
      }
      str += "\nWEST: ";
      it = begin(LEAF::WEST);
      for (size_t n = 0; n < GROUP_SIZE; ++n) {
        if (n > 0) {
          str += ", ";
        }
        str += it->to_string();
        ++it;
      }
    }
    return str;
  }

  void current_time_step(size_t start_index,
                         std::array<Color, puzzle::SIZE_STEP_ARRAY> &out);

private:
  static_assert(N_LEAVES == static_cast<std::size_t>(LEAF::TREFOIL));

  //!< list of marbles in this side of the game
  std::array<puzzle::SpinMarble, N_LEAVES * N> m_marbles;

  //!< status of the mechanical parts of the trefoil
  Status m_status;

  //!< rotate a leaf of a given angle
  bool rotate_leaf(LEAF leaf, double angle);

  //!< set the status the roation for a given leaf, i.e. if marbles crosses
  //!< sections
  void update_rotation_status(LEAF leaf);

  //!< to start to rotate accross the border-ring, we will rebuild the marbles
  void prepare_for_border_rotation();

  //!< to start rotating after border rotation
  void prepare_from_border_rotation();

  //!< utility to reset marbels from NORTH-origin to start border rotation
  void reset_marbles_for_border_rotation();
  //!< utility to reset marbels from NORTH-origin after border rotation
  void reset_marbles_from_border_rotation();
  //!< utility to reorder the marbels according to the internal disk phase
  //!< after a reset.
  void reorder_marbles_after_border_reset();

  //!< rotate the marbles around the outer ring
  bool rotate_border(LEAF leaf, double alpha) {
    m_status.update_shift_for_leaf(LEAF::NORTH, alpha);
    m_status.update_shift_for_leaf(LEAF::EAST, alpha);
    m_status.update_shift_for_leaf(LEAF::WEST, alpha);
    update_rotation_status(LEAF::NORTH);
    update_rotation_status(LEAF::EAST);
    update_rotation_status(LEAF::WEST);
    return true;
  }

  //!< update shift angle, after resetting 1° marble at origin
  double get_angle_for_origin(LEAF leaf) {
    const double DTHETA12 = DTHETA / 1.0;
    const double theta = m_status.get_shift_of_leaf(leaf);
    const double theta0 = theta + DTHETA12 / 2;
    const double t = fmod(theta0 + 360.0, 360.0);
    const int pos = -floor(t / DTHETA12);
    const double alpha = theta + pos * DTHETA12;
    return alpha;
  }
};

// ========================================================================== //
// IMPLEMENTATION
// ========================================================================== //

template <std::size_t N, std::size_t M>
template <std::size_t U>
inline SpinPuzzleSide<N, M>::SpinPuzzleSide(
    std::array<puzzle::SpinMarble, U> &&marbles)
    : m_marbles(std::move(marbles)) {
  static_assert(U == N * N_LEAVES, "Mismatch in the number of marbles");
}

template <std::size_t N, std::size_t M>
bool SpinPuzzleSide<N, M>::is_border_rotation_possible() const {
  auto status = m_status.get_trefoil_status(TIME::CURRENT);
  // check if we move from internal disk rotation to border rotation
  if (status == TREFOIL::INVALID) {
    const double shift = m_status.get_central_disk_shift();
    const double idisk_shift = fmod(shift + 360.0, 360.0);
    if ((300 - TOLLERANCE_ANGLE <= idisk_shift &&
         idisk_shift <= 300 + TOLLERANCE_ANGLE) ||
        (60 - TOLLERANCE_ANGLE <= idisk_shift &&
         idisk_shift <= 60 + TOLLERANCE_ANGLE)) {
      return true;
    }
  }
  return false;
}

template <std::size_t N, std::size_t M>
bool SpinPuzzleSide<N, M>::rotate_marbles(LEAF leaf, double angle) {
  bool rotated = false;

  auto status = m_status.get_trefoil_status(TIME::CURRENT);
  // check if we move from internal disk rotation to border rotation
  if (is_border_rotation_possible()) {
    prepare_for_border_rotation();
    status = TREFOIL::BORDER_ROTATION;
    m_status.set_trefoil_status(status);
  }

  if (status == TREFOIL::INVALID || status == TREFOIL::LEAF_SPINNING) {
    return false;
  } else if (status == TREFOIL::LEAF_ROTATION) {
    return rotate_leaf(leaf, angle);
  } else if (status == TREFOIL::BORDER_ROTATION) {
    return rotate_border(leaf, angle / 12);
  }
  return rotated;
}

template <std::size_t N, std::size_t M>
void SpinPuzzleSide<N, M>::update_rotation_status(LEAF leaf) {
  uint8_t n = static_cast<uint8_t>(leaf);
  double alpha = std::fmod(
      std::fmod(m_status.get_shift_of_leaf(leaf) + 360.0, 360.0), DTHETA);
  double dtheta = DTHETA;
  double tollerance = TOLLERANCE_ANGLE;
  if (m_status.get_trefoil_status(TIME::CURRENT) == TREFOIL::BORDER_ROTATION) {
    dtheta /= 12;
    tollerance /= 12;
  }
  m_status.set_rotation_status(
      leaf, (dtheta / 2 - tollerance < alpha && alpha < dtheta / 2 + tollerance)
                ? ROTATION::INVALID
                : ROTATION::OK);
}

template <std::size_t N, std::size_t M>
bool SpinPuzzleSide<N, M>::rotate_leaf(LEAF leaf, double angle) {
  m_status.update_shift_for_leaf(leaf, angle);
  update_rotation_status(leaf);
  return true;
}

template <std::size_t N, std::size_t M>
bool SpinPuzzleSide<N, M>::rotate_internal_disk(double angle) {
  if (!m_status.is_internal_disk_rotation_possible()) {
    return false;
  }
  // normalize the angle
  const double alpha =
      std::fmod(m_status.get_central_disk_shift() + 360.0, 360.0);
  const double new_shift_cdisk = std::fmod(alpha + angle + 360.0, 360.0);
  // ======================================================================== //
  // FROM BORDER ROTATION TO INTERNAL DISK ROTATION
  if (m_status.get_trefoil_status(TIME::CURRENT) == TREFOIL::BORDER_ROTATION) {
    if ((300 - TOLLERANCE_ANGLE <= alpha && alpha <= 300 + TOLLERANCE_ANGLE) ||
        (60 - TOLLERANCE_ANGLE <= alpha && alpha <= 60 + TOLLERANCE_ANGLE)) {
      prepare_from_border_rotation();
    }
  }

  angle = fmod(angle + 360.0, 360.0);
  m_status.set_trefoil_status(TREFOIL::INVALID);

  if (120 - TOLLERANCE_ANGLE <= new_shift_cdisk &&
      new_shift_cdisk < 240 - TOLLERANCE_ANGLE) {
    // ROTATE marbles NORTH->EAST->WEST
    auto north_it = this->begin(LEAF::NORTH) - 1;
    auto east_it = this->begin(LEAF::EAST) - 1;
    auto west_it = this->begin(LEAF::WEST) - 1;
    for (int8_t i = 1; i <= 3; ++i, --north_it, --east_it, --west_it) {
      std::iter_swap(north_it, west_it);
      std::iter_swap(east_it, west_it);
    }
    // recalcualte the shift angle
    const double updated_angle = new_shift_cdisk - 120;
    m_status.set_central_disk_shift(updated_angle);
  } else if (240 - TOLLERANCE_ANGLE <= new_shift_cdisk &&
             new_shift_cdisk <= 240 + TOLLERANCE_ANGLE) {
    // ROTATE marbles WEST->EAST->NORTH
    auto north_it = this->begin(LEAF::NORTH) - 1;
    auto east_it = this->begin(LEAF::EAST) - 1;
    auto west_it = this->begin(LEAF::WEST) - 1;
    for (int8_t i = 1; i <= 3; ++i, --north_it, --east_it, --west_it) {
      std::iter_swap(north_it, east_it);
      std::iter_swap(east_it, west_it);
    }
    // recalcualte the shift angle
    const double updated_angle = new_shift_cdisk - 240;
    m_status.set_central_disk_shift(updated_angle);
  } else {
    m_status.set_central_disk_shift(new_shift_cdisk);
  }
  if ((0 <= new_shift_cdisk && new_shift_cdisk <= TOLLERANCE_ANGLE) ||
      ((360 - TOLLERANCE_ANGLE) <= new_shift_cdisk && new_shift_cdisk <= 360) ||
      (120 - TOLLERANCE_ANGLE <= new_shift_cdisk &&
       new_shift_cdisk <= 120 + TOLLERANCE_ANGLE) ||
      ((240 - TOLLERANCE_ANGLE) <= new_shift_cdisk &&
       new_shift_cdisk <= 240 + TOLLERANCE_ANGLE)) {
    m_status.set_trefoil_status(TREFOIL::LEAF_ROTATION);
  }

  return true;
}
template <std::size_t N, std::size_t M>
void SpinPuzzleSide<N, M>::reset_marbles_from_border_rotation() {
  std::array<puzzle::SpinMarble, N_LEAVES * N> tmp;
  // start positioning the marbles that are fixed in the leaves
  auto it = this->begin();
  auto begin_tmp = tmp.begin();
  for (size_t n = 0; n < 3 * N; ++it, ++n) {
    std::iter_swap(begin_tmp + n, it);
  }
  std::swap(tmp, m_marbles);
  m_status.set_shift_for_leaf(LEAF::NORTH, 0.0);
  m_status.set_shift_for_leaf(LEAF::EAST, 0.0);
  m_status.set_shift_for_leaf(LEAF::WEST, 0.0);
}

template <std::size_t N, std::size_t M>
void SpinPuzzleSide<N, M>::reorder_marbles_after_border_reset() {
  // depending if the central disk is +60° or -60° we have swap the marbles
  // across the section differently
  const double disk_shift = m_status.get_central_disk_shift();
  const double shift = fmod(disk_shift + 360.0, 360.0);
  // reset iterators
  auto it_north = begin(LEAF::NORTH);
  auto it_east = begin(LEAF::EAST);
  auto it_west = begin(LEAF::WEST);
  if ((60 - TOLLERANCE_ANGLE <= shift && shift <= 60 + TOLLERANCE_ANGLE)) {

    std::iter_swap(it_north + N - 3, it_north + N - 1);
    std::iter_swap(it_east + N - 3, it_east + N - 1);
    std::iter_swap(it_west + N - 3, it_west + N - 1);
  } else if ((300 - TOLLERANCE_ANGLE <= shift &&
              shift <= 300 + TOLLERANCE_ANGLE)) {

    std::iter_swap(it_north + N - 3, it_east + N - 1);
    std::iter_swap(it_north + N - 2, it_east + N - 2);
    std::iter_swap(it_north + N - 1, it_east + N - 3);

    std::iter_swap(it_west + N - 3, it_east + N - 1);
    std::iter_swap(it_west + N - 2, it_east + N - 2);
    std::iter_swap(it_west + N - 1, it_east + N - 3);

    std::iter_swap(it_west + N - 1, it_west + N - 3);

    // CHECK ME !!
    double shift = m_status.get_central_disk_shift();
    shift = fmod(shift + 120, 360.0);
    m_status.set_central_disk_shift(shift);
  } else {
    assert(false);
  }
  double alpha = this->get_angle_for_origin(LEAF::NORTH);
  m_status.set_shift_for_leaf(LEAF::NORTH, alpha);
  m_status.set_shift_for_leaf(LEAF::EAST, alpha);
  m_status.set_shift_for_leaf(LEAF::WEST, alpha);
  update_rotation_status(LEAF::NORTH);
  update_rotation_status(LEAF::EAST);
  update_rotation_status(LEAF::WEST);
}

template <std::size_t N, std::size_t M>
void SpinPuzzleSide<N, M>::prepare_from_border_rotation() {
  reset_marbles_from_border_rotation();
  reorder_marbles_after_border_reset();
}

template <std::size_t N, std::size_t M>
void SpinPuzzleSide<N, M>::reset_marbles_for_border_rotation() {
  std::array<puzzle::SpinMarble, N_LEAVES * N> tmp;
  // start positioning the marbles that are fixed in the leaves
  auto it_north = this->begin(LEAF::NORTH);
  auto it_east = this->begin(LEAF::EAST);
  auto it_west = this->begin(LEAF::WEST);
  auto begin_tmp = tmp.begin();
  for (size_t n = 0; n < N; ++it_north, ++it_east, ++it_west, ++n) {
    std::iter_swap(begin_tmp + n, it_north);
    std::iter_swap(begin_tmp + N + n, it_east);
    std::iter_swap(begin_tmp + 2 * N + n, it_west);
  }
  std::swap(tmp, m_marbles);
  m_status.set_shift_for_leaf(LEAF::NORTH, 0.0);
  m_status.set_shift_for_leaf(LEAF::EAST, 0.0);
  m_status.set_shift_for_leaf(LEAF::WEST, 0.0);
}

template <std::size_t N, std::size_t M>
void SpinPuzzleSide<N, M>::prepare_for_border_rotation() {
  reset_marbles_for_border_rotation();
  reorder_marbles_after_border_reset();
}

template <std::size_t N, std::size_t M>
void SpinPuzzleSide<N, M>::current_time_step(
    size_t start_index, std::array<Color, puzzle::SIZE_STEP_ARRAY> &out) {
  if (get_trifoild_status() == puzzle::TREFOIL::BORDER_ROTATION) {
    auto it = begin(LEAF::TREFOIL);
    for (size_t n_leaf = 0; n_leaf < 3; ++n_leaf) {
      for (size_t n = 0; n < N - M; ++n, ++it, ++start_index) {
        out[start_index] = it->color();
      }
      start_index += 3;
      for (size_t n = N - M; n < N; ++n, ++it, ++start_index) {
        out[start_index] = it->color();
      }
    }
  } else {
    auto it = begin(LEAF::NORTH);
    for (size_t n = 0; n < N; ++n, ++it, ++start_index) {
      out[start_index] = it->color();
    }
    start_index += 3;
    it = begin(LEAF::EAST);
    for (size_t n = 0; n < N; ++n, ++it, ++start_index) {
      out[start_index] = it->color();
    }
    start_index += 3;
    it = begin(LEAF::WEST);
    for (size_t n = 0; n < N; ++n, ++it, ++start_index) {
      out[start_index] = it->color();
    }
  }
}

} // namespace puzzle
#endif // SPINPUZZLESIDE_H
