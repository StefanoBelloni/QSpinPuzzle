#ifndef SPIN_PUZZLE_FILESYSTEM_H
#define SPIN_PUZZLE_FILESYSTEM_H

#include <QString>
#include <string>

namespace puzzle {
class FileSystem
{
public:
  FileSystem();
  FileSystem(const QString& basedir);

  std::string get_puzzle_file() const;
  std::string get_records_puzzle_file() const;
  std::string get_current_puzzle_file() const;
  std::string get_config_puzzle_file() const;
  std::string get_recoding_puzzle_directory() const;
  std::string get_recoding_puzzle(const std::string& name) const;
  std::string get_recoding_puzzle_name() const;

  void create_filesystem() const;

private:
  QString m_basedir;
};
}

#endif // SPIN_PUZZLE_FILESYSTEM_H
