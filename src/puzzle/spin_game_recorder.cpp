#include "spin_game_recorder.h"
#include "spin_puzzle_game.h"

namespace puzzle {

void
Recorder::rotate_marbles(LEAF leaf, double angle)
{
  m_events.emplace_back(EventType::ROTATE_MARBLES, angle, leaf);
}

void
Recorder::rotate_internal_disk(double angle)
{
  m_events.emplace_back(EventType::ROTATE_INTERNAL_DISK, angle);
}

void
Recorder::spin_leaf(LEAF leaf, double angle)
{
  m_events.emplace_back(EventType::SPIN_LEAF, angle, leaf);
}

void
Recorder::spin_leaf(LEAF leaf)
{
  this->spin_leaf(leaf, 180.0);
}

void
Recorder::swap_side()
{
  m_events.emplace_back(EventType::SWAP_SIDE);
}
void
Recorder::reset()
{
  m_events.clear();
}

void Recorder::rec(const SpinPuzzleGame& game) {
  game.serialize(m_start_game);
  m_recording = true;
}

void
Recorder::replay(SpinPuzzleGame& game)
{
  m_start_game.seekg(0, std::ios::beg);
  game.load(m_start_game);
  play(game, m_events.begin(), m_events.end());
}

void
Recorder::play(SpinPuzzleGame& game, std::vector<Event>::iterator begin, std::vector<Event>::iterator end)
{
  for (m_current = begin; m_current != end && m_current != m_events.end(); ++m_current) {
    const Event& event = *m_current;
    switch (event.type()) {
      case Recorder::EventType::ROTATE_MARBLES:
        game.rotate_marbles(event.leaf(), event.angle());
        break;
      case Recorder::EventType::ROTATE_INTERNAL_DISK:
        game.rotate_internal_disk(event.angle());
        break;
      case Recorder::EventType::SPIN_LEAF_ANGLE:
        game.spin_leaf(event.leaf(), event.angle());
        break;
      case Recorder::EventType::SPIN_LEAF:
        game.spin_leaf(event.leaf());
        break;
      case Recorder::EventType::SWAP_SIDE:
        game.swap_side();
        break;
      default:
        break;
    }
  }
}

bool Recorder::step_forward(SpinPuzzleGame& game, size_t steps) {
  play(game, m_current, m_current + steps);
  return m_current != m_events.end();
}

void Recorder::rewind() {
  m_current = m_events.begin();
}

void Recorder::rewind(SpinPuzzleGame& game) {
  m_start_game.seekg(0, std::ios::beg);
  game.load(m_start_game);
  m_current = m_events.begin();
}

Recorder::EventType
Recorder::Event::type() const
{
  return m_eventType;
}

LEAF
Recorder::Event::leaf() const
{
  return m_leaf;
}
double
Recorder::Event::angle() const
{
  return m_angle;
}
}