#include "spin_puzzle_filesystems.h"
#include <QDir>
#include <QStandardPaths>
#include <QString>
#include <string>

namespace puzzle {
FileSystem::FileSystem()
{
  m_basedir =
    QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
}

FileSystem::FileSystem(const QString& basedir)
{
  m_basedir = m_basedir;
}

std::string
FileSystem::get_config_puzzle_file() const
{
  auto filename = QDir(m_basedir + "/config_puzzle.txt");
  return filename.path().toStdString();
}

void
FileSystem::create_filesystem() const
{
  auto dir = QDir(m_basedir);
  if (!dir.exists()) {
    dir.mkpath(".");
  }
  auto dir_statistcs = QDir(m_basedir + "/statistics");
  if (!dir_statistcs.exists()) {
    dir_statistcs.mkpath(".");
  }
}

std::string
FileSystem::get_current_puzzle_file() const
{
  auto filename = QDir(m_basedir + "/current_puzzle.txt");
  return filename.path().toStdString();
}

std::string
FileSystem::get_puzzle_file() const
{
  auto filename = QDir(m_basedir + "/puzzle.txt");
  return filename.path().toStdString();
}

std::string
FileSystem::get_records_puzzle_file() const
{
  auto filename = QDir(m_basedir + "/records.txt");
  return filename.path().toStdString();
}

}
