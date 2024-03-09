#include "spin_puzzle_cipher.h"

namespace puzzle {

std::string
Cipher::encrypt(const std::string& input)
{
  if (m_version == Cipher::VERSION::v0) {
    return caesar_encrypt(input);
  }
  return input;
}

std::string
Cipher::decrypt(const std::string& input)
{
  if (m_version == Cipher::VERSION::v0) {
    return caesar_decrypt(input);
  }
  return input;
}

std::string
Cipher::caesar_encrypt(const std::string& input)
{
  std::string output;
  int n = 0;
  for (char c : input) {
    if (c == '\n') {
      output += '\n';
    } else {
      int position = c - CAESAR_ALPHABET[0];
      output += CAESAR_ALPHABET[(position - CAESAR_KEY + SIZE_CAESAR_ALPHABET) %
                                SIZE_CAESAR_ALPHABET];
    }
  }
  return output;
}
std::string
Cipher::caesar_decrypt(const std::string& input)
{
  std::string output;
  for (char c : input) {
    if (c == '\n') {
      output += '\n';
    } else {
      int position = c - CAESAR_ALPHABET[0];
      output += CAESAR_ALPHABET[(position + CAESAR_KEY + SIZE_CAESAR_ALPHABET) %
                                SIZE_CAESAR_ALPHABET];
    }
  }
  return output;
}
} // namespace puzzle