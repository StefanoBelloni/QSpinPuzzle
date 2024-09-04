#include "spin_puzzle_definitions.h"
#include <chrono>
#include <vector>

namespace puzzle {

class SpinPuzzleGame;

class Recorder
{
public:
  enum class EventType
  {
    ROTATE_MARBLES,
    ROTATE_INTERNAL_DISK,
    SPIN_LEAF_ANGLE,
    SPIN_LEAF,
    SWAP_SIDE
  };
  class Event
  {
  public:
    Event(EventType eventType, double angle = 0.0, LEAF leaf = LEAF::INVALID)
      : m_eventType(eventType)
      , m_angle(angle)
      , m_leaf(leaf)
#ifdef QSPIN_PUZZLE_RECORD_TIMES
      , m_time(std::chrono::system_clock::now())
#endif
    {
    }
    EventType type() const;
    LEAF leaf() const;
    double angle() const;

  private:
    EventType m_eventType;
    LEAF m_leaf;
    double m_angle;
#ifdef QSPIN_PUZZLE_RECORD_TIMES
    std::chrono::time_point<std::chrono::system_clock> m_time;
#endif
  };

  void rotate_marbles(LEAF leaf, double angle);
  void rotate_internal_disk(double angle);
  void spin_leaf(LEAF leaf, double angle);
  void spin_leaf(LEAF leaf);
  void swap_side();
  void reset();

  void replay_record(SpinPuzzleGame& game);

private:
  std::vector<Event> m_events;
};
}