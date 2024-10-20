QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QSpinPuzzle
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += src/
INCLUDEPATH += src/puzzle
INCLUDEPATH += src/widgets

ANDROID_VERSION_CODE = 16410

SOURCES += \
    src/puzzle/spin_action_provider.cpp \
    src/puzzle/spin_marble.cpp \
    src/puzzle/spin_puzzle_definitions.cpp \
    src/puzzle/spin_puzzle_game.cpp \
    src/puzzle/spin_puzzle_matrix.cpp \
    src/puzzle/spin_puzzle_side.cpp \
    src/puzzle/spin_puzzle_cipher.cpp \
    src/puzzle/spin_configuration.cpp \
    src/puzzle/spin_puzzle_record.cpp \
    src/puzzle/spin_puzzle_record.h \
    src/puzzle/spin_metrics.cpp \
    src/puzzle/spin_metrics.h \
    src/puzzle/spin_game_recorder.h \
    src/puzzle/spin_game_recorder.cpp \
    src/widgets/spin_puzzle_history_widget.cpp \
    src/widgets/spin_puzzle_replay_widget.cpp \
    src/widgets/spin_puzzle_widget.cpp \
    src/widgets/spin_puzzle_window.cpp \
    src/widgets/spin_puzzle_config_widget.cpp \
    src/widgets/spin_puzzle_filesystems.cpp  \
    src/main.cpp

HEADERS  += \
    src/puzzle/spin_action_provider.h \
    src/puzzle/spin_marble.h \
    src/puzzle/spin_puzzle_definitions.h \
    src/puzzle/spin_puzzle_game.h \
    src/puzzle/spin_puzzle_matrix.h \
    src/puzzle/spin_puzzle_side.h \
    src/puzzle/spin_puzzle_cipher.h \
    src/puzzle/spin_configuration.h \
    src/widgets/spin_puzzle_replay_widget.h \
    src/widgets/spin_puzzle_history_widget.h \
    src/widgets/spin_puzzle_widget.h \
    src/widgets/spin_puzzle_window.h \
    src/widgets/spin_puzzle_filesystems.h  \
    src/puzzle/spin_game_recorder.h \
    src/widgets/spin_puzzle_config_widget.h

DISTFILES += \
    README.md \
    images/QSpinPuzzle_Front.png \
    images/records_import_export.png \
    images/spin_puzzle_play.gif \
    images/spin_puzzle.png

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}

RESOURCES += \
    resources/images.qrc
