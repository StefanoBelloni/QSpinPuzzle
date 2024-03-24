#include <gtest/gtest.h>

#include <sstream>

#include "puzzle/spin_puzzle_cipher.h"
#include "puzzle/spin_puzzle_game.h"

using namespace puzzle;

TEST(PuzzleCipher, cipher_basic)
{
  Cipher cipher(Cipher::VERSION::v0);
  std::stringstream out;
  SpinPuzzleGame game;
  game.shuffle();
  std::string shuffled_game = game.to_string();
  game.serialize(out);
  std::string enciphered = cipher.encrypt(out.str());
  game.reset();
  out.clear();
  std::string deciphered = cipher.decrypt(out.str());
  out << deciphered;
  game.load(out);
  ASSERT_EQ(game.to_string(), shuffled_game);
}