#include <gtest/gtest.h>

#include <sstream>

#include "puzzle/spin_puzzle_record.h"

using namespace puzzle;

TEST(PuzzleRecord, records_serialization_sstring)
{
  std::stringstream s;
  SpinPuzzleGame game;
  game.shuffle();
  auto record1 = SpinPuzzleRecord("QSpinPuzzleTest", 12345, 6, game);

  SpinPuzzleRecord record2{};
  ASSERT_EQ(record2.username(), "");
  ASSERT_EQ(record2.time(), 0);
  ASSERT_EQ(record2.level(), -1);

  ASSERT_TRUE(record1.serialize(s));

  record2.load(s);
  auto game2 = record2.game();

  ASSERT_EQ(record2.username(), "QSpinPuzzleTest");
  ASSERT_EQ(record2.time(), 12345);
  ASSERT_EQ(record2.level(), 6);
  ASSERT_EQ(game2.current_time_step(), game.current_time_step());
}

TEST(PuzzleRecord, records_serialization_fstream)
{
  // TODO: warning: the use of `tmpnam' is dangerous, better use `mkstemp'
  std::string filename = std::tmpnam(nullptr);
  std::ofstream out(filename);
  ASSERT_TRUE(out.is_open());

  SpinPuzzleGame game;
  game.shuffle();
  auto record1 = SpinPuzzleRecord("QSpinPuzzleTest", 12345, 6, game);
  record1.update_time(54321);
  record1.update_username("TestPuzzleQSpin");
  SpinPuzzleRecord record2{};
  ASSERT_EQ(record2.username(), "");
  ASSERT_EQ(record2.time(), 0);
  ASSERT_EQ(record2.level(), -1);

  record1.serialize(out);
  out.close();

  std::ifstream in(filename);
  ASSERT_TRUE(in.is_open());

  record2.load(in);
  auto game2 = record2.game();

  ASSERT_EQ(record2.username(), "TestPuzzleQSpin");
  ASSERT_EQ(record2.time(), 54321);
  ASSERT_EQ(record2.level(), 6);
  ASSERT_EQ(game2.current_time_step(), game.current_time_step());

  std::remove(filename.c_str());
}

TEST(PuzzleRecord, records_multiple_serialization_fstream)
{
  size_t n_games = 10;
  std::vector<SpinPuzzleGame> games;
  // TODO: warning: the use of `tmpnam' is dangerous, better use `mkstemp'
  std::string filename = std::tmpnam(nullptr);
  // ================================================ //
  // SERIALIZE
  std::ofstream out(filename);
  ASSERT_TRUE(out.is_open());
  for (size_t n = 0; n < n_games; ++n) {
    SpinPuzzleGame game;
    game.shuffle();
    auto record1 =
      SpinPuzzleRecord("QSpinPuzzleTest" + std::to_string(n), n * n, n, game);
    games.emplace_back(game);
    record1.serialize(out);
  }
  out.close();
  // ================================================ //

  // ================================================ //
  // LOAD
  std::ifstream in(filename);
  ASSERT_TRUE(in.is_open());
  for (size_t n = 0; n < n_games; ++n) {
    auto record1 = SpinPuzzleRecord();
    record1.load(in);
    ASSERT_EQ(record1.username(), "QSpinPuzzleTest" + std::to_string(n));
    ASSERT_EQ(record1.time(), n * n);
    ASSERT_EQ(record1.level(), n);
    auto game = record1.game();
    ASSERT_EQ(games[n].current_time_step(), game.current_time_step());
  }
  // ================================================ //

  in.close();
  std::remove(filename.c_str());
}