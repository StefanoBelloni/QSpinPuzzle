#include "spin_action_provider.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

ActionProvider::ActionProvider() {}

std::vector<int> ActionProvider::getSequenceOfCommands(int seed, int size) {
  if (seed == 0) {
    // First create an instance of an engine.
    std::random_device rnd_device;
    // Specify the engine and distribution.
    std::mt19937 mersenne_engine{rnd_device()}; // Generates random integers
    std::uniform_int_distribution<int> dist{0, this->N - 1};
    auto gen = [this, &dist, &mersenne_engine]() {
      return keys[dist(mersenne_engine)];
    };
    std::vector<int> actions(size);
    std::generate(std::begin(actions), std::end(actions), gen);
    return actions;
  }

  // use the seed if not 0
  std::default_random_engine rnd_device(seed);
  // Specify the engine and distribution.
  std::mt19937 mersenne_engine{rnd_device()}; // Generates random integers
  std::uniform_int_distribution<int> dist{0, this->N - 1};
  auto gen = [this, &dist, &mersenne_engine]() {
    return keys[dist(mersenne_engine)];
  };
  std::vector<int> actions(size);
  std::generate(std::begin(actions), std::end(actions), gen);
  return actions;
}