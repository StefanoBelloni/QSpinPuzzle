#ifndef SPIN_PUZZLE_RECORDER_H
#define SPIN_PUZZLE_RECORDER_H

#include "spin_puzzle_definitions.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

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

  Recorder() = default;
  Recorder(const Recorder& recorder)
    : m_events(recorder.m_events)
    , m_current(recorder.m_current)
    , m_recording{ false }
  {
    m_start_game.clear();
    m_start_game << recorder.m_start_game.str();
  }

  class Event
  {
  public:
    Event(EventType eventType, double angle = 0.0, LEAF leaf = LEAF::INVALID)
      : m_eventType(eventType)
      , m_angle(angle)
      , m_leaf(leaf)
#ifdef QSPIN_PUZZLE_RECORD_TIMES
      , m_time(std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count())
#else
      , m_time(0)
#endif
    {
    }
    explicit Event() {}
    EventType type() const;
    LEAF leaf() const;
    double angle() const;
    size_t time() const;

    template<typename Buffer>
    Buffer& serialize(Buffer& buffer, bool times = true) const
    {
      buffer << static_cast<int32_t>(m_eventType) << " "
             << std::setprecision(std::numeric_limits<double>::digits10)
             << m_angle << " " << static_cast<int32_t>(m_leaf);
      if (times) {
        buffer << " " << m_time;
      } else {
        buffer << " " << 0;
      }
      buffer << "\n";
      return buffer;
    }

    template<typename Buffer>
    void load(Buffer& buffer)
    {
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
    size_t m_time;
  };

  void rotate_marbles(LEAF leaf, double angle);
  void rotate_internal_disk(double angle);
  void spin_leaf(LEAF leaf, double angle);
  void spin_leaf(LEAF leaf);
  void swap_side();
  void reset();
  size_t current_time() const
  {
    if (isEnd()) {
      return (m_events.end() - 1)->time() - m_events.begin()->time();
    }
    return m_current->time() - m_events.begin()->time();
  }

  void rec(const SpinPuzzleGame& game);
  void stop()
  {
    if (!m_recording) {
      return;
    }
    m_start_game.seekg(0, std::ios::beg);
    m_recording = false;
  };
  void replay(SpinPuzzleGame& game);
  size_t play(SpinPuzzleGame& game, size_t time);
  size_t size() const
  {
    return m_events.size();
  }
  size_t current() const
  {
    return m_current - m_events.begin();
  }
  bool step_forward(SpinPuzzleGame& game, size_t steps);
  void rewind();
  void rewind(SpinPuzzleGame& game);
  bool isRecording() const
  {
    return m_recording;
  }
  bool isEnd() const
  {
    return m_current == m_events.end();
  }

  template<typename Buffer>
  Buffer& serialize(Buffer& buffer, bool times = true) const
  {
    buffer << m_start_game.str();
    buffer << m_events.size() << "\n";
    for (const auto& e : m_events) {
      e.serialize(buffer, times);
    }
    return buffer;
  }

  std::FILE* serialize(std::FILE* file) const;

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
  void play(SpinPuzzleGame& game,
            std::vector<Event>::iterator begin,
            std::vector<Event>::iterator end);

  std::vector<Event> m_events;
  std::vector<Event>::iterator m_current = m_events.end();
  std::stringstream m_start_game;
  bool m_recording = false;
};
}

#endif // RECORDER
