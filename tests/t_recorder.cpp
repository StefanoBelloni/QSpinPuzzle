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
  game.shuffle_with_commands(42, 1000, true);
  recorder = game.detached_recorder();
  auto game_as_string = game.to_string();
  // ================================= //
  puzzle::SpinPuzzleGame replayedGame;
  out.seekg(0, std::ios::beg);
  replayedGame.load(out);
  recorder->replay_record(replayedGame);
  auto replayedgame_as_string = replayedGame.to_string();
  ASSERT_EQ(game_as_string, replayedgame_as_string);
}
