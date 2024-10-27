#include <gtest/gtest.h>

#include "puzzle/spin_game_recorder.h"
#include "puzzle/spin_puzzle_game.h"

TEST(PuzzleRecorder, ShuffleAndReplay)
{
  std::stringstream out;
  std::shared_ptr<puzzle::Recorder> recorder =
    std::make_shared<puzzle::Recorder>();
  puzzle::SpinPuzzleGame game;
  game.shuffle();
  game.serialize(out);
  // ================================= //
  game.attach_recorder(recorder);
  game.start_recording();
  game.shuffle_with_commands(42, 1000, true);
  recorder = game.detached_recorder();
  auto game_as_string = game.to_string();
  // ================================= //
  puzzle::SpinPuzzleGame replayedGame;
  recorder->replay(replayedGame);
  auto replayedgame_as_string = replayedGame.to_string();
  ASSERT_EQ(game_as_string, replayedgame_as_string);
}

TEST(PuzzleRecorder, PlayForward)
{
  std::stringstream out;
  std::shared_ptr<puzzle::Recorder> recorder =
    std::make_shared<puzzle::Recorder>();
  puzzle::SpinPuzzleGame game;
  game.shuffle();
  game.serialize(out);
  // ================================= //
  game.attach_recorder(recorder);
  game.start_recording();
  game.shuffle_with_commands(42, 1000, true);
  recorder = game.detached_recorder();
  auto game_as_string = game.to_string();
  // ================================= //
  puzzle::SpinPuzzleGame replayedGame;
  recorder->rewind(replayedGame);
  while (recorder->step_forward(replayedGame, 1)) {
  }
  auto replayedgame_as_string = replayedGame.to_string();
  ASSERT_EQ(game_as_string, replayedgame_as_string);
}

TEST(PuzzleRecorder, Serialize)
{
  std::string expected =
    R"(v0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 0 9 1 9 2 9 3 9 4 9 5 9 6 9 7 9 8 9 9 9 10 8 11 8 12 8 13 8 14 8 15 8 16 8 17 8 18 8 19 8 20 11 21 11 22 11 23 11 24 11 25 11 26 11 27 11 28 11 29 11 0 0 0 0 0 1 0 0 0 1 30 10 31 10 32 10 33 10 34 10 35 10 36 10 37 10 38 10 39 10 40 7 41 7 42 7 43 7 44 7 45 7 46 7 47 7 48 7 49 7 50 12 51 12 52 12 53 12 54 12 55 12 56 12 57 12 58 12 59 12 
10
0 36 0 0
3 180 0 0
0 -36 1 0
0 36 0 0
4 0 5 0
1 -0 5 0
0 36 2 0
1 0 5 0
4 0 5 0
1 -0 5 0
)";
  std::stringstream out;
  std::shared_ptr<puzzle::Recorder> recorder =
    std::make_shared<puzzle::Recorder>();
  puzzle::SpinPuzzleGame game;
  game.attach_recorder(recorder);
  game.start_recording();
  game.shuffle_with_commands(42, 10, true);
  recorder = game.detached_recorder();
  recorder->serialize(out, false);
  out.seekg(0, std::ios::beg);
  ASSERT_EQ(expected, out.str());
}

TEST(PuzzleRecorder, Load)
{
  std::stringstream outRecorder;
  std::stringstream outRecorderLoaded;
  std::stringstream gameStr;
  std::stringstream gameStrReloaded;
  std::shared_ptr<puzzle::Recorder> recorder =
    std::make_shared<puzzle::Recorder>();

  puzzle::SpinPuzzleGame game;
  game.attach_recorder(recorder);
  game.start_recording();
  game.shuffle_with_commands(42, 10, true);
  game.serialize(gameStr);
  recorder = game.detached_recorder();
  recorder->serialize(outRecorder, false);

  puzzle::Recorder recorderLoaded{};
  outRecorder.seekg(0, std::ios::beg);
  recorderLoaded.load(outRecorder);

  recorderLoaded.serialize(outRecorderLoaded);
  outRecorder.seekg(0, std::ios::beg);
  outRecorderLoaded.seekg(0, std::ios::beg);
  ASSERT_EQ(outRecorder.str(), outRecorderLoaded.str());

  puzzle::SpinPuzzleGame gameReloaded;
  recorderLoaded.replay(gameReloaded);
  gameReloaded.serialize(gameStrReloaded);

  gameStrReloaded.seekg(0, std::ios::beg);
  gameStr.seekg(0, std::ios::beg);
  ASSERT_EQ(gameStrReloaded.str(), gameStr.str());
}