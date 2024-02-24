#include "widgets/spin_puzzle_window.h"

#include <QApplication>
#include <QLocale>
#include <QScreen>
#include <QTranslator>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();
  for (const QString &locale : uiLanguages) {
    const QString baseName = "QSpinPuzzle_" + QLocale(locale).name();
    if (translator.load(":/i18n/" + baseName)) {
      a.installTranslator(&translator);
      break;
    }
  }
  SpinPuzzleWindow w;
#ifdef Q_OS_ANDROID
  w.setWindowState(Qt::WindowMaximized);
#else
  QScreen *screen = QGuiApplication::primaryScreen();
  QRect screenGeometry = screen->geometry();
  w.setStyleSheet("border : 1px solid black;");
  int min_size =
      std::min(screenGeometry.height() / 2, screenGeometry.width() / 2) + 10;
  w.resize(QSize(min_size, min_size));
#endif
  w.show();
  return a.exec();
}
