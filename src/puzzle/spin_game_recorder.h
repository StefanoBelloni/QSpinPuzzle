#include "spin_puzzle_definitions.h"
#include <chrono>
#include <vector>
#include <sstream>
#include <iostream>

namespace puzzle {

class SpinPuzzleGame;

class Recorder
{
public:
  enum class EventType
  {
    ROTATE_MARBLES = 0,
    ROTATE_INTERNAL_DISK = 1,
    SPIN_LEAF_ANGLE = 2,
    SPIN_LEAF = 3,
    SWAP_SIDE = 4
  };
  class Event
  {
  public:
    Event(EventType eventType, double angle = 0.0, LEAF leaf = LEAF::INVALID)
      : m_eventType(eventType)
      , m_angle(angle)
      , m_leaf(leaf)
#ifdef QSPIN_PUZZLE_RECORD_TIMES
      , m_time(std::chrono::system_clock::now().time_since_epoch().count())
#else
      , m_time(0)
#endif
    {
    }
    explicit Event() {}
    EventType type() const;
    LEAF leaf() const;
    double angle() const;

    template<typename Buffer>
    Buffer& serialize(Buffer& buffer, bool times=true) const {
      buffer << static_cast<int32_t>(m_eventType) << " "
             << m_angle << " "
             << static_cast<int32_t>(m_leaf);
      if (times) { buffer << " " << m_time;
      } else { buffer << " " << 0; }
      buffer << "\n";
      return buffer;
    }

  template<typename Buffer>
  void load(Buffer& buffer) {
    int32_t type;
    int32_t leaf;
    buffer >> type;
    buffer >> m_angle;
    buffer >> leaf;
    buffer >> m_time;
    m_eventType = static_cast<EventType>(type);
    m_leaf = static_cast<LEAF>(leaf);
  }

  private:
    EventType m_eventType;
    LEAF m_leaf;
    double m_angle;
#ifdef QSPIN_PUZZLE_RECORD_TIMES
    size_t m_time;
#endif
  };

  void rotate_marbles(LEAF leaf, double angle);
  void rotate_internal_disk(double angle);
  void spin_leaf(LEAF leaf, double angle);
  void spin_leaf(LEAF leaf);
  void swap_side();
  void reset();

  void rec(const SpinPuzzleGame& game);
  void stop() { 
    m_start_game.seekg(0, std::ios::beg);
    m_recording = false;
  };
  void replay(SpinPuzzleGame& game);
  bool step_forward(SpinPuzzleGame& game, size_t steps);
  void rewind();
  void rewind(SpinPuzzleGame& game);
  bool isRecording() const {return m_recording;}

  template<typename Buffer>
  Buffer& serialize(Buffer& buffer, bool times=true) const
  {
    buffer << m_start_game.str();
    buffer << m_events.size() << "\n";
    for (const auto& e : m_events) {
      e.serialize(buffer, times);
    }
    return buffer;
  }

  template<typename Buffer>
  void load(Buffer& buffer)
  {
    std::string game;
    std::string event;
    size_t size;
    std::getline(buffer, game);
    m_start_game.clear();
    m_start_game << game << "\n";
    buffer >> size;
    m_events.clear();
    for (size_t n = 0; n < size; ++n) {
      Event e;
      e.load(buffer);
      m_events.emplace_back(e);
    }
  }


private:
  void play(SpinPuzzleGame& game, std::vector<Event>::iterator begin, std::vector<Event>::iterator end);
  std::vector<Event> m_events;
  std::vector<Event>::iterator m_current = m_events.end();
  std::stringstream m_start_game;
  bool m_recording = false;

};
}