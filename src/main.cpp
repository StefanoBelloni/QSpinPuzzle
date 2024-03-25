#include <QApplication>
#include <QLocale>
#include <QScreen>
#include <QTranslator>

#include "widgets/spin_puzzle_window.h"

const char* style = R"(

QSlider { height: 40px; }
QSlider::groove:horizontal {
    border: 1px solid #999999;
    height: 8px;
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);
    margin: 2px 0;
}
QSlider::handle:horizontal {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);
    border: 1px solid #5c5c5c;
    width: 18px;
    margin: -2px 0;
    border-radius: 3px;
}

QSlider::add-page:horizontal { background: red; }
QSlider::sub-page:horizontal { background: green; }
)";

int
main(int argc, char* argv[])
{
  QApplication a(argc, argv);

  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();
  for (const QString& locale : uiLanguages) {
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
  QScreen* screen = QGuiApplication::primaryScreen();
  QRect screenGeometry = screen->geometry();
  a.setStyleSheet(style);
  w.setStyleSheet("border : 1px solid black;");
  int min_size =
    std::min(screenGeometry.height() / 2, screenGeometry.width() / 2) + 10;
  w.resize(QSize(min_size, min_size));
#endif
  w.show();
  return a.exec();
}
