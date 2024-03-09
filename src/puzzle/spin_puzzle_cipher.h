#ifndef SPINPUZZLECYPHER_H
#define SPINPUZZLECYPHER_H

#include <string>

namespace puzzle {

class Cipher
{
public:
  enum class VERSION
  {
    NONE = 0,
    v0 = 1,
    INVALID = 2
  };

  Cipher() = delete;
  explicit Cipher(Cipher::VERSION version)
    : m_version(version){};

  std::string encrypt(const std::string& input);
  std::string decrypt(const std::string& input);

private:
  static constexpr int SIZE_CAESAR_ALPHABET = 96;
  static constexpr int CAESAR_KEY = SIZE_CAESAR_ALPHABET / 2;
  // static constexpr int CAESAR_KEY = 33;
  char CAESAR_ALPHABET[SIZE_CAESAR_ALPHABET] = {
    " !\"#$%&'()*+,-./"
    "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
    "abcdefghijklmnopqrstuvwxyz{|}~"
  };

  std::string caesar_encrypt(const std::string& input);
  std::string caesar_decrypt(const std::string& input);

  Cipher::VERSION m_version;
};
} // namespace puzzle

#endif