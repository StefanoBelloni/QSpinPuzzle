#ifndef ACTIONPROVIDER_H
#define ACTIONPROVIDER_H

#include <QKeyEvent>
#include <vector>

class ActionProvider {
public:
  ActionProvider();

  std::vector<int> getSequenceOfCommands(int seed, int size);

  static constexpr int N = 8;

  int keys[N] = {
      Qt::Key_N,      Qt::Key_E,    Qt::Key_W,     Qt::Key_I,
      Qt::Key_PageUp, Qt::Key_Left, Qt::Key_Right, Qt::Key_P,
  };
};

#endif // ACTIONPROVIDER_H