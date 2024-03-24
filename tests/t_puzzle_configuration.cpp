#include <gtest/gtest.h>

#include <sstream>

#include "puzzle/spin_configuration.h"

using namespace puzzle;

TEST(PuzzleSide, configuration_basic)
{
  Configuration cnf;
  cnf.set_name("QSpinTest");
  cnf.set_level(33);
  cnf.set_speed(3.14);
  cnf.set_realism(42);
  std::stringstream s;
  cnf.dump(s);

  Configuration copy;
  copy.load(s);

  ASSERT_EQ(cnf.name(), copy.name());
  ASSERT_EQ(cnf.level(), copy.level());
  ASSERT_EQ(cnf.speed(), copy.speed());
  ASSERT_EQ(cnf.realism(), copy.realism());
}